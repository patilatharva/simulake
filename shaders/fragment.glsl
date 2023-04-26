#version 330 core

#define SAND_TYPE 1
#define WATER_TYPE 2

uniform sampler2D u_grid_data_texture;

in vec2 TexCoord;

out vec4 frag_color;

void main() {
  vec4 grid_data = texture(u_grid_data_texture, TexCoord);

  int type = int(grid_data.r);
  float mass = grid_data.g;

  if (type == SAND_TYPE) {
    frag_color = vec4(1.0, 1.0, 0.0, 1.0);
  } else if (type == WATER_TYPE) {
    frag_color = vec4(0.0, 0.0, 1.0, 1.0);
  } else {
    frag_color = vec4(0.0, 0.0, 0.0, 1.0);
  }
}
