#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "shader.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

struct Cell {
  int type;
  float mass;
};

struct GridData {
  int width;
  int height;
  Cell* cells;
};

class Renderer {
public:
  Renderer();

  ~Renderer() {
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteProgram(_shader.get_id());
  };

  /* disable copy construction/assignment */
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;

  /* submit new grid data to renderer */
  void submit_grid(GridData grid_data);

  /* render frame based on current GridData */
  void render(GLFWwindow*& window);

private:

  void _update_grid_data_texture(const GridData& grid_data);

  std::tuple<std::vector<float>, std::vector<unsigned int>> _generate_grid();

  void _set_cell_size(int cell_size);

  Shader _shader;

  int _cell_size;
  glm::ivec2 _grid_size;     /* grid width, height in cells */
  glm::ivec2 _viewport_size; /* viewport width, height in pixels */

  int _num_cells;
  std::vector<float> _vertices;
  std::vector<unsigned int> _ebo_indices;

  GLuint _VAO, _VBO, _EBO, _GRID_DATA_TEXTURE;
};

#endif /* ifndef RENDERER_HPP */
