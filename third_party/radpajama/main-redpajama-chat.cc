/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-│
│vi: set net ft=c++ ts=4 sts=4 sw=4 fenc=utf-8                              :vi│
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  radpajama.com                                                               │
│  Copyright (c) 2023 Ariel Núñez                                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/sigtimedwait.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/weirdtypes.h"
#include "libc/log/log.h"
#include "libc/runtime/pathconf.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/ss.h"
#include "libc/time/time.h"
#include "third_party/libcxx/algorithm"
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
#include "third_party/musl/crypt.h"
#include "third_party/musl/lockf.h"
#include "third_party/radpajama/common-gptneox.h"
#include "third_party/radpajama/gptneox.h"
// clang-format off

static console_state con_st;
static gptneox_context ** g_ctx;

static bool is_interacting = false;

void sigint_handler(int signo) {
    set_console_color(con_st, CONSOLE_COLOR_DEFAULT);
    printf("\n"); // this also force flush stdout.
    if (signo == SIGINT) {
        if (!is_interacting) {
            is_interacting=true;
        } else {
            gptneox_print_timings(*g_ctx);
            _exit(130);
        }
    }
}

int main(int argc, char ** argv) {
    gpt_params params;
    params.model = "./models/ggml-RedPajama-INCITE-Chat-3B-v1-f16.bin";

    con_st.use_color = true;
    params.n_ctx = 2048;
    params.seed = 1684054676;
    params.use_mmap = true;
    params.use_mlock = true;
    params.memory_f16 = true;
    params.mem_test = false;
    params.interactive = true;
    params.top_k = 30;
    params.top_p =  0.95;
    params.temp = 0.8;
    params.repeat_last_n = 3;
    params.repeat_penalty = 1.1;
    params.instruct = true;
    params.interactive = true;

    verynice();
    ShowCrashReports();

    if (gpt_params_parse(argc, argv, params) == false) {  return 1; }

    std::mt19937 rng(params.seed);
    gptneox_context * ctx;
    g_ctx = &ctx;

    {
        auto lparams = gptneox_context_default_params();

        lparams.n_ctx      = params.n_ctx;
        lparams.n_parts    = params.n_parts;
        lparams.seed       = params.seed;
        lparams.f16_kv     = params.memory_f16;
        lparams.use_mmap   = params.use_mmap;
        lparams.use_mlock  = params.use_mlock;

        ctx = gptneox_init_from_file(params.model.c_str(), lparams);

        if (ctx == NULL) {
            fprintf(stderr, "%s: error: failed to load model '%s'\n", __func__, params.model.c_str());
            return 1;
        }
    }

    if (!params.lora_adapter.empty()) {
        int err = gptneox_apply_lora_from_file(ctx,
                                               params.lora_adapter.c_str(),
                                               params.lora_base.empty() ? NULL : params.lora_base.c_str(),
                                               params.n_threads);
        if (err != 0) {
            fprintf(stderr, "%s: error: failed to apply lora adapter\n", __func__);
            return 1;
        }
    }

    verynice();
    ShowCrashReports();

    // Always interactive for RedPajama chat model
    params.interactive = true;

    if (params.interactive) {
        struct sigaction sigint_action;
        sigint_action.sa_handler = sigint_handler;
        sigemptyset (&sigint_action.sa_mask);
        sigint_action.sa_flags = 0;
        sigaction(SIGINT, &sigint_action, NULL);
    }
    fprintf(stderr, "sampling: temp = %f, top_k = %d, top_p = %f, repeat_last_n = %i, repeat_penalty = %f\n",
        params.temp, params.top_k, params.top_p, params.repeat_last_n, params.repeat_penalty);
    fprintf(stderr, "generate: n_ctx = %d, n_batch = %d, n_predict = %d, n_keep = %d\n", params.n_ctx, params.n_batch, params.n_predict, params.n_keep);
    fprintf(stderr, "\n\n");

    // TODO: replace with ring-buffer
    std::vector<gptneox_token> last_n_tokens = std::vector<gptneox_token>();

    set_console_color(con_st, CONSOLE_COLOR_PROMPT);

    while (true) {
        is_interacting = true;
        int n_past = 0;
        set_console_color(con_st, CONSOLE_COLOR_USER_INPUT);

        if (params.instruct) {
            printf("\n<human>: ");
        }

        std::string buffer;
        if (!params.input_prefix.empty()) {
            buffer += params.input_prefix;
            printf("%s", buffer.c_str());
        }

        std::string line;
        bool another_line = true;
        do {
            if (!std::getline(std::cin, line)) {
                // input stream is bad or EOF received
                return 0;
            }
            if (line.empty() || line.back() != '\\') {
                another_line = false;
            } else {
                line.pop_back(); // Remove the continue character
            }
            buffer += line;
            if (another_line) {
                buffer += '\n';
            }
        } while (another_line);

        is_interacting = false;

        // done taking input, reset color
        set_console_color(con_st, CONSOLE_COLOR_DEFAULT);

        // Check for input
        if (buffer.length() <= 0) {
            continue; // Restart loop for input
        }

        auto prompt_embd = ::gptneox_tokenize(ctx, buffer, false);
        auto embd_inp = std::vector<gptneox_token>();

        embd_inp.push_back(gptneox_str_to_token(ctx, "<"));
        embd_inp.push_back(gptneox_str_to_token(ctx, "human"));
        embd_inp.push_back(gptneox_str_to_token(ctx, ">:"));

        embd_inp.insert(embd_inp.end(), prompt_embd.begin(), prompt_embd.end());

        embd_inp.push_back(gptneox_str_to_token(ctx, "\n"));
        embd_inp.push_back(gptneox_str_to_token(ctx, "<"));
        embd_inp.push_back(gptneox_str_to_token(ctx, "bot"));
        embd_inp.push_back(gptneox_str_to_token(ctx, ">:"));

        // How many tokens to generate - check if theres space in context for atleast one token (or batch size tokens?)
        int inp_size = embd_inp.size();
        auto space = params.n_ctx - inp_size;
        if(space <= 0) {
            fprintf(stderr, "%s : input too long\n", __func__);
            continue;
        }
        // Send batches to eval
        while (n_past < inp_size) {
            auto remaining = inp_size - n_past;
            int n_eval = params.n_batch < remaining ? params.n_batch : remaining;
            if (gptneox_eval(ctx, &embd_inp[n_past], n_eval, n_past, params.n_threads)) {
                fprintf(stderr, "<bot>: %s : failed to eval\n", __func__);
                return 1;
            }
            n_past += n_eval;
        }

        const int n_ctx = gptneox_n_ctx(ctx);
        const int n_vocab = gptneox_n_vocab(ctx);

        const float   temp            = params.temp;
        const int32_t top_k           = params.top_k <= 0 ? gptneox_n_vocab(ctx) : params.top_k;
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

        // Eval until space runs out
        auto out_count = 0;

        printf("<bot>:");
        while (space > 0) {
            // Get token
            gptneox_token id = 0;

            {
                auto logits = gptneox_get_logits(ctx);

                // Apply params.logit_bias map
                for (auto it = params.logit_bias.begin(); it != params.logit_bias.end(); it++) {
                    logits[it->first] += it->second;
                }

                std::vector<gptneox_token_data> candidates;
                candidates.reserve(n_vocab);
                for (gptneox_token token_id = 0; token_id < n_vocab; token_id++) {
                    candidates.emplace_back(gptneox_token_data{token_id, logits[token_id], 0.0f});
                }

                gptneox_token_data_array candidates_p = { candidates.data(), candidates.size(), false };

                // Apply penalties
                gptneox_token nl_token = gptneox_str_to_token(ctx, "\n");
                float nl_logit = logits[nl_token];
                auto last_n_repeat = std::min(std::min((int)last_n_tokens.size(), repeat_last_n), n_ctx);
                gptneox_sample_repetition_penalty(ctx, &candidates_p,
                    last_n_tokens.data() + last_n_tokens.size() - last_n_repeat,
                    last_n_repeat, repeat_penalty);
                gptneox_sample_frequency_and_presence_penalties(ctx, &candidates_p,
                    last_n_tokens.data() + last_n_tokens.size() - last_n_repeat,
                    last_n_repeat, alpha_frequency, alpha_presence);
                if (!penalize_nl) {
                    logits[nl_token] = nl_logit;
                }

                if (temp <= 0) {
                    // Greedy sampling
                    id = gptneox_sample_token_greedy(ctx, &candidates_p);
                } else {
                    if (mirostat == 1) {
                        static float mirostat_mu = 2.0f * mirostat_tau;
                        const int mirostat_m = 100;
                        gptneox_sample_temperature(ctx, &candidates_p, temp);
                        id = gptneox_sample_token_mirostat(ctx, &candidates_p, mirostat_tau, mirostat_eta, mirostat_m, &mirostat_mu);
                    } else if (mirostat == 2) {
                        static float mirostat_mu = 2.0f * mirostat_tau;
                        gptneox_sample_temperature(ctx, &candidates_p, temp);
                        id = gptneox_sample_token_mirostat_v2(ctx, &candidates_p, mirostat_tau, mirostat_eta, &mirostat_mu);
                    } else {
                        // Temperature sampling
                        gptneox_sample_top_k(ctx, &candidates_p, top_k, 1);
                        gptneox_sample_tail_free(ctx, &candidates_p, tfs_z, 1);
                        gptneox_sample_typical(ctx, &candidates_p, typical_p, 1);
                        gptneox_sample_top_p(ctx, &candidates_p, top_p, 1);
                        gptneox_sample_temperature(ctx, &candidates_p, temp);
                        id = gptneox_sample_token(ctx, &candidates_p);
                    }
                }
            }

            // Inc out count and dec space
            out_count += 1;
            space -= 1;
            // Repeat tokens update
            last_n_tokens.push_back(id);
            if ((int)last_n_tokens.size() > params.repeat_last_n) {
                last_n_tokens.erase(last_n_tokens.begin());
            }
            // Redpajama: check if the interactive is done.
            //std::cout<<" last_n_tokens.size: "<< last_n_tokens[0] <<" "<< last_n_tokens[1] <<" "<< last_n_tokens[2] << std::endl;
            if (last_n_tokens.size()==3 && last_n_tokens[0]==gptneox_str_to_token(ctx, "<")
            && last_n_tokens[1]==gptneox_str_to_token(ctx, "human") && last_n_tokens[2]==gptneox_str_to_token(ctx, ">:")){
                space = 0;
                continue;
            }

            // Check for eos - end early - check eos before bos in case they are the same
            if (id == gptneox_token_eos()) {
                space = 0;
                continue;
            }
            // Check for bos - skip callback if so
            if (id == gptneox_token_bos()) {
                continue;
            }

            if (last_n_tokens[2]==gptneox_str_to_token(ctx, "<")){
                ;
            }
            else if (last_n_tokens[2]==gptneox_str_to_token(ctx, "human")){
                if (last_n_tokens[1]==gptneox_str_to_token(ctx, "<")){
                    ;
                }
                else{
                    printf("%s", gptneox_token_to_str(ctx, id));
                }
            }
            else if (last_n_tokens[1]==gptneox_str_to_token(ctx, "<")){
                    printf("<");
                    printf("%s", gptneox_token_to_str(ctx, id));
                }
            else{
                printf("%s", gptneox_token_to_str(ctx, id));
            }
            fflush(stdout);
            // Check if we need to run another eval
            if (space > 0) {
                // Send generated token back into model for next generation
                if (gptneox_eval(ctx, &id, 1, n_past, params.n_threads)) {
                    fprintf(stderr, "%s : failed to eval\n", __func__);
                    return 1;
                }
                // Increment past count
                n_past += 1;
            }
            // Check for user interrupt
            if (is_interacting) { space = 0; }
        }
        printf("\n");
        fflush(stdout);
    }

    gptneox_print_timings(ctx);
    gptneox_free(ctx);
    set_console_color(con_st, CONSOLE_COLOR_DEFAULT);
    return 0;
}
