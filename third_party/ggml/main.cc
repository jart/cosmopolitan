/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-│
│vi: set net ft=c++ ts=4 sts=4 sw=4 fenc=utf-8                              :vi│
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  llama.com                                                                   │
│  Copyright (c) 2023 Justine Alexandra Roberts Tunney                         │
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/ioprio.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prio.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "third_party/ggml/common.h"
#include "third_party/ggml/llama.h"
#include "third_party/ggml/llama_util.h"
#include "third_party/libcxx/atomic"
#include "third_party/libcxx/iostream"
#include "third_party/libcxx/string"
#include "libc/serialize.h"
#include "third_party/libcxx/vector"

#define EPHEMERAL(fmt) "\r\e[K\033[1;35m" fmt " \033[0m"

asm(".ident\t\"\\n\\n\
llama.cpp (MIT License)\\n\
Copyright (c) 2023 Georgi Gerganov\"");
asm(".include \"libc/disclaimer.inc\"");

static gpt_params params;
static llama_context * ctx;
static console_state con_st;

static int n_past;
static int n_remain;
static int n_consumed;
static bool input_noecho;

////////////////////////////////////////////////////////////////////////////////

static std::atomic<bool> is_stalled;
static std::atomic<bool> is_terminated;
static std::atomic<bool> is_interacting;

static void acknowledge_shutdown(void) {
    write(2, "^C", 2);
}

static void sigint_handler_batch(int signo) {
    is_terminated = true;
    acknowledge_shutdown();
}

static void sigint_handler_interactive(int signo) {
    if (!is_interacting && !is_stalled) {
        is_interacting = true;
    } else {
        is_terminated = true;
        acknowledge_shutdown();
    }
}

static int CompareTime(struct timespec a, struct timespec b) {
  int cmp;
  if (!(cmp = (a.tv_sec > b.tv_sec) - (a.tv_sec < b.tv_sec))) {
    cmp = (a.tv_nsec > b.tv_nsec) - (a.tv_nsec < b.tv_nsec);
  }
  return cmp;
}

////////////////////////////////////////////////////////////////////////////////
// ux explanatory logging for llama.com developers

#if 0
#define DEVLOG(...) (void)0
#else
#define DEVLOG(...) if (g_devlog) fprintf(g_devlog, __VA_ARGS__)
static FILE *g_devlog;
__attribute__((__constructor__)) static void init(void) {
    char path[PATH_MAX];
    static char linebuf[4096];
    snprintf(path, sizeof(path), "/tmp/llama-%s.log", getenv("USER"));
    if ((g_devlog = fopen(path, "wa"))) {
        setvbuf(g_devlog, linebuf, _IOLBF, sizeof(linebuf));
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////

enum jtlp_status {
    kPromptPending,
    kPromptCompleted,
    kPromptFinished
};

struct jtlp_header {
    uint8_t magic[4];
    uint8_t version[4];
    uint8_t state_size[8];
    uint8_t model_dev[8];
    uint8_t model_ino[8];
    uint8_t model_mtim_sec[8];
    uint8_t model_mtim_nsec[8];
    uint8_t prompt_size[8];
};

constexpr uint32_t kJtlpMagic = 'j' | 't' << 8 | 'l' << 16 | 'p' << 24;
constexpr uint32_t kJtlpVersion = 0;

static std::string last_output;
static std::vector<llama_token> last_n_tokens;
static std::string::size_type longest_antiprompt;
static enum jtlp_status prompt_status = kPromptPending;

static void remember_init() {
    last_output.clear();
    last_n_tokens.resize(llama_n_ctx(ctx), 0);
    for (std::string & antiprompt : params.antiprompt) {
        longest_antiprompt = std::max(longest_antiprompt, antiprompt.size());
    }
    longest_antiprompt += llama_longest_token(ctx) * 2;
}

static void remember_token(llama_token tok,
                           bool is_user_input = false) {
    last_n_tokens.erase(last_n_tokens.begin());
    last_n_tokens.push_back(tok);
    if (!is_user_input) {
        last_output.append(llama_token_to_str(ctx, tok));
        if (last_output.size() > longest_antiprompt) {
            last_output.erase(0, last_output.size() - longest_antiprompt);
        }
    }
    DEVLOG("remember_token(%`'s, %d) -> %`'s\n",
           llama_token_to_str(ctx, tok), is_user_input,
           last_output.c_str());
}

static bool has_antiprompt(std::string::size_type *out_index = nullptr,
                           std::string *out_antiprompt = nullptr) {
    for (std::string & antiprompt : params.antiprompt) {
        std::string::size_type index = last_output.rfind(antiprompt);
        if (index != std::string::npos) {
            if (out_index) *out_index = index;
            if (out_antiprompt) *out_antiprompt = antiprompt;
            DEVLOG("found antiprompt %`'s at index %d of %`'s\n",
                   antiprompt.c_str(), (int)index, last_output.c_str());
            return true;
        }
    }
    return false;
}

static void finish_initializing_prompt() {
    prompt_status = kPromptFinished;
    if (params.interactive) {
        std::string::size_type ap_index;
        is_interacting = true;
        if (has_antiprompt(&ap_index)) {
            console_set_color(con_st, CONSOLE_COLOR_PROMPT);
            printf("%s", last_output.substr(ap_index).c_str());
            fflush(stdout);
        }
        console_set_color(con_st, CONSOLE_COLOR_USER_INPUT);
    }
    last_output.clear();
}

////////////////////////////////////////////////////////////////////////////////

static int on_missing_feature(const char *name) {
    fprintf(stderr, "%s: error: cpuid %s not detected\n", __func__, name);
    fprintf(stderr, "%s: amd microprocessors made after 2017 usually work\n", __func__);
    fprintf(stderr, "%s: intel microprocessors made after 2013 usually work\n", __func__);
    return 1;
}

int main(int argc, char ** argv) {

    verynice();
    ShowCrashReports();

    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    params.model = "models/llama-7B/ggml-model.bin";

#ifdef __x86_64__
    if (!X86_HAVE(AVX2)) return on_missing_feature("avx2");
    if (!X86_HAVE(AVX)) return on_missing_feature("avx");
    if (!X86_HAVE(FMA)) return on_missing_feature("fma");
    if (!X86_HAVE(SSE3)) return on_missing_feature("sse3");
    if (!X86_HAVE(F16C)) return on_missing_feature("f16c");
#endif /* __x86_64__ */

    if (gpt_params_parse(argc, argv, params) == false) {
        return 1;
    }

    // save choice to use color for later
    // (note for later: this is a slightly awkward choice)
    con_st.use_color = params.use_color;

    con_st.multiline_input = params.multiline_input;
    console_init(con_st);
    atexit([]() { console_cleanup(con_st); });

    if (params.perplexity) {
        printf("\n************\n");
        printf("%s: please use the 'perplexity' tool for perplexity calculations\n", __func__);
        printf("************\n\n");

        return 0;
    }

    if (params.embedding) {
        printf("\n************\n");
        printf("%s: please use the 'embedding' tool for embedding calculations\n", __func__);
        printf("************\n\n");

        return 0;
    }

    if (params.n_ctx > 2048) {
        fprintf(stderr, "%s: warning: model does not support context sizes greater than 2048 tokens (%d specified);"
                "expect poor results\n", __func__, params.n_ctx);
    }

    if (params.seed < 0) {
        params.seed = time(NULL);
    }

    if (params.verbose > 0) {
        fprintf(stderr, "%s: seed = %d\n", __func__, params.seed);
    }

    std::mt19937 rng(params.seed);
    if (params.random_prompt) {
        params.prompt = gpt_random_prompt(rng);
    }

//    params.prompt = R"(// this function checks if the number n is prime
//bool is_prime(int n) {)";

    struct stat model_stat;

    // load the model and apply lora adapter, if any
    ctx = llama_init_from_gpt_params(params);
    if (ctx == NULL) {
        fprintf(stderr, "%s: error: unable to load model\n", __func__);
        return 1;
    }

    stat(params.model.c_str(), &model_stat);

    if (!params.lora_adapter.empty()) {
        int err = llama_apply_lora_from_file(ctx,
                                             params.lora_adapter.c_str(),
                                             params.lora_base.empty() ? NULL : params.lora_base.c_str(),
                                             params.n_threads);
        if (err != 0) {
            fprintf(stderr, "%s: error: failed to apply lora adapter\n", __func__);
            return 1;
        }
    }

    // print system information
    if (params.verbose > 0) {
        fprintf(stderr, "\n");
        fprintf(stderr, "system_info: n_threads = %d / %d | %s\n",
                params.n_threads, std::thread::hardware_concurrency(), llama_print_system_info());
    }

    // determine the maximum memory usage needed to do inference for the given n_batch and n_predict parameters
    // uncomment the "used_mem" line in llama.cpp to see the results
    if (params.mem_test) {
        {
            const std::vector<llama_token> tmp(params.n_batch, 0);
            llama_eval(ctx, tmp.data(), tmp.size(), 0, params.n_threads);
        }

        {
            const std::vector<llama_token> tmp = { 0, };
            llama_eval(ctx, tmp.data(), tmp.size(), params.n_predict - 1, params.n_threads);
        }

        if (params.verbose > 0) {
            llama_print_timings(ctx);
        }
        llama_free(ctx);

        return 0;
    }

    // Add a space in front of the first character to match OG llama tokenizer behavior
    // params.prompt.insert(0, 1, ' ');

    // tokenize the prompt
    auto embd_inp = ::llama_tokenize(ctx, params.prompt, true);

    const int n_ctx = llama_n_ctx(ctx);

    if ((int) embd_inp.size() > n_ctx - 4) {
        fprintf(stderr, "%s: error: prompt is too long (%d tokens, max %d)\n", __func__, (int) embd_inp.size(), n_ctx - 4);
        return 1;
    }

    // number of tokens to keep when resetting context
    int n_keep = params.n_keep;
    if (n_keep < 0 || n_keep > (int)embd_inp.size() || params.instruct) {
        n_keep = (int)embd_inp.size();
    }
    if (!n_keep && !params.n_keep_str.empty()) {
        auto pivot = ::llama_tokenize(ctx, params.n_keep_str, false);
        auto pos = std::search(embd_inp.begin(), embd_inp.end(),
                               pivot.begin(), pivot.end());
        if (pos == embd_inp.end()) {
            fprintf(stderr, "%s: error: --n_keep %`'s substring not found within prompt\n",
                    __func__, params.n_keep_str.c_str());
            return 1;
        }
        n_keep = (pos - embd_inp.begin()) + (pivot.end() - pivot.begin());
    }

    // prefix & suffix for instruct mode
    const auto inp_pfx = ::llama_tokenize(ctx, "\n\n### Instruction:\n\n", true);
    const auto inp_sfx = ::llama_tokenize(ctx, "\n\n### Response:\n\n", false);

    // in instruct mode, we inject a prefix and a suffix to each input by the user
    if (params.instruct) {
        params.interactive_first = true;
        params.antiprompt.push_back("### Instruction:\n\n");
    }

    // enable interactive mode if interactive start is specified
    if (params.interactive_first) {
        params.interactive = true;
    }

    // determine newline token
    auto llama_token_newline = ::llama_tokenize(ctx, "\n", false);

    if (params.verbose_prompt) {
        fprintf(stderr, "\n");
        fprintf(stderr, "%s: prompt: '%s'\n", __func__, params.prompt.c_str());
        fprintf(stderr, "%s: number of tokens in prompt = %zu\n", __func__, embd_inp.size());
        for (int i = 0; i < (int) embd_inp.size(); i++) {
            fprintf(stderr, "%6d %6d -> %`'s\n", i, embd_inp[i], llama_token_to_str(ctx, embd_inp[i]));
        }
        fprintf(stderr, "%s: first part of prompt: \"", __func__);
        for (int i = 0; i < n_keep; i++) {
            fprintf(stderr, "%'s", llama_token_to_str(ctx, embd_inp[i]));
        }
        fprintf(stderr, "\"\n");
        fprintf(stderr, "%s: second part of prompt: \"", __func__);
        for (int i = n_keep; i < (int)embd_inp.size(); i++) {
            fprintf(stderr, "%'s", llama_token_to_str(ctx, embd_inp[i]));
        }
        fprintf(stderr, "\"\n");
        fprintf(stderr, "\n");
    }

    // setup ctrl-c handler
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (params.interactive) {
        sa.sa_handler = sigint_handler_interactive;
    } else {
        sa.sa_handler = sigint_handler_batch;
    }
    sigaction(SIGINT, &sa, NULL);

    if (params.interactive) {
        if (params.verbose > 0) {
            fprintf(stderr, "%s: interactive mode on.\n", __func__);
        }

        if (params.verbose > 0 && params.antiprompt.size()) {
            for (auto antiprompt : params.antiprompt) {
                fprintf(stderr, "Reverse prompt: '%s'\n", antiprompt.c_str());
            }
        }

        if (params.verbose > 0 && !params.input_prefix.empty()) {
            fprintf(stderr, "Input prefix: '%s'\n", params.input_prefix.c_str());
        }
    }

    if (params.verbose > 0) {
        fprintf(stderr, "sampling: temp = %f, top_k = %d, top_p = %f, repeat_last_n = %i, repeat_penalty = %f\n",
                params.temp, params.top_k, params.top_p, params.repeat_last_n, params.repeat_penalty);
        fprintf(stderr, "generate: n_ctx = %d, n_batch = %d, n_predict = %d, n_keep = %d\n",
                n_ctx, params.n_batch, params.n_predict, n_keep);
        fprintf(stderr, "\n\n");
    }

    if (params.verbose > 0 && params.interactive) {
        fprintf(stderr, "== Running in interactive mode. ==\n"
               " - Press Ctrl+C to interject at any time.\n"
               " - Press Return to return control to LLaMa.\n"
               " - If you want to submit another line, end your input in '\\'.\n\n");
        is_interacting = params.interactive_first;
    }

    remember_init();

    input_noecho = params.verbose <= 0;

    n_past     = 0;
    n_remain   = params.n_predict;
    n_consumed = 0;

    // instantly reload prompt if it's cached
    int fd = open(params.prompt_path.c_str(), O_RDONLY);
    if (fd != -1) {
        size_t state_size;
        size_t prompt_size;
        struct timespec mtim;
        struct jtlp_header *header;
        off_t rc = lseek(fd, 0, SEEK_END);
        LLAMA_ASSERT(rc != -1);
        void *map = MAP_FAILED;
        size_t file_size = rc;
        if (file_size < sizeof(header)) {
            fprintf(stderr, "%s: prompt file too small\n",
                    params.prompt_path.c_str());
            goto CantReloadPrompt;
        }
        map = mmap(0, file_size, PROT_READ, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED) {
            fprintf(stderr, "%s: mmap failed: %s\n",
                    params.prompt_path.c_str(), strerror(errno));
            goto CantReloadPrompt;
        }
        header = (struct jtlp_header *)map;
        // check file format magic
        if (READ32LE(header->magic) != kJtlpMagic) {
            fprintf(stderr, "%s: prompt file has wrong magic\n",
                    params.prompt_path.c_str());
            goto CantReloadPrompt;
        }
        // check file format version
        if (READ32LE(header->version) > kJtlpVersion) {
            fprintf(stderr, "%s: prompt has future file format version\n",
                    params.prompt_path.c_str());
            goto CantReloadPrompt;
        }
        // check expected state size
        state_size = llama_get_state_size(ctx);
        if (READ64LE(header->state_size) != state_size) {
            if (params.verbose > 0) {
                fprintf(stderr, "%s: prompt has stale data state size\n",
                        params.prompt_path.c_str());
            }
            goto CantReloadPrompt;
        }
        // check model device id
        if (READ64LE(header->model_dev) != model_stat.st_dev) {
            fprintf(stderr, "%s: prompt is for different model (dev)\n",
                    params.prompt_path.c_str());
            goto CantReloadPrompt;
        }
        // check model inode id
        if (READ64LE(header->model_ino) != model_stat.st_ino) {
            fprintf(stderr, "%s: prompt is for different model (ino)\n",
                    params.prompt_path.c_str());
            goto CantReloadPrompt;
        }
        // check model modified timestamp
        mtim.tv_sec = READ64LE(header->model_mtim_sec);
        mtim.tv_nsec = READ64LE(header->model_mtim_nsec);
        if (CompareTime(model_stat.st_mtim, mtim) > 0) {
            if (params.verbose > 0) {
                fprintf(stderr, "%s: model file timestamp changed; will reload and regenerate prompt\n",
                        params.prompt_path.c_str());
            }
            goto CantReloadPrompt;
        }
        // check prompt file size
        prompt_size = READ64LE(header->prompt_size);
        if (sizeof(struct jtlp_header) + prompt_size + state_size > file_size) {
            fprintf(stderr, "%s: prompt file size unexpected\n",
                    params.prompt_path.c_str());
            goto CantReloadPrompt;
        }
        // check prompt textus
        if (prompt_size != params.prompt.size() ||
            memcmp(header + 1, params.prompt.c_str(), prompt_size) != 0) {
            if (params.verbose > 0) {
                fprintf(stderr, "%s: prompt text changed; will reload and regenerate\n",
                        params.prompt_path.c_str());
            }
            goto CantReloadPrompt;
        }
        // read the transformer state
        llama_set_state_data(ctx, (uint8_t *)(header + 1) + prompt_size);
        // we're finished loading the prompt file
        if (params.verbose > 0) {
            fprintf(stderr, "%s: %s: reloaded previously saved prompt\n",
                    __func__, params.prompt_path.c_str());
        }
        // now setup the business logic
        llama_set_rng_seed(ctx, params.seed);
        while ((int) embd_inp.size() > n_consumed) {
            remember_token(embd_inp[n_consumed++]);
        }
        n_past = n_consumed;
        finish_initializing_prompt();
  CantReloadPrompt:
        if (map != MAP_FAILED) {
            munmap(map, file_size);
        }
        close(fd);
    }

    if (prompt_status == kPromptPending && params.verbose > 0) {
        // the first thing we will do is to output the prompt, so set color accordingly
        console_set_color(con_st, CONSOLE_COLOR_PROMPT);
    }

    std::vector<llama_token> embd;

    if (prompt_status == kPromptPending &&
        !params.verbose && con_st.use_color) {
        fprintf(stderr, EPHEMERAL("loading weights..."));
    }

    // tracks if last character written to stdout was newline
    bool got_newline = false;

    while ((n_remain != 0 || params.interactive) && !is_terminated) {

        // perform evaluation
        if (embd.size() > 0) {
            DEVLOG("perform evaluation embd.size()=%d\n", (int)embd.size());
            if (n_past + (int) embd.size() > n_ctx) {
                n_past = n_keep;
                embd.insert(embd.begin(),
                            last_n_tokens.end() - (n_past - n_keep) / 2 - embd.size(),
                            last_n_tokens.end() - embd.size());
            }
            for (int i = 0; i < (int) embd.size() && !is_terminated; i += params.n_batch) {
                int n_eval = (int) embd.size() - i;
                if (n_eval > params.n_batch) {
                    n_eval = params.n_batch;
                }
                is_stalled = n_eval > 1;
                DEVLOG("llama_eval(n_evel=%d, n_past=%d)\n", n_eval, n_past);
                if (llama_eval(ctx, &embd[i], n_eval, n_past, params.n_threads)) {
                    fprintf(stderr, "%s : failed to eval\n", __func__);
                    console_set_color(con_st, CONSOLE_COLOR_DEFAULT);
                    return 1;
                }
                is_stalled = false;
                n_past += n_eval;
                if (prompt_status == kPromptPending &&
                    !params.verbose && con_st.use_color && embd_inp.size()) {
                    fprintf(stderr, EPHEMERAL("loading prompt %d%% ..."),
                            (int)(n_consumed / (double)embd_inp.size() * 100));
                }
            }
            if (is_terminated) {
                break;
            }
            embd.clear();
        }

        // save prompt to disk atomically as soon as it's finished loading
        bool just_finished_initializing_prompt = prompt_status == kPromptCompleted;
        if (just_finished_initializing_prompt && !params.prompt_path.empty()) {
            int fd = -1;
            int close_rc;
            size_t file_size;
            size_t state_size;
            std::string tmppath;
            void *map = MAP_FAILED;
            struct jtlp_header header;
            if (!params.verbose && con_st.use_color) {
                fprintf(stderr, EPHEMERAL("caching prompt..."));
            }
            state_size = llama_get_state_size(ctx);
            WRITE32LE(header.magic, kJtlpMagic);
            WRITE32LE(header.version, kJtlpVersion);
            WRITE64LE(header.state_size, state_size);
            WRITE64LE(header.model_dev, model_stat.st_dev);
            WRITE64LE(header.model_ino, model_stat.st_ino);
            WRITE64LE(header.model_mtim_sec, model_stat.st_mtim.tv_sec);
            WRITE64LE(header.model_mtim_nsec, model_stat.st_mtim.tv_nsec);
            WRITE64LE(header.prompt_size, params.prompt.size());
            file_size = sizeof(header) + params.prompt.size() + state_size;
            tmppath.append(params.prompt_path);
            tmppath.append(".XXXXXX");
            fd = mkstemp(&tmppath[0]);
            if (fd == -1) {
                fprintf(stderr, "%s: mkstemp failed: %s\n",
                        tmppath.c_str(), strerror(errno));
                goto CouldNotSavePrompt;
            }
            if (ftruncate(fd, file_size)) {
                fprintf(stderr, "%s: ftruncate failed: %s\n",
                        tmppath.c_str(), strerror(errno));
                goto CouldNotSavePrompt;
            }
            map = mmap(0, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (map == MAP_FAILED) {
                fprintf(stderr, "%s: mmap failed: %s\n",
                        tmppath.c_str(), strerror(errno));
                goto CouldNotSavePrompt;
            }
            llama_copy_state_data(ctx, (uint8_t *)map + sizeof(header) + params.prompt.size());
            memcpy((uint8_t *)map + sizeof(header), params.prompt.c_str(), params.prompt.size());
            memcpy(map, &header, sizeof(header));
            if (msync(map, file_size, MS_ASYNC) && params.verbose > 0) {
                fprintf(stderr, "%s: msync failed: %s\n",
                        tmppath.c_str(), strerror(errno));
            }
            if (munmap(map, file_size) && params.verbose > 0) {
                fprintf(stderr, "%s: munmap failed: %s\n",
                        tmppath.c_str(), strerror(errno));
            }
            map = MAP_FAILED;
            close_rc = close(fd);
            fd = -1;
            if (close_rc) {
                fprintf(stderr, "%s: close failed: %s\n",
                        tmppath.c_str(), strerror(errno));
                goto CouldNotSavePrompt;
            }
            if (rename(tmppath.c_str(), params.prompt_path.c_str())) {
                fprintf(stderr, "%s -> %s: rename failed: %s\n",
                        tmppath.c_str(), params.prompt_path.c_str(), strerror(errno));
                goto CouldNotSavePrompt;
            }
            tmppath.clear();
      CouldNotSavePrompt:
            if (map != MAP_FAILED) munmap(map, file_size);
            if (fd != -1) close(fd);
            if (!tmppath.empty()) unlink(tmppath.c_str());
        }
        if (just_finished_initializing_prompt) {
            if (!params.verbose && con_st.use_color) {
                fprintf(stderr, EPHEMERAL(""));
            }
            finish_initializing_prompt();
        }

        if (prompt_status == kPromptFinished &&
            (int) embd_inp.size() <= n_consumed && !is_interacting) {
            // out of user input, sample next token
            DEVLOG("out of user input, sample next token w/ embd_inp.size()=%d n_consumed=%d\n",
                   (int)embd_inp.size(), n_consumed);
            const float   temp            = params.temp;
            const int32_t top_k           = params.top_k <= 0 ? llama_n_vocab(ctx) : params.top_k;
            const float   top_p           = params.top_p;
            const float   tfs_z           = params.tfs_z;
            const float   typical_p       = params.typical_p;
            const int32_t repeat_last_n   = params.repeat_last_n < 0 ? n_ctx : params.repeat_last_n;
            const float   repeat_penalty  = params.repeat_penalty;
            const float   alpha_presence  = params.presence_penalty;
            const float   alpha_frequency = params.frequency_penalty;
            const int     mirostat        = params.mirostat;
            const float   mirostat_tau    = params.mirostat_tau;
            const float   mirostat_eta    = params.mirostat_eta;
            const bool    penalize_nl     = params.penalize_nl;

            llama_token id = 0;

            {
                auto logits  = llama_get_logits(ctx);
                auto n_vocab = llama_n_vocab(ctx);

                // Apply params.logit_bias map
                for (auto it = params.logit_bias.begin(); it != params.logit_bias.end(); it++) {
                    logits[it->first] += it->second;
                }

                std::vector<llama_token_data> candidates;
                candidates.reserve(n_vocab);
                for (llama_token token_id = 0; token_id < n_vocab; token_id++) {
                    candidates.emplace_back(llama_token_data{token_id, logits[token_id], 0.0f});
                }

                llama_token_data_array candidates_p = { candidates.data(), candidates.size(), false };

                // Apply penalties
                float nl_logit = logits[llama_token_nl()];
                auto last_n_repeat = std::min(std::min((int)last_n_tokens.size(), repeat_last_n), n_ctx);
                llama_sample_repetition_penalty(ctx, &candidates_p,
                    last_n_tokens.data() + last_n_tokens.size() - last_n_repeat,
                    last_n_repeat, repeat_penalty);
                llama_sample_frequency_and_presence_penalties(ctx, &candidates_p,
                    last_n_tokens.data() + last_n_tokens.size() - last_n_repeat,
                    last_n_repeat, alpha_frequency, alpha_presence);
                if (!penalize_nl) {
                    logits[llama_token_nl()] = nl_logit;
                }

                if (temp <= 0) {
                    // Greedy sampling
                    id = llama_sample_token_greedy(ctx, &candidates_p);
                } else {
                    if (mirostat == 1) {
                        static float mirostat_mu = 2.0f * mirostat_tau;
                        const int mirostat_m = 100;
                        llama_sample_temperature(ctx, &candidates_p, temp);
                        id = llama_sample_token_mirostat(ctx, &candidates_p, mirostat_tau, mirostat_eta, mirostat_m, &mirostat_mu);
                    } else if (mirostat == 2) {
                        static float mirostat_mu = 2.0f * mirostat_tau;
                        llama_sample_temperature(ctx, &candidates_p, temp);
                        id = llama_sample_token_mirostat_v2(ctx, &candidates_p, mirostat_tau, mirostat_eta, &mirostat_mu);
                    } else {
                        // Temperature sampling
                        llama_sample_top_k(ctx, &candidates_p, top_k, 1);
                        llama_sample_tail_free(ctx, &candidates_p, tfs_z, 1);
                        llama_sample_typical(ctx, &candidates_p, typical_p, 1);
                        llama_sample_top_p(ctx, &candidates_p, top_p, 1);
                        llama_sample_temperature(ctx, &candidates_p, temp);
                        id = llama_sample_token(ctx, &candidates_p);
                    }
                }

                remember_token(id);
            }

            // replace end of text token with newline token when in interactive mode
            if (id == llama_token_eos() && params.interactive && !params.instruct) {
                id = llama_token_newline.front();
                if (params.antiprompt.size() != 0) {
                    // tokenize and inject first reverse prompt
                    const auto first_antiprompt = ::llama_tokenize(ctx, params.antiprompt.front(), false);
                    embd_inp.insert(embd_inp.end(), first_antiprompt.begin(), first_antiprompt.end());
                }
            }

            // add it to the context
            embd.push_back(id);

            // echo this to console
            input_noecho = false;

            // decrement remaining sampling budget
            --n_remain;

        } else {
            DEVLOG("some user input remains from prompt or interaction w/ embd_inp.size()=%d n_consumed=%d\n",
                   (int)embd_inp.size(), n_consumed);
            // some user input remains from prompt or interaction, forward it to processing
            while ((int) embd_inp.size() > n_consumed) {
                embd.push_back(embd_inp[n_consumed]);
                remember_token(embd_inp[n_consumed++], true);
                if ((int) embd.size() >= params.n_batch) {
                    break;
                }
            }

            // we've nearly finished loading the prompt
            if (prompt_status == kPromptPending &&
                (int) embd_inp.size() <= n_consumed) {
                prompt_status = kPromptCompleted;
            }
        }

        // checks for reverse prompt
        //
        // 1. in interactive mode, this lets us detect when the llm is
        //    prompting the user, so we can pause for input, e.g.
        //
        //       --interactive
        //       --prompt $'CompanionAI: How can I help you?\nHuman:'
        //       --reverse-prompt 'Human:'
        //
        // 2. in normal mode, the reverse prompt can be used to specify
        //    a custom EOS token, e.g.
        //
        //       --prompt 'Question: How old are you?\nAnswer: '
        //       --reverse-prompt $'\n'
        //
        bool is_antiprompt;
        std::string ap_text;
        std::string::size_type ap_extra;
        std::string::size_type ap_index;
        if (prompt_status == kPromptFinished) {
            is_antiprompt = has_antiprompt(&ap_index, &ap_text);
        } else {
            is_antiprompt = false;
        }

        // display text
        if (!input_noecho && embd.size()) {
            std::string printme;
            for (auto id : embd) {
                printme.append(llama_token_to_str(ctx, id));
            }
            if (is_antiprompt) {
                ap_extra = last_output.size() - ap_index;
                printme.erase(std::max(0, (int)(printme.size() - ap_extra)));
            }
            if (printme.size()) {
                got_newline = printme[printme.size() - 1] == '\n';
                printf("%s", printme.c_str());
                fflush(stdout);
            }
        }
        if (is_antiprompt) {
            if (!params.interactive) {
                DEVLOG("exiting due to antiprompt\n");
                if (!got_newline) {
                    printf("\n");
                }
                break;
            }
            // scrub antiprompt so to detect it must be typed again
            last_output.erase(0, ap_index + ap_text.size());
            DEVLOG("scrubbed antiprompt -> %`'s\n", last_output.c_str());
        }
        if (prompt_status == kPromptCompleted) {
            DEVLOG("avoid reading line before last token loads\n");
            continue;  // avoid reading line before last token loads
        }

        // reset color to default if we there is no pending user input
        if (params.verbose && !input_noecho && (int)embd_inp.size() == n_consumed) {
            console_set_color(con_st, CONSOLE_COLOR_DEFAULT);
        }

        if (is_antiprompt) {
            is_interacting = true;
            console_set_color(con_st, CONSOLE_COLOR_USER_INPUT);
            fflush(stdout);
        }

        // in interactive mode, and not currently processing queued inputs;
        // check if we should prompt the user for more
        if (params.interactive && (int) embd_inp.size() <= n_consumed) {

            if (n_past > 0 && is_interacting) {

                // potentially set color to indicate we are taking user input
                console_set_color(con_st, CONSOLE_COLOR_USER_INPUT);

                if (params.instruct) {
                    printf("\n> ");
                }

                std::string buffer;
                if (!params.input_prefix.empty()) {
                    buffer += params.input_prefix;
                    printf("%s", buffer.c_str());
                }

                // display a "waiting for input" indicator, just in case
                // the model doesn't halt on the antiprompt.
                if (con_st.use_color) {
                    fprintf(stdout, "?\b");
                    fflush(stdout);
                }

                std::string line;
                bool another_line = true;
                do {
                    another_line = console_readline(con_st, line);
                    buffer += line;
                } while (another_line);

                // done taking input, reset color
                console_set_color(con_st, CONSOLE_COLOR_DEFAULT);

                // Add tokens to embd only if the input buffer is non-empty
                // Entering a empty line lets the user pass control back
                if (buffer.length() > 1) {
                    // append input suffix if any
                    if (!params.input_suffix.empty()) {
                        buffer += params.input_suffix;
                        printf("%s", params.input_suffix.c_str());
                    }

                    // instruct mode: insert instruction prefix
                    if (params.instruct && !is_antiprompt) {
                        n_consumed = embd_inp.size();
                        embd_inp.insert(embd_inp.end(), inp_pfx.begin(), inp_pfx.end());
                    }

                    auto line_inp = ::llama_tokenize(ctx, buffer, false);
                    embd_inp.insert(embd_inp.end(), line_inp.begin(), line_inp.end());

                    // instruct mode: insert response suffix
                    if (params.instruct) {
                        embd_inp.insert(embd_inp.end(), inp_sfx.begin(), inp_sfx.end());
                    }

                    n_remain -= line_inp.size();
                }

                input_noecho = true; // do not echo this again
            }

            if (n_past > 0) {
                is_interacting = false;
            }
            assert(!is_interacting);
        }

        // end of text token
        if (!embd.empty() && embd.back() == llama_token_eos()) {
            if (params.instruct) {
                is_interacting = true;
            } else if (params.verbose > 0) {
                fprintf(stderr, " [end of text]\n");
                break;
            }
        }

        // In interactive mode, respect the maximum number of tokens and drop back to user input when reached.
        if (params.interactive && n_remain <= 0 && params.n_predict != -1) {
            n_remain = params.n_predict;
            is_interacting = true;
        }
    }

    if (is_terminated) {
        console_cleanup(con_st);
        printf("\n");
        if (params.verbose > 0) {
            llama_print_timings(ctx);
        }
        _exit(128 + SIGINT);
    }

    if (params.verbose > 0) {
        llama_print_timings(ctx);
    }
    llama_free(ctx);

    console_set_color(con_st, CONSOLE_COLOR_DEFAULT);

    return 0;
}
