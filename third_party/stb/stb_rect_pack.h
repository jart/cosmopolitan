#ifndef COSMOPOLITAN_THIRD_PARTY_STB_STB_RECT_PACK_H_
#define COSMOPOLITAN_THIRD_PARTY_STB_STB_RECT_PACK_H_

#define STBRP__MAXVAL         0x7fffffff
#define STB_RECT_PACK_VERSION 1

#define STBRP_HEURISTIC_Skyline_default       0
#define STBRP_HEURISTIC_Skyline_BL_sortHeight 0
#define STBRP_HEURISTIC_Skyline_BF_sortHeight 1

COSMOPOLITAN_C_START_

typedef struct stbrp_context stbrp_context;
typedef struct stbrp_node stbrp_node;
typedef struct stbrp_rect stbrp_rect;
typedef int stbrp_coord;

struct stbrp_node {
  stbrp_coord x, y;
  stbrp_node *next;
};

struct stbrp_context {
  int width;
  int height;
  int align;
  int init_mode;
  int heuristic;
  int num_nodes;
  stbrp_node *active_head;
  stbrp_node *free_head;
  stbrp_node extra[2];  // so optimal user-node-count is 'width' not 'width+2'
};

struct stbrp_rect {
  int id;            // reserved for your use:
  stbrp_coord w, h;  // input
  stbrp_coord x, y;  // output
  int was_packed;    // non-zero if valid packing
};                   // 16 bytes, nominally

void stbrp_init_target(stbrp_context *, int, int, stbrp_node *, int);
void stbrp_setup_allow_out_of_mem(stbrp_context *, int);
void stbrp_setup_heuristic(stbrp_context *, int);
int stbrp_pack_rects(stbrp_context *, stbrp_rect *, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_STB_STB_RECT_PACK_H_ */
