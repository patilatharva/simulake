#include <iostream>

#include <glad/glad.h>

#include "constants.hpp"
#include "renderer.hpp"
#include "shader.hpp"

Renderer::Renderer(GridData grid_data) {
  /* initialize opengl loader */
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    std::cerr << "ERROR::GLAD_INITIALIZATION_FAILURE" << std::endl;

  /* compile shaders */
  _shader = Shader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
  if (0 == _shader.get_id())
    std::cerr << "ERROR::SHADER_CREATION_FAILURE" << std::endl;

  /* set state variables */
  _set_cell_size(4);
  _set_viewport_size(_cell_size * grid_data.width,
                     _cell_size * grid_data.height);

  /* create gl objects */
  glGenVertexArrays(1, &_VAO);
  glGenBuffers(1, &_VBO);
  glGenBuffers(1, &_EBO);

  /* bind vertex data and load into buffer */
  glBindVertexArray(_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

  /* configure vertex attribute pointers */
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
}

void Renderer::render(GridData grid_data, GLFWwindow *&window) {
  glClear(GL_COLOR_BUFFER_BIT);

  _shader.use();
  _set_grid_size(grid_data);
  _set_viewport_size(_cell_size * _grid_size);

  const auto &grid = _generate_grid(grid_data);
  const auto &vertices = std::get<0>(grid);
  const auto &indices = std::get<1>(grid);

  glBindVertexArray(_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);


  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(),
               &(vertices.front()), GL_STREAM_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
               &(indices.front()), GL_STREAM_DRAW);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

  glfwSwapBuffers(window);
}


std::tuple<std::vector<float>, std::vector<unsigned int>>
  Renderer::_generate_grid(GridData grid_data) {

  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  for (int i = 0; i < _grid_size.x; ++i) {
    for (int j = 0; j < _grid_size.y; ++j) {
      /* draw a quad in following order using triangle strip
       *
       * 2----3
       * |    |
       * |    |
       * 1----4
       */

      glm::vec2 bot_left{2.0f * (i * _cell_size) / _viewport_size.x - 1.0f,
                         2.0f * (j * _cell_size) / _viewport_size.y - 1.0f};

      glm::vec2 top_right{
          2.0f * (i + 1) * _cell_size / _viewport_size.x - 1.0f,
          2.0f * (j + 1) * _cell_size / _viewport_size.y - 1.0f};

      int index = j * grid_data.width + i;
      int cell_type = static_cast<float>(grid_data.cells[index].type);
      float mass = grid_data.cells[index].mass;

      unsigned int base_index =
          static_cast<unsigned int>(vertices.size() / 4);

      vertices.emplace_back(bot_left.x);
      vertices.emplace_back(bot_left.y);
      vertices.emplace_back(cell_type);
      vertices.emplace_back(mass);

      vertices.emplace_back(bot_left.x);
      vertices.emplace_back(top_right.y);
      vertices.emplace_back(cell_type);
      vertices.emplace_back(mass);

      vertices.emplace_back(top_right.x);
      vertices.emplace_back(bot_left.y);
      vertices.emplace_back(cell_type);
      vertices.emplace_back(mass);

      vertices.emplace_back(top_right.x);
      vertices.emplace_back(top_right.y);
      vertices.emplace_back(cell_type);
      vertices.emplace_back(mass);

      indices.push_back(base_index + 0);
      indices.push_back(base_index + 1);
      indices.push_back(base_index + 2);
      indices.push_back(base_index + 1);
      indices.push_back(base_index + 3);
      indices.push_back(base_index + 2);
    }
  }

  return std::make_tuple(vertices, indices);
}

void Renderer::_set_cell_size(int cell_size) {
  _cell_size = cell_size;
  _shader.set_int("u_cell_size", cell_size);
}

void Renderer::_set_grid_size(GridData grid_data) {
  _num_cells = grid_data.width * grid_data.height;
  _grid_size[0] = grid_data.width;
  _grid_size[1] = grid_data.height;
}

void Renderer::_set_viewport_size(int width, int height) {
  _viewport_size[0] = width;
  _viewport_size[1] = height;
  _shader.set_float2("u_resolution", _viewport_size);
  glViewport(0, 0, width, height);
}

void Renderer::_set_viewport_size(glm::ivec2 size) {
  _set_viewport_size(size[0], size[1]);
}
