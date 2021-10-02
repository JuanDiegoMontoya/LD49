#include "physics.h"

#include <array>
#include <unordered_map>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>

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
    PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
  {
    PX_UNUSED(attributes0);
    PX_UNUSED(attributes1);
    PX_UNUSED(filterData0);
    PX_UNUSED(filterData1);
    PX_UNUSED(constantBlockSize);
    PX_UNUSED(constantBlock);

    // all initial and persisting reports for everything, with per-point data
    pairFlags = PxPairFlag::eSOLVE_CONTACT
      | PxPairFlag::eDETECT_DISCRETE_CONTACT
      | PxPairFlag::eNOTIFY_TOUCH_FOUND
      | PxPairFlag::eNOTIFY_TOUCH_PERSISTS
      | PxPairFlag::eNOTIFY_CONTACT_POINTS
      | PxPairFlag::eDETECT_CCD_CONTACT;
    return PxFilterFlag::eDEFAULT;
  }

  class ErrorCallback : public PxDefaultErrorCallback
  {
  public:
    virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
    {
      PxDefaultErrorCallback::reportError(code, message, file, line);
    }
  };

  class ContactReportCallback : public PxSimulationEventCallback
  {
  public:
    //ContactReportCallback(std::unordered_map<physx::PxRigidActor*, Entity>& ea) : gEntityActors(ea) {}
    ContactReportCallback(PhysicsImpl* physics) : physics_(physics) {}

  private:
    //std::unordered_map<physx::PxRigidActor*, Entity>& gEntityActors;
    PhysicsImpl* physics_{};

    void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
    void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
    void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
    void onTrigger(PxTriggerPair* pairs, PxU32 count) { PX_UNUSED(pairs); PX_UNUSED(count); }
    void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}
    void onContact(const PxContactPairHeader& pairHeader, [[maybe_unused]] const PxContactPair* pairs, [[maybe_unused]] PxU32 nbPairs)
    {
      //auto it1 = gEntityActors.find(pairHeader.actors[0]);
      //auto it2 = gEntityActors.find(pairHeader.actors[1]);
      //if (it1 != gEntityActors.end())
      //{
      //  //printf("%s", it1->second.GetComponent<Components::Tag>().tag.c_str());
      //}
      //if (it2 != gEntityActors.end())
      //{
      //  //printf("%s", it2->second.GetComponent<Components::Tag>().tag.c_str());
      //}
    }
  };
}



struct PhysicsImpl
{
  ////////////////////////////////////////////////////////
  // objects
  ////////////////////////////////////////////////////////
  const double tick = 1.0 / 50.0;

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

  std::unordered_map<physx::PxController*, Game::GameObject*> gEntityControllers;


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
    sceneDesc.gravity = PxVec3(0, -15.81f, 0);
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
    gMaterials[(int)Game::MaterialType::OBJECT] = gPhysics->createMaterial(0.2f, 0.4f, 0.9f);

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

  void Simulate(float dt)
  {
    static bool resultsReady = true;
    static double accumulator = 0;
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
      actor = PxCreateDynamic(*gPhysics, pose, *geom, *gMaterials[(int)material], 10.0f);
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