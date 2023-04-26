#include <iostream>

#include <glad/glad.h>
#include <vector>

#include "constants.hpp"
#include "renderer.hpp"
#include "shader.hpp"

Renderer::Renderer() {
  /* initialize opengl loader */
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    std::cerr << "ERROR::GLAD_INITIALIZATION_FAILURE" << std::endl;

  /* compile shaders */
  _shader = Shader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
  if (0 == _shader.get_id())
    std::cerr << "ERROR::SHADER_CREATION_FAILURE" << std::endl;

  /* set state variables */
  _set_cell_size(RENDERER_CELL_SIZE);
  _num_cells = 0;

  /* create gl objects */
  glGenVertexArrays(1, &_VAO);
  glGenBuffers(1, &_VBO);
  glGenBuffers(1, &_EBO);
  glGenTextures(1, &_GRID_DATA_TEXTURE);

  /* bind vertex data and load into buffer */
  glBindVertexArray(_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

  /* configure vertex attribute pointers */
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void Renderer::submit_grid(GridData grid_data) {
  int curr_num_cells = grid_data.width * grid_data.height;
  if (curr_num_cells != _num_cells) {
    /* update dimensions */
    _num_cells = curr_num_cells;
    _grid_size[0] = grid_data.width;
    _grid_size[1] = grid_data.height;
    _viewport_size[0] = _cell_size * grid_data.width;
    _viewport_size[1] = _cell_size * grid_data.height;
    glViewport(0, 0, _viewport_size[0], _viewport_size[1]);

    /* regenerate grid */
    const auto& grid = _generate_grid();
    _vertices = std::get<0>(grid);
    _ebo_indices = std::get<1>(grid);
  }

  _update_grid_data_texture(grid_data);
}

void Renderer::render(GLFWwindow *&window) {
  glClear(GL_COLOR_BUFFER_BIT);

  _shader.use();

  /* activate cells texture */
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _GRID_DATA_TEXTURE);
  _shader.set_int("u_grid_data_texture", 0);

  /* bind vertex buffer and draw triangles */
  glBindVertexArray(_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

  glBufferData(GL_ARRAY_BUFFER,
               sizeof(float) * _vertices.size(),
               &(_vertices.front()), GL_STREAM_DRAW);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(unsigned int) * _ebo_indices.size(),
               &(_ebo_indices.front()), GL_STREAM_DRAW);

  glDrawElements(GL_TRIANGLES, _ebo_indices.size(), GL_UNSIGNED_INT, 0);

  /* flush buffer */
  glfwSwapBuffers(window);
}

void Renderer::_update_grid_data_texture(const GridData& grid_data) {
  glBindTexture(GL_TEXTURE_2D, _GRID_DATA_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  std::vector<float> texture_data(_num_cells * 2);
  for (size_t i = 0; i < _num_cells; ++i) {
    texture_data[i * 2] = static_cast<float>(grid_data.cells[i].type);
    texture_data[i * 2 + 1] = grid_data.cells[i].mass;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, grid_data.width, grid_data.height, 0, GL_RG, GL_FLOAT, texture_data.data());
}

std::tuple<std::vector<float>, std::vector<unsigned int>>
  Renderer::_generate_grid() {

  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  for (int i = 0; i < _grid_size.x; ++i) {
    for (int j = 0; j < _grid_size.y; ++j) {

      glm::vec2 bot_left{2.0f * (i * _cell_size) / _viewport_size.x - 1.0f,
                         2.0f * (j * _cell_size) / _viewport_size.y - 1.0f};

      glm::vec2 top_right{
          2.0f * (i + 1) * _cell_size / _viewport_size.x - 1.0f,
          2.0f * (j + 1) * _cell_size / _viewport_size.y - 1.0f};

      glm::vec2 tex_bot_left{static_cast<float>(i) / _grid_size.x,
                             static_cast<float>(j) / _grid_size.y};

      glm::vec2 tex_top_right{static_cast<float>(i + 1) / _grid_size.x,
                              static_cast<float>(j + 1) / _grid_size.y};

      unsigned int base_index =
          static_cast<unsigned int>(vertices.size() / 4);

      vertices.push_back(bot_left.x);
      vertices.push_back(bot_left.y);
      vertices.push_back(tex_bot_left.x);
      vertices.push_back(tex_bot_left.y);

      vertices.push_back(bot_left.x);
      vertices.push_back(top_right.y);
      vertices.push_back(tex_bot_left.x);
      vertices.push_back(tex_top_right.y);

      vertices.push_back(top_right.x);
      vertices.push_back(bot_left.y);
      vertices.push_back(tex_top_right.x);
      vertices.push_back(tex_bot_left.y);

      vertices.push_back(top_right.x);
      vertices.push_back(top_right.y);
      vertices.push_back(tex_top_right.x);
      vertices.push_back(tex_top_right.y);

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
