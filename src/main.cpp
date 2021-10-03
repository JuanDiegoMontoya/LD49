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
  //Game::EntityManager entityManager;
  //GFX::Camera camera;
  world.io = &ImGui::GetIO();
  Game::Physics physics;
  GFX::Renderer renderer;
  world.camera.proj = glm::perspective(glm::radians(90.0f), static_cast<float>(frameWidth) / frameHeight, 0.3f, 100.0f);
  world.camera.viewInfo.position = { -5.5, 3, 0 };
  physics.SetWorld(&world);

  GFX::Mesh mesh = GFX::LoadMesh("sphere.obj");

  MeshHandle bunnyHandle = renderer.GenerateMeshHandle(mesh);
  Game::GameObject* bunny = world.entityManager.GetObject(world.entityManager.CreateEntity());
  bunny->transform.position = { 0, 5.5, 0 };
  bunny->transform.scale = glm::vec3(1);
  bunny->mesh = bunnyHandle;
  bunny->renderable.visible = true;

  Game::Sphere sphere{ 1.0f };
  physics.AddObject(bunny, Game::MaterialType::OBJECT, &sphere);

  double prevFrame = glfwGetTime();
  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // press escape to close window
    if (world.io->KeysDownDuration[GLFW_KEY_ESCAPE] == 0.0f)
    {
      world.paused = !world.paused;
    }

    if (world.paused)
    {
      ImGui::SetNextWindowPos(ImVec2(world.io->DisplaySize.x * 0.5f, world.io->DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
      ImGui::Begin("Main Menu");
      if (ImGui::Button("Resume"))
      {
        world.paused = false;
      }

      if (ImGui::Button("Quit"))
      {
        glfwSetWindowShouldClose(window, true);
      }
      ImGui::End();
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else
    {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    double curFrame = glfwGetTime();
    double dt = curFrame - prevFrame;
    prevFrame = curFrame;

    if (world.paused)
    {
      dt = 0;
    }

    if (!world.paused)
    {
      physics.Simulate(dt);
    }

    for (const auto& obj : world.entityManager.GetObjects())
    {
      renderer.Submit(obj.transform, obj.mesh, obj.renderable);
    }
    renderer.Draw(world.camera);

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