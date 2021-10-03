#include "physics.h"
#include "game.h"
#include "components.h"
#include "gfx/mesh.h"
#include "gfx/camera.h"
#include "world.h"

#include <array>
#include <unordered_map>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GLFW/glfw3.h>

#include <imgui.h>

// hack to make physx happy
#if !NDEBUG
  #define _DEBUG
#endif

#include <PhysX/physx/include/PxPhysicsAPI.h>
#include <PhysX/physx/include/cooking/PxCooking.h>
#include <PhysX/pxshared/include/foundation/PxAllocatorCallback.h>
#include <PhysX/pxshared/include/foundation/PxErrorCallback.h>
#include <PhysX/physx/include/PxScene.h>
#include <PhysX/physx/include/PxSimulationEventCallback.h>

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

using namespace physx;

struct PhysicsImpl;

namespace
{
  class PxLockRead
  {
  public:
    PxLockRead(PxScene* scn) : scene(scn) { scene->lockRead(); }
    ~PxLockRead() { scene->unlockRead(); }

  private:
    PxScene* scene;
  };
  class PxLockWrite
  {
  public:
    PxLockWrite(PxScene* scn) : scene(scn) { scene->lockWrite(); }
    ~PxLockWrite() { scene->unlockWrite(); }

  private:
    PxScene* scene;
  };

  PxVec3 toPxVec3(const glm::vec3& v)
  {
    return { v.x, v.y, v.z };
  }

  glm::vec3 toGlmVec3(const PxVec3& v)
  {
    return { v.x, v.y, v.z };
  }

  PxQuat toPxQuat(const glm::quat& q)
  {
    return { q.x, q.y, q.z, q.w }; // px quat constructor takes x, y, z, w
  }

  glm::quat toGlmQuat(const PxQuat& q)
  {
    return { q.w, q.x, q.y, q.z }; // glm quat constructor takes w, x, y, z
  }

  PxMat44 toPxMat4(const glm::mat4& m)
  {
    return
    {
      { m[0][0], m[0][1], m[0][2], m[0][3] },
      { m[1][0], m[1][1], m[1][2], m[1][3] },
      { m[2][0], m[2][1], m[2][2], m[2][3] },
      { m[3][0], m[3][1], m[3][2], m[3][3] }
    };
  }

  glm::mat4 toGlmMat4(const PxMat44& m)
  {
    return
    {
      { m[0][0], m[0][1], m[0][2], m[0][3] },
      { m[1][0], m[1][1], m[1][2], m[1][3] },
      { m[2][0], m[2][1], m[2][2], m[2][3] },
      { m[3][0], m[3][1], m[3][2], m[3][3] }
    };
  }
  
  PxFilterFlags contactReportFilterShader(
    [[maybe_unused]] PxFilterObjectAttributes attributes0, [[maybe_unused]] PxFilterData filterData0,
    [[maybe_unused]] PxFilterObjectAttributes attributes1, [[maybe_unused]] PxFilterData filterData1,
    PxPairFlags& pairFlags, [[maybe_unused]] const void* constantBlock, [[maybe_unused]] PxU32 constantBlockSize)
  {
    // all initial and persisting reports for everything, with per-point data
    pairFlags = PxPairFlag::eSOLVE_CONTACT
      | PxPairFlag::eDETECT_DISCRETE_CONTACT
      | PxPairFlag::eNOTIFY_TOUCH_FOUND
      //| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
      | PxPairFlag::eNOTIFY_CONTACT_POINTS
      | PxPairFlag::eDETECT_CCD_CONTACT
      | PxPairFlag::eNOTIFY_THRESHOLD_FORCE_FOUND
      ;
    return PxFilterFlag::eDEFAULT;
  }

  class ErrorCallback : public PxDefaultErrorCallback
  {
  public:
    void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override
    {
      PxDefaultErrorCallback::reportError(code, message, file, line);
    }
  };

}

class ContactReportCallback : public PxSimulationEventCallback
{
public:
  //ContactReportCallback(std::unordered_map<physx::PxRigidActor*, Entity>& ea) : gEntityActors(ea) {}
  ContactReportCallback(PhysicsImpl* physics) : physics_(physics) {}

private:
  //std::unordered_map<physx::PxRigidActor*, Entity>& gEntityActors;
  PhysicsImpl* physics_{};

  void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count);
  void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
  void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
  void onTrigger(PxTriggerPair* pairs, PxU32 count) { PX_UNUSED(pairs); PX_UNUSED(count); }
  void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}
  void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
};


struct PhysicsImpl
{
  ////////////////////////////////////////////////////////
  // objects
  ////////////////////////////////////////////////////////
  const double tick = 1.0 / 50.0;
  bool resultsReady = true;
  double accumulator = 0;

  World* world = nullptr;
  PxController* controller = nullptr;
  glm::vec3 pVel{};
  //glm::vec3 pPos{};
  //glm::vec3 pAccel{};
  PxControllerCollisionFlags cFlags{};
  float pAccum = 0;
  const double pTick = 1.0 / 200.0;

  physx::PxDefaultAllocator gAllocator;
  ErrorCallback gErrorCallback;
  
  physx::PxFoundation* gFoundation = nullptr;
  physx::PxPhysics* gPhysics = nullptr;

  ContactReportCallback* gContactReportCallback{};

  physx::PxDefaultCpuDispatcher* gDispatcher = nullptr;
  physx::PxScene* gScene = nullptr;
  physx::PxPvd* gPvd = nullptr;
  physx::PxCooking* gCooking = nullptr;
  physx::PxControllerManager* gCManager = nullptr;
  std::array<physx::PxMaterial*, 3> gMaterials;

  std::unordered_map<physx::PxRigidActor*, Game::GameObject*> gActorToObject;
  std::unordered_map<Game::GameObject*, physx::PxRigidActor*> gObjectToActor;


  ////////////////////////////////////////////////////////
  // functions
  ////////////////////////////////////////////////////////
  PhysicsImpl()
  {
    gContactReportCallback = new ContactReportCallback(this);
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

#if !NDEBUG
    gPvd = PxCreatePvd(*gFoundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif

    PxTolerancesScale tolerances;
    tolerances.length = 1;
    tolerances.speed = 15.81f;
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, tolerances, true, gPvd);
    gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(tolerances));
    PxInitExtensions(*gPhysics, gPvd);
    gDispatcher = PxDefaultCpuDispatcherCreate(0);

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.gravity = PxVec3(0, -25.0f, 0);
    sceneDesc.filterShader = contactReportFilterShader;
    sceneDesc.simulationEventCallback = gContactReportCallback;
    sceneDesc.solverType = PxSolverType::ePGS; // faster than eTGS
    //sceneDesc.flags |= PxSceneFlag::eREQUIRE_RW_LOCK;

    gScene = gPhysics->createScene(sceneDesc);
    PxLockWrite lkw(gScene);
    gCManager = PxCreateControllerManager(*gScene);
    PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if (pvdClient)
    {
      pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
    }

    gMaterials[(int)Game::MaterialType::PLAYER] = gPhysics->createMaterial(0.2f, 0.2f, 0.0f);
    gMaterials[(int)Game::MaterialType::TERRAIN] = gPhysics->createMaterial(0.4f, 0.4f, 0.6f);
    gMaterials[(int)Game::MaterialType::OBJECT] = gPhysics->createMaterial(0.2f, 0.4f, 0.7f);

    PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterials[(int)Game::MaterialType::TERRAIN]);
    gScene->addActor(*groundPlane);
  }

  ~PhysicsImpl()
  {
    gScene->lockWrite();
    PX_RELEASE(gCManager);
    gScene->unlockWrite();
    PX_RELEASE(gScene);
    PX_RELEASE(gDispatcher);
    PxCloseExtensions();
    PX_RELEASE(gCooking);
    PX_RELEASE(gPhysics);
    if (gPvd)
    {
      PxPvdTransport* transport = gPvd->getTransport();
      PX_RELEASE(gPvd);
      PX_RELEASE(transport);
    }
    PX_RELEASE(gFoundation);

    delete gContactReportCallback;
  }

  Game::collider_t CookMesh(const GFX::Mesh& mesh)
  {
    assert(mesh.indices.size() % 3 == 0);

    PxTolerancesScale scale;
    PxCookingParams params(scale);
    //params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
    //params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
    //params.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
    params.meshWeldTolerance = 0.2f;

    gCooking->setParams(params);

    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = static_cast<PxU32>(mesh.vertices.size());
    meshDesc.points.stride = sizeof(GFX::Vertex);
    meshDesc.points.data = mesh.vertices.data();

    meshDesc.triangles.count = static_cast<PxU32>(mesh.indices.size() / 3);
    meshDesc.triangles.stride = 3 * sizeof(GFX::index_t);
    meshDesc.triangles.data = mesh.indices.data();

    PxTriangleMesh* aTriangleMesh = gCooking->createTriangleMesh(meshDesc, gPhysics->getPhysicsInsertionCallback());
    bool res = gCooking->validateTriangleMesh(meshDesc);
    printf("%d ", res);
    return aTriangleMesh;
  }

  void SetWorld(World* wld)
  {
    assert(world == nullptr && "Only call SetWorld once!");
    
    world = wld;

    PxCapsuleControllerDesc desc;
    desc.upDirection = { 0, 1, 0 };
    desc.density = 10.0f;
    desc.stepOffset = 0.1f;
    desc.material = gMaterials[(int)Game::MaterialType::PLAYER];
    desc.height = 2.0f;
    desc.radius = 0.7f;
    desc.contactOffset = 0.1f;

    controller = gCManager->createController(desc);
    auto vp = world->camera.viewInfo.position;
    controller->setPosition({ vp.x, vp.y, vp.z });
  }

  void SimulatePlayer(float dt)
  {
    const float speed = 10;
    const float gravity = -15;
    const float jump = 8.2;
    const float accelerationGround = 50.0f;
    const float accelerationAir = 20.0f;
    const float decelerationGround = 40.0f;
    const float decelerationAir = 3.0f;
    const float moveSpeed = 5.0;
    const float maxXZSpeed = moveSpeed;

    // "friction"
    //pVel *= 0.98;

    // mouse controls
    auto& vi = world->camera.viewInfo;
    vi.yaw += world->io->MouseDelta.x * .003f;
    vi.pitch = glm::clamp(vi.pitch - world->io->MouseDelta.y * .003f, glm::radians(-89.0f), glm::radians(89.0f));

    const auto fwd = world->camera.viewInfo.GetForwardDir();
    const glm::vec2 xzForward = glm::normalize(glm::vec2(fwd.x, fwd.z));
    const glm::vec2 xzRight = glm::normalize(glm::vec2(-xzForward.y, xzForward.x));

    float acceleration = cFlags & PxControllerCollisionFlag::eCOLLISION_DOWN ? accelerationGround : accelerationAir;
    float curSpeed0 = acceleration * dt;

    glm::vec2 xzForce{ 0 };
    if (world->io->KeysDown[GLFW_KEY_W])
    {
      xzForce += xzForward * moveSpeed;
    }
    if (world->io->KeysDown[GLFW_KEY_S])
    {
      xzForce -= xzForward * moveSpeed;
    }
    if (world->io->KeysDown[GLFW_KEY_D])
    {
      xzForce += xzRight * moveSpeed;
    }
    if (world->io->KeysDown[GLFW_KEY_A])
    {
      xzForce -= xzRight * moveSpeed;
    }

    if (xzForce != glm::vec2(0))
    {
      xzForce = glm::normalize(xzForce) * curSpeed0;
    }

    glm::vec2 tempXZvel{ pVel.x + xzForce[0], pVel.z + xzForce[1] };
    float curSpeed = glm::length(glm::vec2(pVel.x, pVel.z));
    if (auto len = glm::length(tempXZvel); (len > curSpeed && len > maxXZSpeed))
    {
      tempXZvel = tempXZvel / len * curSpeed;
    }
    pVel.x = tempXZvel[0];
    pVel.z = tempXZvel[1];

    //ImGui::Begin("dbg");
    //ImGui::Text("pVel: (%f, %f, %f)", pVel.x, pVel.y, pVel.z);
    //ImGui::Text("curSpeed: %f", glm::length(tempXZvel));
    //ImGui::End();

    pAccum += dt;
    float dtFixed = pTick;
    while (pAccum > pTick)
    {
      pAccum -= pTick;

      pVel.y += gravity * dtFixed;
      glm::vec2 velXZ{ pVel.x, pVel.z };
      float deceleration = 0;

      glm::vec3 pVel2 = pVel * dtFixed;
      cFlags = controller->move({ pVel2.x, pVel2.y, pVel2.z }, 0.001f, dtFixed, PxControllerFilters{});
      if (cFlags & PxControllerCollisionFlag::eCOLLISION_DOWN || cFlags & PxControllerCollisionFlag::eCOLLISION_UP)
      {
        pVel.y = 0;

        if (cFlags & PxControllerCollisionFlag::eCOLLISION_UP)
        {
          controller->move({ 0, -.01, 0 }, 0.001f, dtFixed, PxControllerFilters{});
        }

        // jump if colliding below
        if (cFlags & PxControllerCollisionFlag::eCOLLISION_DOWN && world->io->KeysDown[GLFW_KEY_SPACE])
        {
          pVel.y = jump;
        }

        deceleration = decelerationGround;
      }
      else
      {
        // use air friction
        deceleration = decelerationAir;
      }

      // use friction if no movement was input, or if above max speed
      if (deceleration != 0 && (xzForce == glm::vec2(0) || glm::length(velXZ) > maxXZSpeed))
      {
        //glm::vec2 dV = velXZ * glm::clamp((1.0f - decceleration) * dt, 0.001f, 1.0f); // exponential friction
        glm::vec2 dV;
        if (glm::all(glm::epsilonEqual(velXZ, glm::vec2(0), .001f)))
        {
          dV = { 0,0 };
        }
        else
        {
          dV = glm::clamp(glm::abs(glm::normalize(velXZ)) * deceleration * dtFixed, 0.001f, 1.0f);// linear friction
        }
        velXZ -= glm::min(glm::abs(dV), glm::abs(velXZ)) * glm::sign(velXZ);
      }
      pVel.x = velXZ.x;
      pVel.z = velXZ.y;

      if (cFlags & PxControllerCollisionFlag::eCOLLISION_SIDES)
      {
      }

      // if the actual position is less than if you added velocity to previous position (i.e. you collided with something),
      // then lower the velocity correspondingly
      //const auto& pe = controller->getPosition();
      //glm::vec3 actualPosition{ pe.x, pe.y, pe.z };
      //glm::vec3 actualVelocity = (actualPosition - startPosition) / dtFixed;
      //if (glm::length(glm::vec2(actualVelocity.x, actualVelocity.z)) < glm::length(glm::vec2(velocity.x, velocity.z)))
      //{
      //  velocity.x = actualVelocity.x;
      //  velocity.z = actualVelocity.z;
      //}

      //printf("%f, %f, %f\n", velocity.x, velocity.y, velocity.z);
    }
  }

  void Simulate(float dt)
  {
    if (controller && world)
    {
      SimulatePlayer(dt);
      const auto& p = controller->getPosition();
      world->camera.viewInfo.position = { p.x, p.y, p.z };
    }

    bool asdf = false;
    accumulator += dt;
    accumulator = glm::min(accumulator, tick * 20); // accumulate 20 steps of backlog
    if (accumulator > tick) // NOTE: not while loop, because we want to avoid the Well of Despair
    {
      PxLockWrite lkw(gScene);
      if (resultsReady)
      {
        gScene->simulate(tick);
        resultsReady = false;
      }
      if (gScene->fetchResults(false))
      {
        resultsReady = true;
        accumulator -= tick;
        asdf = true;
      }
    }

    if (!asdf)
      return;

    const int numstatic = gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);

    // update all entity transforms whose actor counterpart was updated
    const auto actorTypes = PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC;
    const auto numActors = gScene->getNbActors(actorTypes);
    if (numActors > 0)
    {
      std::vector<PxRigidActor*> actors(numActors);
      gScene->getActors(actorTypes, reinterpret_cast<PxActor**>(actors.data()), numActors);
      for (auto actor : actors)
      {
        const bool sleeping = actor->is<PxRigidDynamic>() ? actor->is<PxRigidDynamic>()->isSleeping() : false;
        if (sleeping)
          continue;
        const auto& pose = actor->getGlobalPose();

        auto objectIt = gActorToObject.find(actor);
        if (objectIt != gActorToObject.end())
        {
          //auto& tr = entityit->second.GetComponent<Component::Transform>();
          auto* dynamic = actor->is<PxRigidDynamic>();

          glm::quat q(toGlmQuat(pose.q));
          objectIt->second->transform.position = toGlmVec3(pose.p);
          objectIt->second->transform.rotation = q;
        }
      }
    }
  }

  void AddObject(Game::GameObject* object, Game::MaterialType material, Game::collider_t mesh)
  {
    assert(0 && "This function doesn't work!");
    PxTriangleMeshGeometry geom(reinterpret_cast<PxTriangleMesh*>(mesh));
    geom.scale.scale = toPxVec3(object->transform.scale);
    geom.scale.rotation = toPxQuat({ 1, 0, 0, 0 });
    auto pose = PxTransform(toPxVec3(object->transform.position), toPxQuat(object->transform.rotation));
    printf("%d %d", pose.isValid(), geom.isValid());

    PxRigidActor* actor{};
    switch (material)
    {
    case Game::MaterialType::PLAYER:
      break;
    case Game::MaterialType::TERRAIN:
    {
      actor = PxCreateStatic(*gPhysics, pose, geom, *gMaterials[(int)material]);
      break;
    }
    case Game::MaterialType::OBJECT:
    {
      actor = PxCreateDynamic(*gPhysics, pose, geom, *gMaterials[(int)material], 10.0f);
      break;
    }
    default:
      break;
    }

    gActorToObject[actor] = object;
    gObjectToActor[object] = actor;

    gScene->addActor(*actor);
  }

  void AddObject(Game::GameObject* object, Game::MaterialType material, const Game::Shape* shape)
  {
    auto pose = PxTransform(toPxVec3(object->transform.position), toPxQuat(object->transform.rotation));

    PxGeometry* geom;
    PxSphereGeometry sphere;
    PxBoxGeometry box;
    if (auto s = dynamic_cast<const Game::Sphere*>(shape))
    {
      sphere.radius = s->radius;
      geom = &sphere;
    }
    else if (auto b = dynamic_cast<const Game::Box*>(shape))
    {
      box.halfExtents = toPxVec3(b->halfExtents);
      geom = &box;
    }


    PxRigidActor* actor{};
    switch (material)
    {
    case Game::MaterialType::PLAYER:
      break;
    case Game::MaterialType::TERRAIN:
    {
      actor = PxCreateStatic(*gPhysics, pose, *geom, *gMaterials[(int)material]);
      break;
    }
    case Game::MaterialType::OBJECT:
    {
      auto* dynamic = PxCreateDynamic(*gPhysics, pose, *geom, *gMaterials[(int)material], 10.0f);
      actor = dynamic;
      if (object->type == EntityType::EXPLOSIVE)
      {
        dynamic->setContactReportThreshold(25000.0);
      }
      break;
    }
    default:
      break;
    }


    gActorToObject[actor] = object;
    gObjectToActor[object] = actor;

    gScene->addActor(*actor);
  }

  void RemoveObject(Game::GameObject* object)
  {
    if (object)
    {
      auto actor = gObjectToActor[object];
      gObjectToActor.erase(object);
      gActorToObject.erase(actor);
      gScene->removeActor(*actor);
    }
  }

  void SetObjectTransform(Game::GameObject* object, Transform transform)
  {
    gObjectToActor[object]->setGlobalPose({ toPxVec3(transform.position), toPxQuat(transform.rotation) });
  }
};

namespace Game
{
  Physics::Physics()
  {
    impl_ = new PhysicsImpl;
  }

  Physics::~Physics()
  {
    delete impl_;
  }

  //collider_t Physics::CookMesh(const GFX::Mesh& mesh)
  //{
  //  return impl_->CookMesh(mesh);
  //}

  void Physics::SetWorld(World* world)
  {
    impl_->SetWorld(world);
  }

  void Physics::Simulate(float dt)
  {
    impl_->Simulate(dt);
  }

  //void Physics::AddObject(GameObject* object, MaterialType material, collider_t mesh)
  //{
  //  impl_->AddObject(object, material, mesh);
  //}

  void Physics::AddObject(GameObject* object, MaterialType material, const Shape* shape)
  {
    impl_->AddObject(object, material, shape);
  }

  void Physics::RemoveObject(GameObject* object)
  {
    impl_->RemoveObject(object);
  }

  void Physics::SetObjectTransform(GameObject* object, Transform transform)
  {
  }
}

void ContactReportCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
  //printf("a");
}

void ContactReportCallback::onContact(const PxContactPairHeader& pairHeader, [[maybe_unused]] const PxContactPair* pairs, [[maybe_unused]] PxU32 nbPairs)
{
  auto it1 = physics_->gActorToObject.find(pairHeader.actors[0]);
  auto it2 = physics_->gActorToObject.find(pairHeader.actors[1]);
  if (it1 != physics_->gActorToObject.end())
  {
    //printf("a");
  }
  if (it2 != physics_->gActorToObject.end())
  {
    //printf("b");
  }
  //printf("C");

  for (int i = 0; i < nbPairs; i++)
  {
    const auto& pair = pairs[i];

    for (int j = 0; j < pair.contactCount; j++)
    {
      auto a = pairHeader.actors[0];
      auto b = pairHeader.actors[1];

      if (!(pair.events & PxPairFlag::eNOTIFY_THRESHOLD_FORCE_FOUND))
      {
        continue;
      }
      //if ()
      if (auto ait = physics_->gActorToObject.find(a); ait != physics_->gActorToObject.end())
      {

      }
      printf("%f\n", pair.contactImpulses[j]);
    }
  }
}