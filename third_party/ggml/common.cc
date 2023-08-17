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
#include "third_party/ggml/common.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/termios.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/termios.h"
#include "third_party/ggml/llama.h"
#include "third_party/ggml/llama_util.h"
#include "third_party/libcxx/algorithm"
#include "third_party/libcxx/cassert"
#include "third_party/libcxx/cstring"
#include "third_party/libcxx/fstream"
#include "third_party/libcxx/iterator"
#include "third_party/libcxx/sstream"
#include "third_party/libcxx/string"

__static_yoink("zipos");

asm(".ident\t\"\\n\\n\
llama.cpp (MIT License)\\n\
Copyright (c) 2023 Georgi Gerganov\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

static std::string replace_all(std::string const& original,
                               std::string const& before,
                               std::string const& after) {
    // https://stackoverflow.com/a/7724536/1653720
    std::string retval;
    std::string::const_iterator end = original.end();
    std::string::const_iterator current = original.begin();
    std::string::const_iterator next =
            std::search(current, end, before.begin(), before.end());
    while (next != end) {
        retval.append(current, next);
        retval.append(after);
        current = next + before.size();
        next = std::search(current, end, before.begin(), before.end());
    }
    retval.append(current, next);
    return retval;
}

static bool append_file_to_prompt(const char *path, gpt_params & params) {
    std::ifstream file(path);
    if (!file) {
        fprintf(stderr, "error: failed to open file '%s'\n", path);
        return false;
    }
    std::copy(std::istreambuf_iterator<char>(file),
              std::istreambuf_iterator<char>(),
              back_inserter(params.prompt));
    if (params.prompt.back() == '\n') {
        params.prompt.pop_back();
    }
    return true;
}

bool gpt_params_parse(int argc, char ** argv, gpt_params & params) {
    params.n_threads = std::min(20., (unsigned)__get_cpu_count() * .75);

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
        } else if (arg == "-v" || arg == "--verbose") {
            ++params.verbose;
        } else if (arg == "-q" || arg == "--quiet") {
            --params.verbose;
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
        } else if (arg == "-C" || arg == "--prompt_cache") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.prompt_path = argv[i];
        } else if (arg == "-f" || arg == "--file") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            if (!append_file_to_prompt(argv[i], params)) {
                invalid_param = true;
                break;
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
            params.n_keep_str = argv[i];
            if (is_integer_str(argv[i])) {
                params.n_keep = std::stoi(params.n_keep_str);
                if (!params.n_keep) {
                    params.n_keep_str = "";
                }
            }
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
        } else if (arg == "--multiline-input") {
            params.multiline_input = true;
        } else if (arg == "--color") {
            params.use_color = true;
        } else if (arg == "--mlock") {
            params.use_mlock = true;
        } else if (arg == "--gpu-layers" || arg == "-ngl" || arg == "--n-gpu-layers") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
#ifdef LLAMA_SUPPORTS_GPU_OFFLOAD
            params.n_gpu_layers = std::stoi(argv[i]);
#else
            fprintf(stderr, "warning: not compiled with GPU offload support, --n-gpu-layers option will be ignored\n");
            fprintf(stderr, "warning: see main README.md for information on enabling GPU BLAS support\n");
#endif
        } else if (arg == "--no-mmap") {
            params.use_mmap = false;
        } else if (arg == "--mtest") {
            params.mem_test = true;
        } else if (arg == "--verbose-prompt") {
            params.verbose_prompt = true;
        } else if (arg == "-r" || arg == "--stop" || arg == "--reverse-prompt") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.antiprompt.push_back(argv[i]);
        } else if (arg == "--perplexity") {
            params.perplexity = true;
        } else if (arg == "--ignore-eos") {
            params.logit_bias[llama_token_eos()] = -INFINITY;
        } else if (arg == "--no-penalize-nl") {
            params.penalize_nl = false;
        } else if (arg == "-l" || arg == "--logit-bias") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            std::stringstream ss(argv[i]);
            llama_token key = 0;
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
            gpt_print_usage(stdout, argc, argv, default_params);
            exit(0);
        } else if (arg == "--random-prompt") {
            params.random_prompt = true;
        } else if (arg == "--in-prefix") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.input_prefix = argv[i];
        } else if (arg == "--in-suffix") {
            if (++i >= argc) {
                invalid_param = true;
                break;
            }
            params.input_suffix = argv[i];
        } else {
            fprintf(stderr, "error: unknown argument: %s\n", arg.c_str());
            gpt_print_usage(stderr, argc, argv, default_params);
            exit(1);
        }
    }
    if (invalid_param) {
        fprintf(stderr, "error: invalid parameter for argument: %s\n", arg.c_str());
        gpt_print_usage(stderr, argc, argv, default_params);
        exit(1);
    }

    // if no prompt is specified, then use companion ai
    if (params.prompt.empty()) {
        if (params.verbose > 0) {
            fprintf(stderr, "%s: No prompt specified\n", __func__);
            fprintf(stderr, "%s: Loading CompanionAI\n", __func__);
        }
        if (fileexists("third_party/ggml/companionai.txt")) {
            append_file_to_prompt("third_party/ggml/companionai.txt", params);
        } else {
            append_file_to_prompt("/zip/companionai.txt", params);
        }
        const char *user;
        user = getenv("USER");
        if (!user || !*user) {
            user = "Cosmo";
        }
        params.prompt = replace_all(params.prompt, "USER_NAME", user);
        std::string user_prompt;
        user_prompt.append(user);
        user_prompt.append(":");
        params.logit_bias[llama_token_eos()] = -INFINITY;
        params.antiprompt.push_back(user_prompt);
        params.repeat_penalty = 1.17647;
        params.repeat_last_n = 256;
        params.interactive = true;
        params.n_predict = -1;
        params.n_ctx = 2048;
        params.n_keep = 0;
        params.n_keep_str = "\n\n\n";
        params.top_k = 40;
        params.top_p = .5;
        params.temp = 0.4;
    }

    return true;
}

void gpt_print_usage(FILE *f, int /*argc*/, char ** argv, const gpt_params & params) {
    fprintf(f, "usage: %s [options]\n", argv[0]);
    fprintf(f, "\n");
    fprintf(f, "options:\n");
    fprintf(f, "  -h, --help            show this help message and exit\n");
    fprintf(f, "  -v, --verbose         print helpful information to stderr [repeatable]\n");
    fprintf(f, "  -s, --silent          disables ephemeral progress indicators [repeatable]\n");
    fprintf(f, "  -i, --interactive     run in interactive mode\n");
    fprintf(f, "  --interactive-first   run in interactive mode and wait for input right away\n");
    fprintf(f, "  -ins, --instruct      run in instruction mode (use with Alpaca models)\n");
    fprintf(f, "  --multiline-input     allows you to write or paste multiple lines without ending each in '\\'\n");
    fprintf(f, "  -r PROMPT, --stop PROMPT, --reverse-prompt PROMPT\n");
    fprintf(f, "                        stop generating text when the specified text is encountered.\n");
    fprintf(f, "                        this option may be repeated.\n");
    fprintf(f, "  -s SEED, --seed SEED  RNG seed (default: -1, use random seed for < 0)\n");
    fprintf(f, "  -t N, --threads N     number of threads to use during computation (default: %d)\n", params.n_threads);
    fprintf(f, "  -p PROMPT, --prompt PROMPT\n");
    fprintf(f, "                        prompt to start generation with (default: Companion AI)\n");
    fprintf(f, "  --random-prompt       start with a randomized prompt.\n");
    fprintf(f, "  --in-prefix STRING    string to prefix user inputs with (default: empty)\n");
    fprintf(f, "  --in-suffix STRING    string to suffix after user inputs with (default: empty)\n");
    fprintf(f, "  -f FNAME, --file FNAME\n");
    fprintf(f, "                        text file containing prompt (default: Companion AI)\n");
    fprintf(f, "  -C FNAME, --prompt_cache FNAME\n");
    fprintf(f, "                        path of cache for fast prompt reload (default: .prompt.jtlp)\n");
    fprintf(f, "  -n N, --n_predict N   number of tokens to predict (default: %d, -1 = infinity)\n", params.n_predict);
    fprintf(f, "  --top_k N             top-k sampling (default: %d, 0 = disabled)\n", params.top_k);
    fprintf(f, "  --top_p N             top-p sampling (default: %.1f, 1.0 = disabled)\n", (double)params.top_p);
    fprintf(f, "  --tfs N               tail free sampling, parameter z (default: %.1f, 1.0 = disabled)\n", (double)params.tfs_z);
    fprintf(f, "  --typical N           locally typical sampling, parameter p (default: %.1f, 1.0 = disabled)\n", (double)params.typical_p);
    fprintf(f, "  --repeat_last_n N     last n tokens to consider for penalize (default: %d, 0 = disabled, -1 = ctx_size)\n", params.repeat_last_n);
    fprintf(f, "  --repeat_penalty N    penalize repeat sequence of tokens (default: %.1f, 1.0 = disabled)\n", (double)params.repeat_penalty);
    fprintf(f, "  --presence_penalty N  repeat alpha presence penalty (default: %.1f, 0.0 = disabled)\n", (double)params.presence_penalty);
    fprintf(f, "  --frequency_penalty N repeat alpha frequency penalty (default: %.1f, 0.0 = disabled)\n", (double)params.frequency_penalty);
    fprintf(f, "  --mirostat N          use Mirostat sampling.\n");
    fprintf(f, "                        Top K, Nucleus, Tail Free and Locally Typical samplers are ignored if used.\n");
    fprintf(f, "                        (default: %d, 0 = disabled, 1 = Mirostat, 2 = Mirostat 2.0)\n", params.mirostat);
    fprintf(f, "  --mirostat_lr N       Mirostat learning rate, parameter eta (default: %.1f)\n", (double)params.mirostat_eta);
    fprintf(f, "  --mirostat_ent N      Mirostat target entropy, parameter tau (default: %.1f)\n", (double)params.mirostat_tau);
    fprintf(f, "  -l TOKEN_ID(+/-)BIAS, --logit-bias TOKEN_ID(+/-)BIAS\n");
    fprintf(f, "                        modifies the likelihood of token appearing in the completion,\n");
    fprintf(f, "                        i.e. `--logit-bias 15043+1` to increase likelihood of token ' Hello',\n");
    fprintf(f, "                        or `--logit-bias 15043-1` to decrease likelihood of token ' Hello'\n");
    fprintf(f, "  --repeat_last_n N     last n tokens to consider for penalize (default: %d)\n", params.repeat_last_n);
    fprintf(f, "  --repeat_penalty N    penalize repeat sequence of tokens (default: %.1f)\n", (double)params.repeat_penalty);
    fprintf(f, "  -c N, --ctx_size N    size of the prompt context (default: %d)\n", params.n_ctx);
    fprintf(f, "  --ignore-eos          ignore end of stream token and continue generating (implies --logit-bias 2-inf)\n");
    fprintf(f, "  --no-penalize-nl      do not penalize newline token\n");
    fprintf(f, "  --memory_f32          use f32 instead of f16 for memory key+value\n");
    fprintf(f, "  --temp N              temperature (default: %.1f)\n", (double)params.temp);
    fprintf(f, "  --n_parts N           number of model parts (default: -1 = determine from dimensions)\n");
    fprintf(f, "  -b N, --batch_size N  batch size for prompt processing (default: %d)\n", params.n_batch);
    fprintf(f, "  --perplexity          compute perplexity over the prompt\n");
    fprintf(f, "  --keep NUM|STR        number of tokens to keep from the initial prompt, or substring\n");
    fprintf(f, "                        to search for within prompt that divides the actual prompt from\n");
    fprintf(f, "                        its initial example text (default: %d, -1 = all)\n", params.n_keep);
    if (llama_mlock_supported()) {
        fprintf(f, "  --mlock               force system to keep model in RAM rather than swapping or compressing\n");
    }
    if (llama_mmap_supported()) {
        fprintf(f, "  --no-mmap             do not memory-map model (slower load but may reduce pageouts if not using mlock)\n");
    }
#ifdef LLAMA_SUPPORTS_GPU_OFFLOAD
    fprintf(stderr, "  -ngl N, --n-gpu-layers N\n");
    fprintf(stderr, "                        number of layers to store in VRAM\n");
#endif
    fprintf(f, "  --mtest               compute maximum memory usage\n");
    fprintf(f, "  --verbose-prompt      print prompt before generation\n");
    fprintf(f, "  --lora FNAME          apply LoRA adapter (implies --no-mmap)\n");
    fprintf(f, "  --lora-base FNAME     optional model to use as a base for the layers modified by the LoRA adapter\n");
    fprintf(f, "  -m FNAME, --model FNAME\n");
    fprintf(f, "                        model path (default: %s)\n", params.model.c_str());
    fprintf(f, "\n");
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
std::vector<llama_token> llama_tokenize(struct llama_context * ctx, const std::string & text, bool add_bos) {
    // initialize to prompt numer of chars, since n_tokens <= n_prompt_chars
    std::vector<llama_token> res(text.size() + (int) add_bos);
    const int n = llama_tokenize(ctx, text.c_str(), res.data(), res.size(), add_bos);
    assert(n >= 0);
    res.resize(n);
    return res;
}

struct llama_context * llama_init_from_gpt_params(const gpt_params & params) {
    auto lparams = llama_context_default_params();

    lparams.n_ctx      = params.n_ctx;
    lparams.n_parts    = params.n_parts;
    lparams.seed       = params.seed;
    lparams.f16_kv     = params.memory_f16;
    lparams.use_mmap   = params.use_mmap;
    lparams.use_mlock  = params.use_mlock;
    lparams.logits_all = params.perplexity;
    lparams.embedding  = params.embedding;

    llama_context * lctx = llama_init_from_file(params.model.c_str(), lparams, params.verbose);

    if (lctx == NULL) {
        fprintf(stderr, "%s: error: failed to load model '%s'\n", __func__, params.model.c_str());
        return NULL;
    }

    if (!params.lora_adapter.empty()) {
        int err = llama_apply_lora_from_file(lctx,
                                             params.lora_adapter.c_str(),
                                             params.lora_base.empty() ? NULL : params.lora_base.c_str(),
                                             params.n_threads);
        if (err != 0) {
            fprintf(stderr, "%s: error: failed to apply lora adapter\n", __func__);
            return NULL;
        }
    }

    return lctx;
}

void console_init(console_state & con_st) {
#if defined(_WIN32)
    // Windows-specific console initialization
    DWORD dwMode = 0;
    con_st.hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (con_st.hConsole == INVALID_HANDLE_VALUE || !GetConsoleMode(con_st.hConsole, &dwMode)) {
        con_st.hConsole = GetStdHandle(STD_ERROR_HANDLE);
        if (con_st.hConsole != INVALID_HANDLE_VALUE && (!GetConsoleMode(con_st.hConsole, &dwMode))) {
            con_st.hConsole = NULL;
        }
    }
    if (con_st.hConsole) {
        // Enable ANSI colors on Windows 10+
        if (con_st.use_color && !(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            SetConsoleMode(con_st.hConsole, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
        // Set console output codepage to UTF8
        SetConsoleOutputCP(CP_UTF8);
    }
    HANDLE hConIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hConIn != INVALID_HANDLE_VALUE && GetConsoleMode(hConIn, &dwMode)) {
        // Set console input codepage to UTF16
        _setmode(_fileno(stdin), _O_WTEXT);

        // Turn off ICANON (ENABLE_LINE_INPUT) and ECHO (ENABLE_ECHO_INPUT)
        dwMode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
        SetConsoleMode(hConIn, dwMode);
    }
#else
    // POSIX-specific console initialization
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &con_st.prev_state);
    new_termios = con_st.prev_state;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    con_st.tty = fopen("/dev/tty", "w+");
    if (con_st.tty != nullptr) {
        setvbuf(con_st.tty, NULL, _IONBF, 0);
        con_st.out = con_st.tty;
    }

    setlocale(LC_ALL, "");
#endif
}

void console_cleanup(console_state & con_st) {
    // Reset console color
    console_set_color(con_st, CONSOLE_COLOR_DEFAULT);

#if !defined(_WIN32)
    if (con_st.tty != nullptr) {
        con_st.out = stdout;
        fclose(con_st.tty);
        con_st.tty = nullptr;
    }
    // Restore the terminal settings on POSIX systems
    tcsetattr(STDIN_FILENO, TCSANOW, &con_st.prev_state);
#endif
}

/* Keep track of current color of output, and emit ANSI code if it changes. */
void console_set_color(console_state & con_st, console_color_t color) {
    if (con_st.use_color && con_st.color != color) {
        fflush(stdout);
        switch(color) {
            case CONSOLE_COLOR_DEFAULT:
                fprintf(con_st.out, ANSI_COLOR_RESET);
                break;
            case CONSOLE_COLOR_PROMPT:
                fprintf(con_st.out, ANSI_COLOR_YELLOW);
                break;
            case CONSOLE_COLOR_USER_INPUT:
                fprintf(con_st.out, ANSI_BOLD ANSI_COLOR_GREEN);
                break;
        }
        con_st.color = color;
        fflush(con_st.out);
    }
}

char32_t getchar32() {
    wchar_t wc = getwchar();
    if (static_cast<wint_t>(wc) == WEOF) {
        return WEOF;
    }

#if WCHAR_MAX == 0xFFFF
    if ((wc >= 0xD800) && (wc <= 0xDBFF)) { // Check if wc is a high surrogate
        wchar_t low_surrogate = getwchar();
        if ((low_surrogate >= 0xDC00) && (low_surrogate <= 0xDFFF)) { // Check if the next wchar is a low surrogate
            return (static_cast<char32_t>(wc & 0x03FF) << 10) + (low_surrogate & 0x03FF) + 0x10000;
        }
    }
    if ((wc >= 0xD800) && (wc <= 0xDFFF)) { // Invalid surrogate pair
        return 0xFFFD; // Return the replacement character U+FFFD
    }
#endif

    return static_cast<char32_t>(wc);
}

void pop_cursor(console_state & con_st) {
#if defined(_WIN32)
    if (con_st.hConsole != NULL) {
        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        GetConsoleScreenBufferInfo(con_st.hConsole, &bufferInfo);

        COORD newCursorPosition = bufferInfo.dwCursorPosition;
        if (newCursorPosition.X == 0) {
            newCursorPosition.X = bufferInfo.dwSize.X - 1;
            newCursorPosition.Y -= 1;
        } else {
            newCursorPosition.X -= 1;
        }

        SetConsoleCursorPosition(con_st.hConsole, newCursorPosition);
        return;
    }
#endif
    putc('\b', con_st.out);
}

int estimateWidth(char32_t codepoint) {
#if defined(_WIN32)
    return 1;
#else
    return wcwidth(codepoint);
#endif
}

int put_codepoint(console_state & con_st, const char* utf8_codepoint, size_t length, int expectedWidth) {
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    if (!GetConsoleScreenBufferInfo(con_st.hConsole, &bufferInfo)) {
        // go with the default
        return expectedWidth;
    }
    COORD initialPosition = bufferInfo.dwCursorPosition;
    DWORD nNumberOfChars = length;
    WriteConsole(con_st.hConsole, utf8_codepoint, nNumberOfChars, &nNumberOfChars, NULL);

    CONSOLE_SCREEN_BUFFER_INFO newBufferInfo;
    GetConsoleScreenBufferInfo(con_st.hConsole, &newBufferInfo);

    // Figure out our real position if we're in the last column
    if (utf8_codepoint[0] != 0x09 && initialPosition.X == newBufferInfo.dwSize.X - 1) {
        DWORD nNumberOfChars;
        WriteConsole(con_st.hConsole, &" \b", 2, &nNumberOfChars, NULL);
        GetConsoleScreenBufferInfo(con_st.hConsole, &newBufferInfo);
    }

    int width = newBufferInfo.dwCursorPosition.X - initialPosition.X;
    if (width < 0) {
        width += newBufferInfo.dwSize.X;
    }
    return width;
#else
    // we can trust expectedWidth if we've got one
    if (expectedWidth >= 0 || con_st.tty == nullptr) {
        fwrite(utf8_codepoint, length, 1, con_st.out);
        return expectedWidth;
    }

    fputs("\033[6n", con_st.tty); // Query cursor position
    int x1, x2, y1, y2;
    int results = 0;
    results = fscanf(con_st.tty, "\033[%d;%dR", &y1, &x1);

    fwrite(utf8_codepoint, length, 1, con_st.tty);

    fputs("\033[6n", con_st.tty); // Query cursor position
    results += fscanf(con_st.tty, "\033[%d;%dR", &y2, &x2);

    if (results != 4) {
        return expectedWidth;
    }

    int width = x2 - x1;
    if (width < 0) {
        // Calculate the width considering text wrapping
        struct winsize w;
        tcgetwinsize(STDOUT_FILENO, &w);
        width += w.ws_col;
    }
    return width;
#endif
}

void replace_last(console_state & con_st, char ch) {
#if defined(_WIN32)
    pop_cursor(con_st);
    put_codepoint(con_st, &ch, 1, 1);
#else
    fprintf(con_st.out, "\b%c", ch);
#endif
}

void append_utf8(char32_t ch, std::string & out) {
    if (ch <= 0x7F) {
        out.push_back(static_cast<unsigned char>(ch));
    } else if (ch <= 0x7FF) {
        out.push_back(static_cast<unsigned char>(0xC0 | ((ch >> 6) & 0x1F)));
        out.push_back(static_cast<unsigned char>(0x80 | (ch & 0x3F)));
    } else if (ch <= 0xFFFF) {
        out.push_back(static_cast<unsigned char>(0xE0 | ((ch >> 12) & 0x0F)));
        out.push_back(static_cast<unsigned char>(0x80 | ((ch >> 6) & 0x3F)));
        out.push_back(static_cast<unsigned char>(0x80 | (ch & 0x3F)));
    } else if (ch <= 0x10FFFF) {
        out.push_back(static_cast<unsigned char>(0xF0 | ((ch >> 18) & 0x07)));
        out.push_back(static_cast<unsigned char>(0x80 | ((ch >> 12) & 0x3F)));
        out.push_back(static_cast<unsigned char>(0x80 | ((ch >> 6) & 0x3F)));
        out.push_back(static_cast<unsigned char>(0x80 | (ch & 0x3F)));
    } else {
        // Invalid Unicode code point
    }
}

// Helper function to remove the last UTF-8 character from a string
void pop_back_utf8_char(std::string & line) {
    if (line.empty()) {
        return;
    }

    size_t pos = line.length() - 1;

    // Find the start of the last UTF-8 character (checking up to 4 bytes back)
    for (size_t i = 0; i < 3 && pos > 0; ++i, --pos) {
        if ((line[pos] & 0xC0) != 0x80) break; // Found the start of the character
    }
    line.erase(pos);
}

bool console_readline(console_state & con_st, std::string & line) {
    console_set_color(con_st, CONSOLE_COLOR_USER_INPUT);
    if (con_st.out != stdout) {
        fflush(stdout);
    }

    line.clear();
    std::vector<int> widths;
    bool is_special_char = false;
    bool end_of_stream = false;

    char32_t input_char;
    while (true) {
        fflush(con_st.out); // Ensure all output is displayed before waiting for input
        input_char = getchar32();

        if (input_char == '\r' || input_char == '\n') {
            break;
        }

        if (input_char == WEOF || input_char == 0x04 /* Ctrl+D*/) {
            end_of_stream = true;
            break;
        }

        if (is_special_char) {
            console_set_color(con_st, CONSOLE_COLOR_USER_INPUT);
            replace_last(con_st, line.back());
            is_special_char = false;
        }

        if (input_char == '\033') { // Escape sequence
            char32_t code = getchar32();
            if (code == '[' || code == 0x1B) {
                // Discard the rest of the escape sequence
                while ((code = getchar32()) != WEOF) {
                    if ((code >= 'A' && code <= 'Z') || (code >= 'a' && code <= 'z') || code == '~') {
                        break;
                    }
                }
            }
        } else if (input_char == 0x08 || input_char == 0x7F) { // Backspace
            if (!widths.empty()) {
                int count;
                do {
                    count = widths.back();
                    widths.pop_back();
                    // Move cursor back, print space, and move cursor back again
                    for (int i = 0; i < count; i++) {
                        replace_last(con_st, ' ');
                        pop_cursor(con_st);
                    }
                    pop_back_utf8_char(line);
                } while (count == 0 && !widths.empty());
            }
        } else {
            int offset = line.length();
            append_utf8(input_char, line);
            int width = put_codepoint(con_st, line.c_str() + offset, line.length() - offset, estimateWidth(input_char));
            if (width < 0) {
                width = 0;
            }
            widths.push_back(width);
        }

        if (!line.empty() && (line.back() == '\\' || line.back() == '/')) {
            console_set_color(con_st, CONSOLE_COLOR_PROMPT);
            replace_last(con_st, line.back());
            is_special_char = true;
        }
    }

    bool has_more = con_st.multiline_input;
    if (is_special_char) {
        replace_last(con_st, ' ');
        pop_cursor(con_st);

        char last = line.back();
        line.pop_back();
        if (last == '\\') {
            line += '\n';
            fputc('\n', con_st.out);
            has_more = !has_more;
        } else {
            // llama will just eat the single space, it won't act as a space
            if (line.length() == 1 && line.back() == ' ') {
                line.clear();
                pop_cursor(con_st);
            }
            has_more = false;
        }
    } else {
        if (end_of_stream) {
            has_more = false;
        } else {
            line += '\n';
            fputc('\n', con_st.out);
        }
    }

    fflush(con_st.out);
    return has_more;
}
