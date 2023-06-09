#include "appstate.hpp"

namespace simulake {

void AppState::set_renderer(Renderer *renderer) noexcept {
  AppState &state = AppState::get_instance();
  state.renderer = renderer;
}

void AppState::set_window(Window *window) noexcept {
  AppState &state = AppState::get_instance();
  state.window = window;
}

void AppState::set_grid(GridBase *grid) noexcept {
  AppState &state = AppState::get_instance();
  state.grid = grid;

  Renderer::uniform_opts_t uniforms_to_update = {
    {Renderer::UniformId::GRID_DIM,
        glm::vec2{static_cast<float>(grid->get_width()),
                  static_cast<float>(grid->get_height())}}};

  state.renderer->submit_shader_uniforms(uniforms_to_update);
}

void AppState::set_selected_cell_type(const simulake::CellType type) noexcept {
  AppState &state = AppState::get_instance();
  state.selected_cell_type = type;
}

void AppState::set_spawn_radius(const std::uint32_t num_cells) noexcept {
  AppState &state = AppState::get_instance();
  state.spawn_radius = num_cells;

  Renderer::uniform_opts_t uniforms_to_update = {
    {Renderer::UniformId::SPAWN_RADIUS, static_cast<float>(state.spawn_radius)}};

  state.renderer->submit_shader_uniforms(uniforms_to_update);
}

void AppState::set_cell_size(const std::uint32_t cell_size) noexcept {
  AppState &state = AppState::get_instance();
  state.cell_size = cell_size;

  Renderer::uniform_opts_t uniforms_to_update = {
      {Renderer::UniformId::CELL_SIZE, static_cast<float>(cell_size)}};

  state.renderer->submit_shader_uniforms(uniforms_to_update);
}

void AppState::set_mouse_pressed(const bool pressed) noexcept {
  AppState &state = AppState::get_instance();
  state.mouse_pressed = pressed;
}

void AppState::set_erase_mode(const bool erase_mode) noexcept {
  AppState &state = AppState::get_instance();
  state.erase_mode = erase_mode;
}

void AppState::set_window_size(const std::uint32_t width,
                               const std::uint32_t height) noexcept {
  AppState &state = AppState::get_instance();
  state.window_width = width;
  state.window_height = height;

  Renderer::uniform_opts_t uniforms_to_update = {
      {Renderer::UniformId::RESOLUTION,
       glm::vec2{static_cast<float>(width), static_cast<float>(height)}}};

  state.renderer->submit_shader_uniforms(uniforms_to_update);
  state.renderer->set_viewport_size(width, height);
}

void AppState::set_mouse_pos(const float xpos, const float ypos) noexcept {
  AppState &state = AppState::get_instance();
  state.prev_mouse_x = xpos;
  state.prev_mouse_y = ypos;

  Renderer::uniform_opts_t uniforms_to_update = {
    {Renderer::UniformId::MOUSE_POS, glm::vec2{xpos, ypos}}};

  state.renderer->submit_shader_uniforms(uniforms_to_update);
}

void AppState::set_time(const float curr_time) noexcept {
  AppState &state = AppState::get_instance();
  state.delta_time = curr_time - state.prev_time;
  state.prev_time = state.time;
  state.time = curr_time;

  Renderer::uniform_opts_t uniforms_to_update = {
    {Renderer::UniformId::TIME, curr_time}};

  state.renderer->submit_shader_uniforms(uniforms_to_update);
}

void AppState::set_paused(const bool paused) noexcept {
  AppState &state = AppState::get_instance();
  state.paused = paused;
}

CellType AppState::get_target_type() noexcept {
  AppState &state = AppState::get_instance();
  return state.erase_mode ? CellType::AIR : state.selected_cell_type;
}

Renderer *AppState::get_renderer() noexcept {
  AppState &state = AppState::get_instance();
  return state.renderer;
}

Window *AppState::get_window() noexcept {
  AppState &state = AppState::get_instance();
  return state.window;
}

GridBase *AppState::get_grid() noexcept {
  AppState &state = AppState::get_instance();
  return state.grid;
}

simulake::CellType AppState::get_selected_cell_type() noexcept {
  AppState &state = AppState::get_instance();
  return state.selected_cell_type;
}

std::uint32_t AppState::get_spawn_radius() noexcept {
  AppState &state = AppState::get_instance();
  return state.spawn_radius;
}

std::uint32_t AppState::get_cell_size() noexcept {
  AppState &state = AppState::get_instance();
  return state.cell_size;
}

bool AppState::is_mouse_pressed() noexcept {
  AppState &state = AppState::get_instance();
  return state.mouse_pressed;
}

bool AppState::is_erase_mode() noexcept {
  AppState &state = AppState::get_instance();
  return state.erase_mode;
}

bool AppState::is_paused() noexcept {
  AppState &state = AppState::get_instance();
  return state.paused;
}

std::uint32_t AppState::get_window_width() noexcept {
  AppState &state = AppState::get_instance();
  return state.window_width;
}

std::uint32_t AppState::get_window_height() noexcept {
  AppState &state = AppState::get_instance();
  return state.window_height;
}

float AppState::get_prev_mouse_x() noexcept {
  AppState &state = AppState::get_instance();
  return state.prev_mouse_x;
}

float AppState::get_prev_mouse_y() noexcept {
  AppState &state = AppState::get_instance();
  return state.prev_mouse_y;
}

float AppState::get_time() noexcept {
  AppState &state = AppState::get_instance();
  return state.time;
}

float AppState::get_prev_time() noexcept {
  AppState &state = AppState::get_instance();
  return state.prev_time;
}

float AppState::get_delta_time() noexcept {
  AppState &state = AppState::get_instance();
  return state.delta_time;
}

} /* namespace simulake */
