// vim: ft=cpp :

#ifndef SIMULAKE_COMPUTE_BASE_CL
#define SIMULAKE_COMPUTE_BASE_CL

// TODO(vir): benchmark?
#define USE_ROWMAJOR true

// clang-format off
#define   NONE_TYPE    0
#define   AIR_TYPE     1
#define   SMOKE_TYPE   2
#define   FIRE_TYPE    3
#define   WATER_TYPE   4
#define   OIL_TYPE     5
#define   SAND_TYPE    6
#define   JELLO_TYPE   7
#define   STONE_TYPE   8
// clang-format on

#define FALLS_DOWN(x) (x.type > WATER_TYPE)
#define VACANT(x) (x.type == AIR_TYPE)

#define V_STATIONARY ((float2){0.0f, 0.0f})

#define IS_FLUID(x) (x.type >= AIR_TYPE && x.type <= OIL_TYPE)
#define IS_LIQUID(x) (x.type >= WATER_TYPE && x.type <= OIL_TYPE)
#define IS_AIR(x) (x.type == AIR_TYPE)
#define IS_WATER(x) (x.type == WATER_TYPE)
#define IS_FLAMMABLE(x)                                                        \
  (x.type >= AIR_TYPE && (x.type == OIL_TYPE || x.type == SAND_TYPE))

#define FCLAMP(x, l, h) (fmax((float)l, fmin((float)x, (float)h)))

#if USE_ROWMAJOR
#define GET_INDEX(row, col, width, height) (((col) * (height)) + (row))
#else
#define GET_INDEX(row, col, width, height) (((row) * (width)) + (col))
#endif

// clang-format off
#define   AIR_MASS     0.0f
#define   SMOKE_MASS   0.2f
#define   FIRE_MASS    0.3f
#define   WATER_MASS   1.0f
#define   OIL_MASS     0.8f
#define   SAND_MASS    1.5f
#define   JELLO_MASS   1.7f
#define   STONE_MASS   3.0f
// clang-format on

#define ASSERT_TRUE(x)                                                         \
  {                                                                            \
    if (!(x))                                                                  \
      printf("assert_fail: (%s) was false @ %d\n", #x, __LINE__);              \
  }

#define ASSERT_FALSE(x)                                                        \
  {                                                                            \
    if ((x))                                                                   \
      printf("assert_fail: (%s) was true @ %d\n", #x, __LINE__);               \
  }

// get a random number (0, UINT_MAX) using xor shift
inline uint get_rand(const uint row, const uint col) {
  const uint seed = 1337 + row;
  const uint t = seed ^ (seed << 11);
  return (7331 + col) ^ ((7331 + col) >> 19) ^ (t ^ (t >> 8));
}

inline float get_mass(const uint type) {
  switch (type) {
  case SMOKE_TYPE:
    return SMOKE_MASS;
    break;
  case FIRE_TYPE:
    return FIRE_MASS;
    break;
  case WATER_TYPE:
    return WATER_MASS;
    break;
  case OIL_TYPE:
    return OIL_MASS;
    break;
  case SAND_TYPE:
    return SAND_MASS;
    break;
  case JELLO_TYPE:
    return JELLO_MASS;
    break;

  case AIR_TYPE:
  default:
    return AIR_MASS;
  }
}

// cell attributes
// TODO(vir): benchmark?
typedef struct __attribute__((packed, aligned(8))) {
  char type;
  float mass;
  float2 velocity;

  bool updated;
} grid_t;

#define GEN_NEIGHBOUR_INDICES(row, col, width, height)                         \
  const uint idx_top_left = GET_INDEX(row - 1, col - 1, width, height);        \
  const uint idx_top = GET_INDEX(row - 1, col + 0, width, height);             \
  const uint idx_top_right = GET_INDEX(row - 1, col + 1, width, height);       \
  const uint idx_left = GET_INDEX(row + 0, col - 1, width, height);            \
  const uint idx = GET_INDEX(row + 0, col + 0, width, height);                 \
  const uint idx_right = GET_INDEX(row + 0, col + 1, width, height);           \
  const uint idx_bot_left = GET_INDEX(row + 1, col - 1, width, height);        \
  const uint idx_bot = GET_INDEX(row + 1, col + 0, width, height);             \
  const uint idx_bot_right = GET_INDEX(row + 1, col + 1, width, height);

#define GEN_BOUNDS_VALID(row, col, width, height)                              \
  const bool top_valid = (row - 1) >= 0;                                       \
  const bool bot_valid = (row + 1) < height;                                   \
  const bool left_valid = ((long)col - 1) > 0;                                 \
  const bool right_valid = (col + 1) < width;

#define GEN_LOC_VARS()                                                         \
  const uint col = get_global_id(0);                                           \
  const uint row = get_global_id(1);

#define STEP_IMPL(name)                                                        \
  void name(const uint2 loc, const uint2 dims, __global grid_t *grid,          \
            __global grid_t *next_grid)

#endif