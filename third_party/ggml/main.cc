/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  llama.cpp                                                                   │
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

asm(".ident\t\"\\n\\n\
llama.cpp (MIT License)\\n\
Copyright (c) 2023 Georgi Gerganov\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

// Defines sigaction on msys:
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "third_party/ggml/common.h"
#include "libc/nexgen32e/x86feature.h"
#include "third_party/ggml/llama.h"

#include "third_party/libcxx/cassert"
#include "third_party/libcxx/cinttypes"
#include "third_party/libcxx/cmath"
#include "third_party/libcxx/cstdio"
#include "third_party/libcxx/cstring"
#include "third_party/libcxx/ctime"
#include "third_party/libcxx/fstream"
#include "third_party/libcxx/iostream"
#include "third_party/libcxx/string"
#include "third_party/libcxx/vector"

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include "libc/calls/calls.h"
#include "libc/calls/sigtimedwait.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/ss.h"
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/runtime/pathconf.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/time/time.h"
#include "third_party/getopt/getopt.h"
#include "third_party/musl/crypt.h"
#include "third_party/musl/lockf.h"
#elif defined (_WIN32)
#include "libc/calls/calls.h"
#include "libc/calls/sigtimedwait.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/ss.h"
#endif

static console_state con_st;
static llama_context ** g_ctx;

static bool is_interacting = false;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)) || defined (_WIN32)
void sigint_handler(int signo) {
    set_console_color(con_st, CONSOLE_COLOR_DEFAULT);
    printf("\n"); // this also force flush stdout.
    if (signo == SIGINT) {
        if (!is_interacting) {
            is_interacting=true;
        } else {
            _exit(128 + signo);
        }
    }
}
#endif

static int on_missing_feature(const char *name) {
    fprintf(stderr, "error: we require %s support in your microprocessor.\n", name);
    return 1;
}

int main(int argc, char ** argv) {
    gpt_params params;
    params.model = "models/llama-7B/ggml-model.bin";

    if (!X86_HAVE(AVX2)) return on_missing_feature("avx2");
    if (!X86_HAVE(AVX)) return on_missing_feature("avx");
    if (!X86_HAVE(FMA)) return on_missing_feature("fma");
    if (!X86_HAVE(F16C)) return on_missing_feature("f16c");
    if (!X86_HAVE(SSE3)) return on_missing_feature("sse3");

    if (gpt_params_parse(argc, argv, params) == false) {
        return 1;
    }

    // save choice to use color for later
    // (note for later: this is a slightly awkward choice)
    con_st.use_color = params.use_color;

#if defined (_WIN32)
    win32_console_init(params.use_color);
#endif

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

    if (params.seed <= 0) {
        params.seed = time(NULL);
    }

    if (params.verbose) {
        fprintf(stderr, "%s: seed = %d\n", __func__, params.seed);
    }

    std::mt19937 rng(params.seed);
    if (params.random_prompt) {
        params.prompt = gpt_random_prompt(rng);
    }

//    params.prompt = R"(// this function checks if the number n is prime
//bool is_prime(int n) {)";

    llama_context * ctx;
    g_ctx = &ctx;

    // load the model
    {
        auto lparams = llama_context_default_params();

        lparams.n_ctx      = params.n_ctx;
        lparams.n_parts    = params.n_parts;
        lparams.seed       = params.seed;
        lparams.f16_kv     = params.memory_f16;
        lparams.use_mmap   = params.use_mmap;
        lparams.use_mlock  = params.use_mlock;

        ctx = llama_init_from_file(params.model.c_str(), lparams, params.verbose);

        if (ctx == NULL) {
            fprintf(stderr, "%s: error: failed to load model '%s'\n", __func__, params.model.c_str());
            return 1;
        }
    }

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
    if (params.verbose) {
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

        if (params.verbose) {
            llama_print_timings(ctx);
        }
        llama_free(ctx);

        return 0;
    }

    // Add a space in front of the first character to match OG llama tokenizer behavior
    params.prompt.insert(0, 1, ' ');

    // tokenize the prompt
    auto embd_inp = ::llama_tokenize(ctx, params.prompt, true);

    const int n_ctx = llama_n_ctx(ctx);

    if ((int) embd_inp.size() > n_ctx - 4) {
        fprintf(stderr, "%s: error: prompt is too long (%d tokens, max %d)\n", __func__, (int) embd_inp.size(), n_ctx - 4);
        return 1;
    }

    // number of tokens to keep when resetting context
    if (params.n_keep < 0 || params.n_keep > (int)embd_inp.size() || params.instruct) {
        params.n_keep = (int)embd_inp.size();
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
            fprintf(stderr, "%6d -> '%s'\n", embd_inp[i], llama_token_to_str(ctx, embd_inp[i]));
        }
        if (params.n_keep > 0) {
        fprintf(stderr, "%s: static prompt based on n_keep: '", __func__);
            for (int i = 0; i < params.n_keep; i++) {
                fprintf(stderr, "%s", llama_token_to_str(ctx, embd_inp[i]));
            }
            fprintf(stderr, "'\n");
        }
        fprintf(stderr, "\n");
    }

    if (params.interactive) {
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
        struct sigaction sigint_action;
        sigint_action.sa_handler = sigint_handler;
        sigemptyset (&sigint_action.sa_mask);
        sigint_action.sa_flags = 0;
        sigaction(SIGINT, &sigint_action, NULL);
#elif defined (_WIN32)
        signal(SIGINT, sigint_handler);
#endif

        if (params.verbose) {
            fprintf(stderr, "%s: interactive mode on.\n", __func__);
        }

        if (params.verbose && params.antiprompt.size()) {
            for (auto antiprompt : params.antiprompt) {
                fprintf(stderr, "Reverse prompt: '%s'\n", antiprompt.c_str());
            }
        }

        if (params.verbose && !params.input_prefix.empty()) {
            fprintf(stderr, "Input prefix: '%s'\n", params.input_prefix.c_str());
        }
    }
    
    if (params.verbose) {
        fprintf(stderr, "sampling: temp = %f, top_k = %d, top_p = %f, repeat_last_n = %i, repeat_penalty = %f\n",
                params.temp, params.top_k, params.top_p, params.repeat_last_n, params.repeat_penalty);
        fprintf(stderr, "generate: n_ctx = %d, n_batch = %d, n_predict = %d, n_keep = %d\n", n_ctx, params.n_batch, params.n_predict, params.n_keep);
        fprintf(stderr, "\n\n");
    }

    // TODO: replace with ring-buffer
    std::vector<llama_token> last_n_tokens(n_ctx);
    std::fill(last_n_tokens.begin(), last_n_tokens.end(), 0);

    if (params.verbose && params.interactive) {
        fprintf(stderr, "== Running in interactive mode. ==\n"
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)) || defined (_WIN32)
               " - Press Ctrl+C to interject at any time.\n"
#endif
               " - Press Return to return control to LLaMa.\n"
               " - If you want to submit another line, end your input in '\\'.\n\n");
        is_interacting = params.interactive_first;
    }

    bool is_antiprompt = false;
    bool input_noecho  = !params.verbose;

    int n_past     = 0;
    int n_remain   = params.n_predict;
    int n_consumed = 0;

    // the first thing we will do is to output the prompt, so set color accordingly
    set_console_color(con_st, CONSOLE_COLOR_PROMPT);

    std::vector<llama_token> embd;

    while (n_remain != 0 || params.interactive) {
        // predict
        if (embd.size() > 0) {
            // infinite text generation via context swapping
            // if we run out of context:
            // - take the n_keep first tokens from the original prompt (via n_past)
            // - take half of the last (n_ctx - n_keep) tokens and recompute the logits in batches
            if (n_past + (int) embd.size() > n_ctx) {
                const int n_left = n_past - params.n_keep;

                n_past = params.n_keep;

                // insert n_left/2 tokens at the start of embd from last_n_tokens
                embd.insert(embd.begin(), last_n_tokens.begin() + n_ctx - n_left/2 - embd.size(), last_n_tokens.end() - embd.size());

                //printf("\n---\n");
                //printf("resetting: '");
                //for (int i = 0; i < (int) embd.size(); i++) {
                //    printf("%s", llama_token_to_str(ctx, embd[i]));
                //}
                //printf("'\n");
                //printf("\n---\n");
            }

            // evaluate tokens in batches
            // embd is typically prepared beforehand to fit within a batch, but not always
            for (int i = 0; i < (int) embd.size(); i += params.n_batch) {
                int n_eval = (int) embd.size() - i;
                if (n_eval > params.n_batch) {
                    n_eval = params.n_batch;
                }
                if (llama_eval(ctx, &embd[i], n_eval, n_past, params.n_threads)) {
                    fprintf(stderr, "%s : failed to eval\n", __func__);
                    return 1;
                }
                n_past += n_eval;
            }
        }

        embd.clear();

        if ((int) embd_inp.size() <= n_consumed && !is_interacting) {
            // out of user input, sample next token
            const int32_t top_k          = params.top_k;
            const float   top_p          = params.top_p;
            const float   temp           = params.temp;
            const float   repeat_penalty = params.repeat_penalty;

            llama_token id = 0;

            {
                auto logits = llama_get_logits(ctx);

                if (params.ignore_eos) {
                    logits[llama_token_eos()] = 0;
                }

                id = llama_sample_top_p_top_k(ctx,
                        last_n_tokens.data() + n_ctx - params.repeat_last_n,
                        params.repeat_last_n, top_k, top_p, temp, repeat_penalty);

                last_n_tokens.erase(last_n_tokens.begin());
                last_n_tokens.push_back(id);
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
            // some user input remains from prompt or interaction, forward it to processing
            while ((int) embd_inp.size() > n_consumed) {
                embd.push_back(embd_inp[n_consumed]);
                last_n_tokens.erase(last_n_tokens.begin());
                last_n_tokens.push_back(embd_inp[n_consumed]);
                ++n_consumed;
                if ((int) embd.size() >= params.n_batch) {
                    break;
                }
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
        if (params.antiprompt.size()) {
            std::string last_output;
            for (auto id : last_n_tokens) {
                last_output += llama_token_to_str(ctx, id);
            }
            is_antiprompt = false;
            // Check if each of the reverse prompts appears at the end of the output.
            for (std::string & antiprompt : params.antiprompt) {
                if (last_output.find(antiprompt.c_str(), last_output.length() - antiprompt.length(), antiprompt.length()) != std::string::npos) {
                    is_antiprompt = true;
                    break;
                }
            }
            if (is_antiprompt && !params.interactive) {
                printf("\n");
                break;
            }
        }

        // display text
        if (!input_noecho) {
            for (auto id : embd) {
                printf("%s", llama_token_to_str(ctx, id));
            }
            fflush(stdout);
        }
        // reset color to default if we there is no pending user input
        if (params.verbose && !input_noecho && (int)embd_inp.size() == n_consumed) {
            set_console_color(con_st, CONSOLE_COLOR_DEFAULT);
        }

        if (is_antiprompt) {
            is_interacting = true;
            set_console_color(con_st, CONSOLE_COLOR_USER_INPUT);
            fflush(stdout);
        }

        // in interactive mode, and not currently processing queued inputs;
        // check if we should prompt the user for more
        if (params.interactive && (int) embd_inp.size() <= n_consumed) {

            if (n_past > 0 && is_interacting) {
                // potentially set color to indicate we are taking user input
                set_console_color(con_st, CONSOLE_COLOR_USER_INPUT);

#if defined (_WIN32)
                // Windows: must reactivate sigint handler after each signal
                signal(SIGINT, sigint_handler);
#endif

                if (params.instruct) {
                    printf("\n> ");
                }

                std::string buffer;
                if (!params.input_prefix.empty()) {
                    buffer += params.input_prefix;
                    printf("%s", buffer.c_str());
                }

                std::string line;
                bool another_line = true;
                do {
#if defined(_WIN32)
                    std::wstring wline;
                    if (!std::getline(std::wcin, wline)) {
                        // input stream is bad or EOF received
                        return 0;
                    }
                    win32_utf8_encode(wline, line);
#else
                    if (!std::getline(std::cin, line)) {
                        // input stream is bad or EOF received
                        return 0;
                    }
#endif
                    if (line.empty() || line.back() != '\\') {
                        another_line = false;
                    } else {
                        line.pop_back(); // Remove the continue character
                    }
                    buffer += line + '\n'; // Append the line to the result
                } while (another_line);

                // done taking input, reset color
                set_console_color(con_st, CONSOLE_COLOR_DEFAULT);

                // Add tokens to embd only if the input buffer is non-empty
                // Entering a empty line lets the user pass control back
                if (buffer.length() > 1) {

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
        }

        // end of text token
        if (!embd.empty() && embd.back() == llama_token_eos()) {
            if (params.instruct) {
                is_interacting = true;
            } else if (params.verbose) {
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

#if defined (_WIN32)
    signal(SIGINT, SIG_DFL);
#endif

    if (params.verbose) {
        llama_print_timings(ctx);
    }
    llama_free(ctx);

    set_console_color(con_st, CONSOLE_COLOR_DEFAULT);

    return 0;
}
