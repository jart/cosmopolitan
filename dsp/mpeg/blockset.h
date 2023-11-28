#ifndef COSMOPOLITAN_DSP_MPEG_BLOCKSET_H_
#define COSMOPOLITAN_DSP_MPEG_BLOCKSET_H_

#define PLM_BLOCK_SET(DEST, DEST_INDEX, DEST_WIDTH, SOURCE_INDEX, \
                      SOURCE_WIDTH, BLOCK_SIZE, OP)               \
  do {                                                            \
    int dest_scan = DEST_WIDTH - BLOCK_SIZE;                      \
    int source_scan = SOURCE_WIDTH - BLOCK_SIZE;                  \
    for (int y = 0; y < BLOCK_SIZE; y++) {                        \
      for (int x = 0; x < BLOCK_SIZE; x++) {                      \
        DEST[DEST_INDEX] = OP;                                    \
        SOURCE_INDEX++;                                           \
        DEST_INDEX++;                                             \
      }                                                           \
      SOURCE_INDEX += source_scan;                                \
      DEST_INDEX += dest_scan;                                    \
    }                                                             \
  } while (false)

#endif /* COSMOPOLITAN_DSP_MPEG_BLOCKSET_H_ */
