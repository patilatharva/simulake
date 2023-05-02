#include "appstate.hpp"

namespace simulake {

void AppState::set_selected_cell_type(const simulake::CellType type) noexcept {
  AppState &state = AppState::get_instance();
  state.selected_cell_type = type;
}

void AppState::set_spawn_radius(const std::uint32_t num_cells) noexcept {
  AppState &state = AppState::get_instance();
  state.spawn_radius = num_cells;

  if (!state.paused) {
    Renderer::uniform_opts_t uniforms_to_update = {
      {Renderer::UniformId::SPAWN_RADIUS, static_cast<int>(state.spawn_radius)}};

    state.renderer->submit_shader_uniforms(uniforms_to_update);
  }
}

void AppState::set_cell_size(const std::uint32_t cell_size) noexcept {
  AppState &state = AppState::get_instance();
  state.cell_size = cell_size;

  Renderer::uniform_opts_t uniforms_to_update = {
    {Renderer::UniformId::CELL_SIZE, static_cast<int>(cell_size)}};

  state.renderer->submit_shader_uniforms(uniforms_to_update);
}

void AppState::set_mouse_pressed(const bool pressed) noexcept {
  AppState &state = AppState::get_instance();
  if (state.paused and pressed) {
    Renderer::uniform_opts_t uniforms_to_update = {
      {Renderer::UniformId::MOUSE_POS, glm::vec2{state.prev_mouse_x,
                                                 state.prev_mouse_y}}};
    state.renderer->submit_shader_uniforms(uniforms_to_update);
    state.set_paused(false);
  }

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
    {Renderer::UniformId::RESOLUTION, glm::ivec2{static_cast<int>(width),
                                                 static_cast<int>(height)}}};

  state.renderer->submit_shader_uniforms(uniforms_to_update);
  state.renderer->set_viewport_size(width, height);
}

void AppState::set_mouse_pos(const float xpos, const float ypos) noexcept {
  AppState &state = AppState::get_instance();
  state.prev_mouse_x = xpos;
  state.prev_mouse_y = ypos;

  if (!state.paused) {
    Renderer::uniform_opts_t uniforms_to_update = {
      {Renderer::UniformId::MOUSE_POS, glm::vec2{xpos, ypos}}};

    state.renderer->submit_shader_uniforms(uniforms_to_update);
  }
}

void AppState::set_time(const float curr_time) noexcept {
  AppState &state = AppState::get_instance();
  state.delta_time = curr_time - state.prev_time;
  state.prev_time = state.time;
  state.time = curr_time;
}

void AppState::set_paused(const bool paused) noexcept {
  AppState &state = AppState::get_instance();
  if (state.paused != paused) {
    /* make sure time is updated on pause/unpause */
    state.set_time(state.window->get_time());
    state.paused = paused;
  }
}

bool AppState::get_paused() const noexcept {
  return paused;
}

CellType AppState::get_selected_cell_type() const noexcept {
  return selected_cell_type;
}

CellType AppState::get_target_type() const noexcept {
  return erase_mode ? CellType::AIR : selected_cell_type;
}

} /* namespace simulake */
