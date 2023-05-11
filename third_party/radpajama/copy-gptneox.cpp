#include "ggml.h"
#include "gptneox.h"

#include <cstdio>
#include <map>
#include <string>

static const std::map<std::string, enum gptneox_ftype> GPTNEOX_FTYPE_MAP = {
  {"q4_0", GPTNEOX_FTYPE_MOSTLY_Q4_0},
  {"q4_1", GPTNEOX_FTYPE_MOSTLY_Q4_1},
  {"q4_2", GPTNEOX_FTYPE_MOSTLY_Q4_2},
  //{"q4_3", GPTNEOX_FTYPE_MOSTLY_Q4_3},
  {"q5_0", GPTNEOX_FTYPE_MOSTLY_Q5_0},
  {"q5_1", GPTNEOX_FTYPE_MOSTLY_Q5_1},
  {"q8_0", GPTNEOX_FTYPE_MOSTLY_Q8_0},
};

// usage:
//  ./quantize models/llama/ggml-model.bin models/llama/ggml-model-quant.bin type
//
int main(int argc, char ** argv) {
    ggml_time_init();

    if (argc < 4) {
        fprintf(stderr, "usage: %s model-f32.bin model-quant.bin ftype\n", argv[0]);
        for (auto it = GPTNEOX_FTYPE_MAP.begin(); it != GPTNEOX_FTYPE_MAP.end(); it++) {
            fprintf(stderr, "  type = \"%s\" or %d\n", it->first.c_str(), it->second);
        }
        return 1;
    }

    // needed to initialize f16 tables
    {
        struct ggml_init_params params = { 0, NULL, false };
        struct ggml_context * ctx = ggml_init(params);
        ggml_free(ctx);
    }

    const std::string fname_inp = argv[1];
    const std::string fname_out = argv[2];

    enum gptneox_ftype ftype;
    if (argv[3][0] == 'q') {
        auto it = GPTNEOX_FTYPE_MAP.find(argv[3]);
        if (it == GPTNEOX_FTYPE_MAP.end()) {
            fprintf(stderr, "%s: unknown ftype '%s'\n", __func__, argv[3]);
            return 1;
        }
        ftype = it->second;
    } else {
        ftype = (enum gptneox_ftype)atoi(argv[3]);
    }

    gptneox_model_copy(fname_inp.c_str(), fname_out.c_str(), ftype);

    return 0;
}
