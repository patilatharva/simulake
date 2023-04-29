#include <iostream>

#include "callbacks.hpp"
#include "window.hpp"

namespace simulake {

Window::Window(const std::uint32_t _width, const std::uint32_t _height,
               const std::string_view _title)
    : width(_width), height(_height), title(_title) {

  int rc = glfwInit();
  assert(rc != 0);

  glfwSetErrorCallback(callbacks::error);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  _window.reset(
      glfwCreateWindow(width, height, title.data(), nullptr, nullptr));
  if (_window.get() == nullptr) {
    failure_exit();
  }

  glfwMakeContextCurrent(_window.get());

  /* register window event callbacks */
  glfwSetFramebufferSizeCallback(_window.get(), callbacks::framebuffer_size);
  glfwSetKeyCallback(_window.get(), callbacks::key);
  glfwSetCursorPosCallback(_window.get(), callbacks::cursor_pos);
  glfwSetScrollCallback(_window.get(), callbacks::scroll);
  glfwSetMouseButtonCallback(_window.get(), callbacks::mouse_button);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    failure_exit();
  }
}

bool Window::should_close() const noexcept {
  return glfwWindowShouldClose(_window.get());
}

void Window::swap_buffers() const noexcept { glfwSwapBuffers(_window.get()); }

[[noreturn]] void Window::failure_exit() const noexcept {
  std::cerr << "ERROR::WINDOW_FAILURE_EXIT" << std::endl;
  glfwTerminate();
  std::exit(-1);
}

GLFWwindow *Window::get_window_ptr() const noexcept { return _window.get(); }

} /* namespace simulake */