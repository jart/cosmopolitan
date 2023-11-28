/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-│
│vi: set net ft=c++ ts=4 sts=4 sw=4 fenc=utf-8                              :vi│
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  llama.com                                                                   │
│  Copyright (c) 2023 Georgi Gerganov                                          │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "third_party/ggml/common.h"
#include "third_party/ggml/ggml.h"
#include "third_party/ggml/llama.h"
#include "third_party/ggml/llama_util.h"
#include "third_party/libcxx/map"
#include "third_party/libcxx/vector"

asm(".ident\t\"\\n\\n\
llama.cpp (MIT License)\\n\
Copyright (c) 2023 Georgi Gerganov\"");
asm(".include \"libc/disclaimer.inc\"");

static const std::map<std::string, llama_ftype> LLAMA_FTYPE_MAP = {
    {"f32",  LLAMA_FTYPE_ALL_F32 },
    {"f16",  LLAMA_FTYPE_MOSTLY_F16 },
    {"q4_0", LLAMA_FTYPE_MOSTLY_Q4_0},
    {"q4_1", LLAMA_FTYPE_MOSTLY_Q4_1},
    {"q4_2", LLAMA_FTYPE_MOSTLY_Q4_2},
    {"q5_0", LLAMA_FTYPE_MOSTLY_Q5_0},
    {"q5_1", LLAMA_FTYPE_MOSTLY_Q5_1},
    {"q8_0", LLAMA_FTYPE_MOSTLY_Q8_0},
};

// usage:
//  ./quantize models/llama/ggml-model.bin models/llama/ggml-model-quant.bin type [nthreads]
//
int main(int argc, char ** argv) {
    verynice();
    ShowCrashReports();

    ggjt_v3();
    ggml_time_init();

    if (argc < 3) {
        fprintf(stderr, "usage: %s model-f32.bin model-quant.bin type [nthreads]\n", argv[0]);
        for (auto it = LLAMA_FTYPE_MAP.begin(); it != LLAMA_FTYPE_MAP.end(); it++) {
            fprintf(stderr, "  type = \"%s\" or %d\n", it->first.c_str(), it->second);
        }
        return 1;
    }

    // needed to initialize f16 tables
    llama_init_backend();

    const std::string fname_inp = argv[1];
    const std::string fname_out = argv[2];

    if (fname_inp == fname_out) {
        fprintf(stderr, "%s: input and output names are same\n", fname_inp.c_str());
        exit(1);
    }

    enum llama_ftype ftype;
    if (!is_integer_str(argv[3])) {
        auto it = LLAMA_FTYPE_MAP.find(argv[3]);
        if (it == LLAMA_FTYPE_MAP.end()) {
            fprintf(stderr, "%s: unknown ftype '%s'\n", __func__, argv[3]);
            return 1;
        }
        ftype = it->second;
    } else {
        ftype = (enum llama_ftype)atoi(argv[3]);
    }

    int nthread = argc > 4 ? atoi(argv[4]) : std::min(20, std::max(1, __get_cpu_count() >> 1));

    const int64_t t_main_start_us = ggml_time_us();

    int64_t t_quantize_us = 0;

    // load the model
    {
        const int64_t t_start_us = ggml_time_us();

        if (llama_model_quantize(fname_inp.c_str(), fname_out.c_str(), ftype, nthread)) {
            fprintf(stderr, "%s: failed to quantize model from '%s'\n", __func__, fname_inp.c_str());
            return 1;
        }

        t_quantize_us = ggml_time_us() - t_start_us;
    }

    // report timing
    {
        const int64_t t_main_end_us = ggml_time_us();

        printf("\n");
        printf("%s: quantize time = %8.2f ms\n", __func__, t_quantize_us/1000.0);
        printf("%s:    total time = %8.2f ms\n", __func__, (t_main_end_us - t_main_start_us)/1000.0);
    }

    return 0;
}
