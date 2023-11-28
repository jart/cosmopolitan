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
#include "third_party/radpajama/common-gptneox.h"
#include "third_party/ggml/llama_util.h"
#include "third_party/libcxx/algorithm"
#include "third_party/libcxx/cassert"
#include "third_party/libcxx/cstring"
#include "third_party/libcxx/fstream"
#include "third_party/libcxx/iostream"
#include "third_party/libcxx/iterator"
#include "third_party/libcxx/sstream"
#include "third_party/libcxx/string"

bool gpt_params_parse(int argc, char ** argv, gpt_params & params) {
    params.n_threads = std::min(20., (unsigned)__get_cpu_count() * 0.75);

    bool invalid_param = false;
    std::string arg;
    gpt_params default_params;

    for (int i = 1; i < argc; i++) {
        arg = argv[i];

        if (arg == "-s" || arg == "--seed") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.seed = std::stoi(argv[i]);
        } else if (arg == "-t" || arg == "--threads") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.n_threads = std::stoi(argv[i]);
        } else if (arg == "-p" || arg == "--prompt") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.prompt = argv[i];
        } else if (arg == "--session") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.path_session = argv[i];
        } else if (arg == "-f" || arg == "--file") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            std::ifstream file(argv[i]);
            if (!file) {
                fprintf(stderr, "error: failed to open file '%s'\n", argv[i]);
                invalid_param = true;
                break;
            }
            std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), back_inserter(params.prompt));
            if (params.prompt.back() == '\n') {
                params.prompt.pop_back();
            }
        } else if (arg == "-n" || arg == "--n_predict") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.n_predict = std::stoi(argv[i]);
        } else if (arg == "--top_k") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.top_k = std::stoi(argv[i]);
        } else if (arg == "-c" || arg == "--ctx_size") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.n_ctx = std::stoi(argv[i]);
        } else if (arg == "--memory_f32") {
            params.memory_f16 = false;
        } else if (arg == "--top_p") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.top_p = std::stof(argv[i]);
        } else if (arg == "--temp") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.temp = std::stof(argv[i]);
        } else if (arg == "--tfs") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.tfs_z = std::stof(argv[i]);
        } else if (arg == "--typical") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.typical_p = std::stof(argv[i]);
        } else if (arg == "--repeat_last_n") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.repeat_last_n = std::stoi(argv[i]);
        } else if (arg == "--repeat_penalty") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.repeat_penalty = std::stof(argv[i]);
        } else if (arg == "--frequency_penalty") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.frequency_penalty = std::stof(argv[i]);
        } else if (arg == "--presence_penalty") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.presence_penalty = std::stof(argv[i]);
        } else if (arg == "--mirostat") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.mirostat = std::stoi(argv[i]);
        } else if (arg == "--mirostat_lr") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.mirostat_eta = std::stof(argv[i]);
        } else if (arg == "--mirostat_ent") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.mirostat_tau = std::stof(argv[i]);
        } else if (arg == "-b" || arg == "--batch_size") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.n_batch = std::stoi(argv[i]);
            params.n_batch = std::min(512, params.n_batch);
        } else if (arg == "--keep") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.n_keep = std::stoi(argv[i]);
        } else if (arg == "-m" || arg == "--model") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.model = argv[i];
        } else if (arg == "--lora") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.lora_adapter = argv[i];
            params.use_mmap = false;
        } else if (arg == "--lora-base") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.lora_base = argv[i];
        } else if (arg == "-i" || arg == "--interactive") {
            params.interactive = true;
        } else if (arg == "--embedding") {
            params.embedding = true;
        } else if (arg == "--interactive-first") {
            params.interactive_first = true;
        } else if (arg == "-ins" || arg == "--instruct") {
            params.instruct = true;
        } else if (arg == "--color") {
            params.use_color = true;
        } else if (arg == "--mlock") {
            params.use_mlock = true;
        } else if (arg == "--no-mmap") {
            params.use_mmap = false;
        } else if (arg == "--mtest") {
            params.mem_test = true;
        } else if (arg == "--verbose-prompt") {
            params.verbose_prompt = true;
        } else if (arg == "-r" || arg == "--reverse-prompt") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.antiprompt.push_back(argv[i]);
        } else if (arg == "--perplexity") {
            params.perplexity = true;
        } else if (arg == "--ignore-eos") {
            params.logit_bias[gptneox_token_eos()] = -INFINITY;
        } else if (arg == "--no-penalize-nl") {
            params.penalize_nl = false;
        } else if (arg == "-l" || arg == "--logit-bias") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            std::stringstream ss(argv[i]);
            gptneox_token key = 0;
            char sign = 0;
            std::string value_str;
            if (ss >> key && ss >> sign && std::getline(ss, value_str) && (sign == '+' || sign == '-')) {
                params.logit_bias[key] = std::stof(value_str) * ((sign == '-') ? -1.0f : 1.0f);
            } else {
                invalid_param = true;
                break;
            }
        } else if (arg == "--n_parts") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.n_parts = std::stoi(argv[i]);
        } else if (arg == "-h" || arg == "--help") {
            gpt_print_usage(argc, argv, default_params);
            exit(0);
        } else if (arg == "--random-prompt") {
            params.random_prompt = true;
        } else if (arg == "--in-prefix") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.input_prefix = argv[i];
        } else {
            fprintf(stderr, "error: unknown argument: %s\n", arg.c_str());
            gpt_print_usage(argc, argv, default_params);
            exit(1);
        }
    }
    if (invalid_param) {
        fprintf(stderr, "error: invalid parameter for argument: %s\n", arg.c_str());
        gpt_print_usage(argc, argv, default_params);
        exit(1);
    }

    return true;
}

void gpt_print_usage(int /*argc*/, char ** argv, const gpt_params & params) {
    fprintf(stderr, "usage: %s [options]\n", argv[0]);
    fprintf(stderr, "\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "  -h, --help            show this help message and exit\n");
    fprintf(stderr, "  -i, --interactive     run in interactive mode\n");
    fprintf(stderr, "  --interactive-first   run in interactive mode and wait for input right away\n");
    fprintf(stderr, "  -ins, --instruct      run in instruction mode\n");
    fprintf(stderr, "  -r PROMPT, --reverse-prompt PROMPT\n");
    fprintf(stderr, "                        run in interactive mode and poll user input upon seeing PROMPT (can be\n");
    fprintf(stderr, "                        specified more than once for multiple prompts).\n");
    fprintf(stderr, "  --color               colorise output to distinguish prompt and user input from generations\n");
    fprintf(stderr, "  -s SEED, --seed SEED  RNG seed (default: -1, use random seed for <= 0)\n");
    fprintf(stderr, "  -t N, --threads N     number of threads to use during computation (default: %d)\n", params.n_threads);
    fprintf(stderr, "  -p PROMPT, --prompt PROMPT\n");
    fprintf(stderr, "                        prompt to start generation with (default: empty)\n");
    fprintf(stderr, "  --session FNAME       file to cache model state in (may be large!) (default: none)\n");
    fprintf(stderr, "  --random-prompt       start with a randomized prompt.\n");
    fprintf(stderr, "  --in-prefix STRING    string to prefix user inputs with (default: empty)\n");
    fprintf(stderr, "  -f FNAME, --file FNAME\n");
    fprintf(stderr, "                        prompt file to start generation.\n");
    fprintf(stderr, "  -n N, --n_predict N   number of tokens to predict (default: %d, -1 = infinity)\n", params.n_predict);
    fprintf(stderr, "  --top_k N             top-k sampling (default: %d, 0 = disabled)\n", params.top_k);
    fprintf(stderr, "  --top_p N             top-p sampling (default: %.1f, 1.0 = disabled)\n", (double)params.top_p);
    fprintf(stderr, "  --tfs N               tail free sampling, parameter z (default: %.1f, 1.0 = disabled)\n", (double)params.tfs_z);
    fprintf(stderr, "  --typical N           locally typical sampling, parameter p (default: %.1f, 1.0 = disabled)\n", (double)params.typical_p);
    fprintf(stderr, "  --repeat_last_n N     last n tokens to consider for penalize (default: %d, 0 = disabled, -1 = ctx_size)\n", params.repeat_last_n);
    fprintf(stderr, "  --repeat_penalty N    penalize repeat sequence of tokens (default: %.1f, 1.0 = disabled)\n", (double)params.repeat_penalty);
    fprintf(stderr, "  --presence_penalty N  repeat alpha presence penalty (default: %.1f, 0.0 = disabled)\n", (double)params.presence_penalty);
    fprintf(stderr, "  --frequency_penalty N repeat alpha frequency penalty (default: %.1f, 0.0 = disabled)\n", (double)params.frequency_penalty);
    fprintf(stderr, "  --mirostat N          use Mirostat sampling.\n");
    fprintf(stderr, "                        Top K, Nucleus, Tail Free and Locally Typical samplers are ignored if used.\n");
    fprintf(stderr, "                        (default: %d, 0 = disabled, 1 = Mirostat, 2 = Mirostat 2.0)\n", params.mirostat);
    fprintf(stderr, "  --mirostat_lr N       Mirostat learning rate, parameter eta (default: %.1f)\n", (double)params.mirostat_eta);
    fprintf(stderr, "  --mirostat_ent N      Mirostat target entropy, parameter tau (default: %.1f)\n", (double)params.mirostat_tau);
    fprintf(stderr, "  -l TOKEN_ID(+/-)BIAS, --logit-bias TOKEN_ID(+/-)BIAS\n");
    fprintf(stderr, "                        modifies the likelihood of token appearing in the completion,\n");
    fprintf(stderr, "                        i.e. `--logit-bias 15043+1` to increase likelihood of token ' Hello',\n");
    fprintf(stderr, "                        or `--logit-bias 15043-1` to decrease likelihood of token ' Hello'\n");
    fprintf(stderr, "  -c N, --ctx_size N    size of the prompt context (default: %d)\n", params.n_ctx);
    fprintf(stderr, "  --ignore-eos          ignore end of stream token and continue generating (implies --logit-bias 2-inf)\n");
    fprintf(stderr, "  --no-penalize-nl      do not penalize newline token\n");
    fprintf(stderr, "  --memory_f32          use f32 instead of f16 for memory key+value\n");
    fprintf(stderr, "  --temp N              temperature (default: %.1f)\n", (double)params.temp);
    fprintf(stderr, "  --n_parts N           number of model parts (default: -1 = determine from dimensions)\n");
    fprintf(stderr, "  -b N, --batch_size N  batch size for prompt processing (default: %d)\n", params.n_batch);
    fprintf(stderr, "  --perplexity          compute perplexity over the prompt\n");
    fprintf(stderr, "  --keep                number of tokens to keep from the initial prompt (default: %d, -1 = all)\n", params.n_keep);
    if (gptneox_mlock_supported()) {
        fprintf(stderr, "  --mlock               force system to keep model in RAM rather than swapping or compressing\n");
    }
    if (gptneox_mmap_supported()) {
        fprintf(stderr, "  --no-mmap             do not memory-map model (slower load but may reduce pageouts if not using mlock)\n");
    }
    fprintf(stderr, "  --mtest               compute maximum memory usage\n");
    fprintf(stderr, "  --verbose-prompt      print prompt before generation\n");
    fprintf(stderr, "  --lora FNAME          apply LoRA adapter (implies --no-mmap)\n");
    fprintf(stderr, "  --lora-base FNAME     optional model to use as a base for the layers modified by the LoRA adapter\n");
    fprintf(stderr, "  -m FNAME, --model FNAME\n");
    fprintf(stderr, "                        model path (default: %s)\n", params.model.c_str());
    fprintf(stderr, "\n");
}

std::string gpt_random_prompt(std::mt19937 & rng) {
    const int r = rng() % 10;
    switch (r) {
        case 0: return "So";
        case 1: return "Once upon a time";
        case 2: return "When";
        case 3: return "The";
        case 4: return "After";
        case 5: return "If";
        case 6: return "import";
        case 7: return "He";
        case 8: return "She";
        case 9: return "They";
        default: return "To";
    }

    return "The";
}

// TODO: not great allocating this every time
std::vector<gptneox_token> gptneox_tokenize(struct gptneox_context * ctx, const std::string & text, bool add_bos) {
    // initialize to prompt numer of chars, since n_tokens <= n_prompt_chars
    std::vector<gptneox_token> res(text.size() + (int)add_bos);
    int n = gptneox_tokenize(ctx, text.c_str(), res.data(), res.size(), add_bos);
    assert(n >= 0);
    res.resize(n);

    return res;
}

/* Keep track of current color of output, and emit ANSI code if it changes. */
void set_console_color(console_state & con_st, console_color_t color) {
    if (con_st.use_color && con_st.color != color) {
        switch(color) {
            case CONSOLE_COLOR_DEFAULT:
                printf(ANSI_COLOR_RESET);
                break;
            case CONSOLE_COLOR_PROMPT:
                printf(ANSI_COLOR_YELLOW);
                break;
            case CONSOLE_COLOR_USER_INPUT:
                printf(ANSI_BOLD ANSI_COLOR_GREEN);
                break;
        }
        con_st.color = color;
    }
}
