#include <iostream>
#include <format>
#include <glm/glm.hpp>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <exception>

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

  const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  GLFWwindow* window = glfwCreateWindow(createInfo.width, createInfo.height, "Sample Text", nullptr, nullptr);

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

  // adjustable green hue
  float green = 0.0f;

  while (!glfwWindowShouldClose(window))
  {
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
    ImGui::SliderFloat("Green", &green, 0.0f, 1.0f);
    ImGui::End();

    // pulsing red+blue
    double time = glfwGetTime();
    glClearColor(sin(time), green, cos(time), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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