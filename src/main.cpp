#include <iostream>
#include <format>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <tiny_obj_loader.h>

#include "gfx/renderer.h"
#include "gfx/mesh.h"
#include "gfx/camera.h"
#include "game/game.h"
#include "game/physics.h"
#include "world.h"

struct WindowCreateInfo
{
  bool maximize{};
  bool decorate{};
  uint32_t width{};
  uint32_t height{};
};

GLFWwindow* CreateWindow(const WindowCreateInfo& createInfo)
{
  if (!glfwInit())
  {
    ;
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwSetErrorCallback([](int, const char* desc)
    {
      std::cout << std::format("GLFW error: {}\n", desc);
    });

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_MAXIMIZED, createInfo.maximize);
  glfwWindowHint(GLFW_DECORATED, createInfo.decorate);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

  const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  GLFWwindow* window = glfwCreateWindow(createInfo.width, createInfo.height, "Top Texplosion", nullptr, nullptr);

  if (!window)
  {
    throw std::runtime_error("Failed to create window");
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  return window;
}

void InitOpenGL()
{
  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0)
  {
    throw std::runtime_error("Failed to initialize OpenGL");
  }
}

int main()
{
  GLFWwindow* window = CreateWindow({ .maximize = false, .decorate = true, .width = 800, .height = 600 });

  InitOpenGL();

  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();
  ImGui::StyleColorsDark();
  
  int frameWidth, frameHeight;
  glfwGetFramebufferSize(window, &frameWidth, &frameHeight);

  glViewport(0, 0, frameWidth, frameHeight);

  World world;
  GFX::Renderer renderer;
  GFX::Mesh sphereMesh = GFX::LoadMesh("sphere.obj");
  GFX::Mesh cubeMesh = GFX::LoadMesh("cube.obj");
  world.sphereMeshHandle = renderer.GenerateMeshHandle(sphereMesh);
  world.cubeMeshHandle = renderer.GenerateMeshHandle(cubeMesh);
  //Game::EntityManager entityManager;
  //GFX::Camera camera;
  world.io = &ImGui::GetIO();
  Game::Physics physics;
  world.camera.proj = glm::perspective(glm::radians(90.0f), static_cast<float>(frameWidth) / frameHeight, 0.10f, 1000.0f);
  world.camera.viewInfo.position = { -5.5, 3, 0 };
  physics.SetWorld(&world);

  world.LoadLevel(*Game::levels[0], &physics);

  // HACK: simulate once to remove visual artifacts while in start menu
  physics.Simulate(0);

  double prevFrame = glfwGetTime();
  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // escape toggles paused state if game is paused or unpaused
    if (world.io->KeysDownDuration[GLFW_KEY_ESCAPE] == 0.0f)
    {
      if (world.gameState == GameState::PAUSED)
      {
        world.gameState = GameState::UNPAUSED;
      }
      else if (world.gameState == GameState::UNPAUSED)
      {
        world.gameState = GameState::PAUSED;
      }
    }

    // enable cheats
    if (world.io->KeysDownDuration[GLFW_KEY_F11] == 0.0f)
    {
      world.cheats = !world.cheats;
    }

    // disable mouse if game is unpaused
    if (world.gameState == GameState::UNPAUSED)
    {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
    {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoDecoration |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoBackground |
      ImGuiWindowFlags_AlwaysAutoResize;
    ImGui::Begin("Game window", nullptr, flags);
    if (world.cheats)
    {
      ImGui::Text("Cheats Active!");
      ImGui::Separator();
    }
    ImGui::Text("Bomb inventory: %d", world.bombInventory);
    ImGui::NewLine();
    ImGui::Text("E (press)  : pick up bomb");
    ImGui::Text("F (hold)   : show bomb placement");
    ImGui::Text("F (release): place bomb");
    ImGui::End();

    double curFrame = glfwGetTime();
    double dt = curFrame - prevFrame;
    prevFrame = curFrame;

    if (world.gameState != GameState::UNPAUSED)
    {
      dt = 0;
    }

    switch (world.gameState)
    {
    case GameState::PAUSED:
    {
      ImGui::SetNextWindowPos(ImVec2(world.io->DisplaySize.x * 0.5f, world.io->DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
      ImGui::SetNextWindowSize(ImVec2(400, 300));
      ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration);
      
      if (world.cheats)
      {
        ImGui::Text("Load Level");
        for (const auto& level : Game::levels)
        {
          if (ImGui::Button(level->name, { -1, 0 }))
          {
            world.LoadLevel(*level, &physics);
          }
        }
        ImGui::NewLine();
      }

      ImGui::Text("Level: %s", world.currentLevel->name);

      if (ImGui::Button("Resume", { -1, 0 }))
      {
        world.gameState = GameState::UNPAUSED;
      }

      if (ImGui::Button("Restart Level", { -1, 0 }))
      {
        world.LoadLevel(*world.currentLevel, &physics);
      }

      if (ImGui::Button("Quit", { -1, 0 }))
      {
        glfwSetWindowShouldClose(window, true);
      }

      if (ImGui::TreeNode("How To Play"))
      {
        ImGui::Text(
          "Get to the BLUE platform and avoid lava to win!\n"
          "Running and jumping alone is not enough.\n"
          "You will need to propel yourself with \n"
          "UNSTABLE EXPLOSIVES\n"
          "\n"
          "Press and release F to place a bomb.\n"
          "Bombs are UNSTABLE and explode if they \n"
          "hit anything too hard.\n"
          "\n"
          "TIP: place a bomb in the air and it will \n"
          "explode when it hits the ground.\n"
          "TIP: jump when a bomb explodes to increase \n"
          "propulsion distance.\n"
          "TIP: carefully place multiple bombs on the \n"
          "ground to generate a BIG explosion when they \n"
          "explode.\n"
          "TIP: place bombs under yourself while flying \n"
          "to increase distance.\n"
        );

        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Controls"))
      {
        ImGui::Text(
          "WASD       : walk\n"
          "Spacebar   : jump\n"
          "E          : pickup bomb\n"
          "F (hold)   : bomb placement guide\n"
          "F (release): place bomb\n"
          "Escape     : pause/unpause game"
        );
        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Current Level Hint"))
      {
        ImGui::Text("%s", world.currentLevel->hint);
        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Lore"))
      {
        ImGui::Text(
          "You have magical skin that is explosion-proof, \n"
          "but not lava-proof. You used to be able to hear, \n"
          "but years of blowing yourself up has rendered you \n"
          "deaf. Fortunately, you can still enjoy your favorite \n"
          "hobby: reaching the blue platforms scattered \n"
          "throughout this firey world."
        );

        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Cheats (open at your own risk)"))
      {
        ImGui::Text(
          "F11: toggle cheats\n"
          "\n"
          "When cheats are active:\n"
          "You have lava invulnerability\n"
          "T (hold): velocitate rapidly\n"
        );
        ImGui::TreePop();
      }

      ImGui::End();
      break;
    }
    case GameState::UNPAUSED:
    {
      physics.Simulate(dt);
      break;
    }
    case GameState::DEAD:
    {
      ImGui::SetNextWindowPos(ImVec2(world.io->DisplaySize.x * 0.5f, world.io->DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
      ImGui::Begin("Dead", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
      srand(world.deathCounter + 500);
      const char* deathMessages[] =
      {
        "You burnt to a crisp",
        "You forgot to turn the oven off",
        "You couldn't resist lava's deadly allure",
        "You came face-to-face with death and lost",
        "You are no longer alive",
        "You died :(",
        "You didn't put your oven mitts on"
      };
      ImGui::Text("%s", deathMessages[rand() % IM_ARRAYSIZE(deathMessages)]);
      if (ImGui::Button("Try Again", { -1, 0 }))
      {
        world.LoadLevel(*world.currentLevel, &physics);
      }

      if (ImGui::Button("Ragequit", { -1, 0 }))
      {
        glfwSetWindowShouldClose(window, true);
      }

      ImGui::End();
      break;
    }
    case GameState::WIN_LEVEL:
    {
      if (world.currentLevel->nextLevel != nullptr)
      {
        ImGui::SetNextWindowPos(ImVec2(world.io->DisplaySize.x * 0.5f, world.io->DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::Begin("Level Victory", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("You beat the level!");

        if (ImGui::Button("Next Level", { -1, 0 }))
        {
          world.LoadLevel(*world.currentLevel->nextLevel, &physics);
        }

        if (ImGui::Button("Replay Level", { -1, 0 }))
        {
          world.LoadLevel(*world.currentLevel, &physics);
        }

        ImGui::End();
        break;
      }

      // fallthrough to WIN_GAME if no next level
      [[fallthrough]];
    }
    case GameState::WIN_GAME:
    {
      ImGui::SetNextWindowPos(ImVec2(world.io->DisplaySize.x * 0.5f, world.io->DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
      ImGui::Begin("Game Victory", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::Text("You beat the game, congrats!");
      ImGui::Text("You died %d time%s", world.deathCounter, world.deathCounter == 1 ? "" : "s");
      ImGui::Text("Cheats activated ;)");

      world.cheats = true;

      ImGui::Text("Load Level");
      for (const auto& level : Game::levels)
      {
        if (ImGui::Button(level->name, { -1, 0 }))
        {
          world.LoadLevel(*level, &physics);
        }
      }
      ImGui::NewLine();

      if (ImGui::Button("Quit", { -1, 0 }))
      {
        glfwSetWindowShouldClose(window, true);
      }

      ImGui::End();
      break;
    }
    default:
      assert(0 && "Invalid gamestate!?!?");
      break;
    }


    // draw everything
    for (const auto& obj : world.entityManager.GetObjects())
    {
      renderer.Submit(obj->transform, obj->mesh, obj->renderable);
    }
    renderer.Draw(world.camera, dt);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  return 0;
}