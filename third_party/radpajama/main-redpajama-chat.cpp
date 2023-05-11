// Defines sigaction on msys:
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "common-gptneox.h"
#include "gptneox.h"

#include <cassert>
#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <signal.h>
#include <unistd.h>
#elif defined (_WIN32)
#include <signal.h>
#endif

static console_state con_st;
static gptneox_context ** g_ctx;

static bool is_interacting = false;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)) || defined (_WIN32)
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
#endif

int main(int argc, char ** argv) {
    gpt_params params;
    params.model = "./examples/redpajama/models/pythia/ggml-RedPajama-INCITE-Chat-3B-v1-f16.bin";
    
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
    
    fprintf(stderr, "%s: seed = %d\n", __func__, params.seed);
    
    std::mt19937 rng(params.seed);
    if (params.random_prompt) {
        params.prompt = gpt_random_prompt(rng);
    }
    
    gptneox_context * ctx;
    g_ctx = &ctx;
    
    // load the model
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
    
    // print system information
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "system_info: n_threads = %d / %d | %s\n",
                params.n_threads, std::thread::hardware_concurrency(), gptneox_print_system_info());
    }
    
    // determine the maximum memory usage needed to do inference for the given n_batch and n_predict parameters
    if (params.mem_test) {
        {
            const std::vector<gptneox_token> tmp(params.n_batch, 0);
            gptneox_eval(ctx, tmp.data(), tmp.size(), 0, params.n_threads);
        }
        
        {
            const std::vector<gptneox_token> tmp = { 0, };
            gptneox_eval(ctx, tmp.data(), tmp.size(), params.n_predict - 1, params.n_threads);
        }
        
        gptneox_print_timings(ctx);
        gptneox_free(ctx);
        
        return 0;
    }

    // Always interactive for RedPajama chat model
    params.interactive = true;
    
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
    }
    fprintf(stderr, "sampling: temp = %f, top_k = %d, top_p = %f, repeat_last_n = %i, repeat_penalty = %f\n",
        params.temp, params.top_k, params.top_p, params.repeat_last_n, params.repeat_penalty);
    fprintf(stderr, "generate: n_ctx = %d, n_batch = %d, n_predict = %d, n_keep = %d\n", params.n_ctx, params.n_batch, params.n_predict, params.n_keep);
    fprintf(stderr, "\n\n");
    
    // TODO: replace with ring-buffer
    std::vector<gptneox_token> last_n_tokens = std::vector<gptneox_token>();
    //std::fill(last_n_tokens.begin(), last_n_tokens.end(), 0);
    
    set_console_color(con_st, CONSOLE_COLOR_PROMPT);
    
    if (params.interactive) {
        printf("== Running in interactive mode. ==\n"
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)) || defined (_WIN32)
               " - Press Ctrl+C to interject at any time.\n"
#endif
               " - Press Return to return control to RedPajama.\n"
               " - If you want to submit another line, end your input in '\\'.\n\n");
    }
    
    const int32_t top_k          = params.top_k;
    const float   top_p          = params.top_p;
    const float   temp           = params.temp;
    const float   repeat_penalty = params.repeat_penalty;
    
    // Chat loop
    while (true) {
        is_interacting = true;
        
        int n_past = 0;
        
        // Get input
        
        // potentially set color to indicate we are taking user input
        set_console_color(con_st, CONSOLE_COLOR_USER_INPUT);
        
#if defined (_WIN32)
        // Windows: must reactivate sigint handler after each signal
        signal(SIGINT, sigint_handler);
#endif

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
        
        // Tokenize prompt with RedPajama special tokens

        auto prompt_embd = ::gptneox_tokenize(ctx, buffer, false);
        auto embd_inp = std::vector<gptneox_token>();

        // Redpajama: insert special tokens for OA. (prefix)
        embd_inp.push_back(gptneox_str_to_token(ctx, "<"));
        embd_inp.push_back(gptneox_str_to_token(ctx, "human"));
        embd_inp.push_back(gptneox_str_to_token(ctx, ">:"));
        
        embd_inp.insert(embd_inp.end(), prompt_embd.begin(), prompt_embd.end());

        // Redpajama: insert special tokens for OA. (postfix)
        embd_inp.push_back(gptneox_str_to_token(ctx, "\n"));
        embd_inp.push_back(gptneox_str_to_token(ctx, "<"));
        embd_inp.push_back(gptneox_str_to_token(ctx, "bot"));
        embd_inp.push_back(gptneox_str_to_token(ctx, ">:"));
       
        
        // Verbose prompt
        if (params.verbose_prompt) {
            fprintf(stderr, "\n");
            fprintf(stderr, "%s: prompt: '%s'\n", __func__, buffer.c_str());
            fprintf(stderr, "%s: number of tokens in prompt = %zu\n", __func__, embd_inp.size());
            for (int i = 0; i < (int) embd_inp.size(); i++) {
                fprintf(stderr, "%6d -> '%s'\n", embd_inp[i], gptneox_token_to_str(ctx, embd_inp[i]));
            }
            fprintf(stderr, "\n");
        }
        
        // How many tokens to generate - check if theres space in context for atleast one token (or batch size tokens?)
        auto inp_size = embd_inp.size();
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
            if (last_n_tokens.size() > params.repeat_last_n) {
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
            // Convert token to string and display
            // printf("%s(%d)", gptneox_token_to_str(ctx, id), id);
            
            
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
        //printf("\n %d", space);
        fflush(stdout);
    }
    
#if defined (_WIN32)
    signal(SIGINT, SIG_DFL);
#endif

    gptneox_print_timings(ctx);
    gptneox_free(ctx);

    set_console_color(con_st, CONSOLE_COLOR_DEFAULT);

    return 0;
}
    
    
    
    
    


