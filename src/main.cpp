#include <filesystem>
#include <iostream>
#include <thread>

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <omp.h>

#include "constants.hpp"
#include "renderer.hpp"
#include "shader.hpp"

#include "grid.hpp"
#include "utils.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // TODO: resize grid and push to renderer
  // glViewport(0, 0, width, height);
}

/* initialize GLAD and GLFW, then create window object and set callbacks */
void create_glfw_contexts(GLFWwindow *&window) {
  if (!glfwInit()) {
    std::cerr << "ERROR::GLFW_INITIALIZATION_FAILURE" << std::endl;
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT,
                            WINDOW_TITLE, nullptr, nullptr);
  if (nullptr == window) {
    std::cerr << "ERROR::GLFW_WINDOW_CREATION_FAILURE" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void test_renderer() {
  GLFWwindow *window;
  create_glfw_contexts(window);

  GridData grid_data;
  grid_data.width = 200;
  grid_data.height = 150;

  int test_size = grid_data.width * grid_data.height;
  grid_data.cells = new Cell[test_size];

  Renderer renderer{};
  std::srand(static_cast<unsigned>(std::time(nullptr)));
  for (int i = 0; i < test_size; ++i) {
    grid_data.cells[i].type = std::rand() % 3;
    grid_data.cells[i].mass =
        static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 10.0));
  }

  while (!glfwWindowShouldClose(window)) {
    PROFILE_SCOPE("renderloop");
    renderer.submit_grid(grid_data);
    renderer.render(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}

void test_simulation() {
  PROFILE_FUNCTION();

  // easy-toggles
  constexpr auto DEBUG_PRINT = false;
  constexpr auto NUM_THREADS = 10;
  constexpr auto SIM_STEPS = 1000;

  // TODO(vir): find a better place for this
  omp_set_num_threads(NUM_THREADS);

  // create grid
  const int width = 500, height = 500;
  simulake::Grid grid(width, height);

  // example: demo sand simluation
  {
    // simulake::scope_timer_t timer("full sim");
    PROFILE_SCOPE("sim");

    for (int i = 0; i < SIM_STEPS; i += 1) {
      // spawn new particles in
      if (i % (height / 2) == 0) {
        if (i % height == 0)
          grid.set_state(0, (width * 2 / 3) + 1, simulake::CellType::SAND);
        else
          grid.set_state(0, (width * 1 / 3) - 1, simulake::CellType::SAND);
      }

      if constexpr (DEBUG_PRINT) {
        std::cout << grid << std::endl;
        grid.simulate();
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
      } else {
        grid.simulate();
      }
    }
  }

  if constexpr (DEBUG_PRINT)
    std::cout << grid << std::endl;
}

int main(int argc, char **argv) {
  // test_simulation();
  test_renderer();
  return 0;
}
