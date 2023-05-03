// vim: ft=cpp :

// base macros and definitions
#include "base.cl"
#include "cell.cl"

// {{{ initialize kernel
__kernel void initialize(__global grid_t *grid, __global grid_t *next_grid,
                         uint2 dims) {
  GEN_LOC_VARS();
  const uint size = get_global_size(0); // full grid size (rows)

  const uint width = dims[0];
  const uint height = dims[1];

  const unsigned int idx = GET_INDEX(row, col, width, height);
  // printf("%d-%d-%d\n", row, col, idx);

  grid[idx].type = AIR_TYPE;
  next_grid[idx].type = AIR_TYPE;

  grid[idx].mass = AIR_MASS;
  next_grid[idx].mass = AIR_MASS;

  grid[idx].velocity = V_STATIONARY;
  next_grid[idx].velocity = V_STATIONARY;

  grid[idx].updated = false;
  next_grid[idx].updated = false;

  // // draw box with water
  // if (abs(row - (height / 2) - 5) < 5 &&
  //     abs(col - width / 4 - 5) < width / 2 + 5) {
  //   grid[idx].type = STONE_TYPE;
  //   next_grid[idx].type = STONE_TYPE;

  //   grid[idx].mass = STONE_MASS;
  //   next_grid[idx].mass = STONE_MASS;
  // }

  // else if ((abs(col - (width * 1 / 4) - 5) < 5 ||
  //           abs(col - (width * 3 / 4) - 5) < 5) &&
  //          abs(row - height / 2 + 80) < 100) {
  //   grid[idx].type = STONE_TYPE;
  //   next_grid[idx].type = STONE_TYPE;

  //   grid[idx].mass = STONE_MASS;
  //   next_grid[idx].mass = STONE_MASS;
  // }

  // else if (get_rand(row, col) % 7 == 0) {
  //   grid[idx].type = WATER_TYPE;
  //   next_grid[idx].type = WATER_TYPE;

  //   grid[idx].mass = WATER_MASS;
  //   next_grid[idx].mass = WATER_MASS;
  // }
}
// }}}

// {{{ random init kernel
__kernel void random_init(__global grid_t *grid,
                          const __global grid_t *next_grid, const uint2 dims) {
  GEN_LOC_VARS();

  const uint width = dims[0];
  const uint height = dims[1];

  const uint idx = GET_INDEX(row, col, width, height);
  const uint rand = get_rand(row, col);

  if (rand % 20) {
    grid[idx].type = SAND_TYPE;
    grid[idx].mass = SAND_TYPE;
    grid[idx].velocity = V_STATIONARY;
    grid[idx].updated = false;
  }
}
// }}}

//  simulate kernel
__kernel void simulate(__global grid_t *grid, __global grid_t *next_grid,
                       const uint2 dims) {
  GEN_LOC_VARS();
  const uint2 loc = {row, col};

  const uint width = dims[0];
  const uint height = dims[1];
  const uint num_cells = width * height;

  GEN_BOUNDS_VALID(row, col, width, height);
  GEN_NEIGHBOUR_INDICES(row, col, width, height);

  const uint type = (uint)grid[idx].type; // scale up from std::uint8_t

  switch (type) {
  case SMOKE_TYPE:
    smoke_step(loc, dims, grid, next_grid);
    break;

  case FIRE_TYPE:
    fire_step(loc, dims, grid, next_grid);
    break;

  case WATER_TYPE:
    water_step(loc, dims, grid, next_grid);
    break;

  case OIL_TYPE:
    oil_step(loc, dims, grid, next_grid);
    break;

  case SAND_TYPE:
    sand_step(loc, dims, grid, next_grid);
    break;

  case JELLO_TYPE:
    jello_step(loc, dims, grid, next_grid);
    break;

  case STONE_TYPE:
    stone_step(loc, dims, grid, next_grid);
    break;

  case AIR_TYPE:
  default:
    break;
  }
}

// {{{ render texture kernel
__kernel void render_texture(__write_only image2d_t texture,
                             __global const grid_t *grid,
                             __global const grid_t *next_grid, const uint2 dims,
                             const uint cell_size) {
  GEN_LOC_VARS();

  const uint width = dims[0];
  const uint height = dims[1];

  const uint idx = GET_INDEX(row, col, width, height);
  const uint type = (int)grid[idx].type; // scale up from std::uint8_t

  // write texture
  // attributes go here
  const float4 out_color = {
      type,
      grid[idx].mass,
      grid[idx].velocity.x,
      grid[idx].velocity.y,
  };

  const int2 out_coord = {width - col - 1, height - row - 1};
  write_imagef(texture, out_coord, out_color);
}
// }}}

// {{{ spawn cells kernel
__kernel void spawn_cells(__global grid_t *grid, __global grid_t *next_grid,
                          const uint2 center, const uint paint_radius,
                          const uint target, const uint2 dims,
                          const uint cell_size) {
  GEN_LOC_VARS();

  const uint width = dims.x;
  const uint height = dims.y;
  const uint screen_col = width - col - 1;

  const uint idx = GET_INDEX(row, screen_col, width, height);
  const float d =
      sqrt(pow(col - (float)center.x, 2) + pow(row - (float)center.y, 2));

  if (d <= paint_radius && (VACANT(grid[idx]) || (target == AIR_TYPE))) {
    next_grid[idx].type = target;
    grid[idx].type = target;

    const float mass = get_mass(target);
    next_grid[idx].mass = mass;
    grid[idx].mass = mass;

    next_grid[idx].velocity = V_STATIONARY;
    grid[idx].velocity = V_STATIONARY;

    next_grid[idx].updated = false;
    grid[idx].updated = false;
  }
}
// }}}
