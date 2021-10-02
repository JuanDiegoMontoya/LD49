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
  GLFWwindow* window = glfwCreateWindow(createInfo.width, createInfo.height, "Top Text", nullptr, nullptr);

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
  ImGui_ImplGlfw_InitForOpenGL(window, false);
  ImGui_ImplOpenGL3_Init();
  ImGui::StyleColorsDark();
  
  int frameWidth, frameHeight;
  glfwGetFramebufferSize(window, &frameWidth, &frameHeight);

  glViewport(0, 0, frameWidth, frameHeight);

  Game::EntityManager entityManager;
  Game::Physics physics;
  GFX::Renderer renderer;
  GFX::Camera camera;
  camera.proj = glm::perspective(glm::radians(90.0f), static_cast<float>(frameWidth) / frameHeight, 0.3f, 100.0f);
  camera.viewInfo.position = { -5.5, 1, 0 };

  GFX::Mesh mesh = GFX::LoadMesh("sphere.obj");

  MeshHandle bunnyHandle = renderer.GenerateMeshHandle(mesh);
  Game::GameObject bunny;
  bunny.entity = entityManager.CreateEntity();
  bunny.transform.position = { 0, 5.5, 0 };
  bunny.transform.scale = glm::vec3(1);
  bunny.mesh = bunnyHandle;

  Game::Sphere sphere{ 1.0f };
  physics.AddObject(&bunny, Game::MaterialType::OBJECT, &sphere);
  
  //entityManager.GetObject(bunny.entity) = bunny;

  double prevFrame = glfwGetTime();
  while (!glfwWindowShouldClose(window))
  {
    double curFrame = glfwGetTime();
    double dt = curFrame - prevFrame;
    prevFrame = curFrame;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glfwPollEvents();

    // press escape to close window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
      glfwSetWindowShouldClose(window, true);
    }

    // some ImGui stuff for testing
    ImGui::Begin("joe");
    ImGui::Text("Aloha");
    ImGui::SliderFloat("Pitch", &camera.viewInfo.pitch, -3.14f / 2, 3.14f / 2);
    ImGui::SliderFloat("Yaw", &camera.viewInfo.yaw, -3.14f, 3.14f);
    ImGui::End();


    physics.Simulate(dt);

    //camera.viewInfo.yaw += dt;
    //bunny.transform.rotation *= glm::rotate(glm::quat(1, 0, 0, 0), static_cast<float>(dt), { 0, 1, 0 });
    renderer.Submit(bunny.transform, bunny.mesh, bunny.renderable);
    renderer.Draw(camera);

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