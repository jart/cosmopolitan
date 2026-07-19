# 🛡️ Cosmo Guardian

**The Autonomous, Zero-Dependency AI Agent for the Cosmopolitan Libc Ecosystem.**

Cosmopolitan Libc is an architectural marvel that allows C programs to run everywhere natively. But writing pure `no-std` code and managing massive hermetic `BUILD.mk` files can be challenging for both newcomers and seasoned maintainers.

Meet **Cosmo Guardian**: A pure Python, zero-dependency tool that analyzes, lint, and *automatically repairs* compilation and dependency errors in your Cosmopolitan repository.

---

## ⚡ Zero-Dependency Philosophy

Following Cosmopolitan's ethos of absolute portability, Cosmo Guardian requires **no pip installs**. 
It runs on pure Python Standard Library.
- No `requests`
- No heavy `openai` or `google-genai` SDKs
- Just pure `urllib` and `json`.

## 🛠️ Features

### 1. 🤖 The Autonomous Loop (`auto-build`)
Why debug makefiles when the machine can do it for you?
Run the auto-build command:
```bash
python cosmo-gardian/guardian.py auto-build tool/hello/
```
The agent will:
1. Run `make tool/hello/`
2. Intercept any compiler or linker errors (e.g., `undefined reference to LIBC_DUMMY`).
3. Diagnosticate the issue using the local Knowledge Base.
4. Generate a `.patch` file.
5. Apply the patch automatically.
6. Re-run `make` until it succeeds!

### 2. 🧠 Bring-Your-Own-Key AI Engine (`ai`)
Cosmo Guardian has a built-in RAG engine that searches live Github Issues and PRs to find solutions to obscure APE format bugs.
If you provide an `LLM_API_KEY`, it will use a pure-Python REST call to synthesize a perfect solution for you. It defaults to Gemini, but you can set `LLM_API_URL` to point to OpenAI, Anthropic, or a local LLaMA instance.
```bash
export LLM_API_KEY="your_key"
export LLM_API_URL="..." # Optional
python cosmo-gardian/guardian.py ai "Exec format error" "binfmt_misc"
```

### 3. 🔍 Cosmopolitan Linter (`lint`)
Catch non-compliant code *before* you compile. Cosmo Guardian scans C/C++/Rust files to ensure they respect the `no-std` philosophy.
```bash
python cosmo-gardian/guardian.py lint tool/hello/hello.c
# ❌ Found 1 issue(s) conflicting with Cosmopolitan philosophy:
#  - Line 12: System header <iostream> included.
#    Suggestion: Use Cosmopolitan's libc headers.
```

### 4. 🔗 CI/CD Github Actions Ready
Cosmo Guardian ships with a `.github/workflows/cosmo-guardian.yml` file. It will automatically protect your repository from circular dependencies and philosophy violations on every Pull Request.

---

## 🚀 Quick Start

1. Drop the `cosmo-gardian` folder into your Cosmopolitan repository root.
2. Run the master command:
```bash
python cosmo-gardian/guardian.py --help
```

## 🧠 The Knowledge Base

Cosmo Guardian is powered by a deterministic, lightning-fast JSON Knowledge Base (`knowledge/cosmopolitan.json`) that knows about:
- The `jtckdint` overflow fallback bug in `-std=c99`.
- The myth of `WinMain` vs standard `main()`.
- Windows `pledge/unveil` path restrictions.
- Stack alignment rules and APE `binfmt_misc` setups.

*Built to make Cosmopolitan development as magical as the executables it produces.*
