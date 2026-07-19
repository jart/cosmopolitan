import sys
import os

# Add the current directory to sys.path to allow importing modules
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from parsers.build_mk import DependencyAnalyzer
from parsers.compiler import CompilerWrapper
from engines.diagnostic import DiagnosticEngine
from engines.math_engine import MathEngine
from engines.patch_engine import PatchEngine
from engines.ai_engine import AIEngine
from engines.autonomous import AutoBuilder
from cosmo_agent import CosmoExpertModel
from engines.ai_engine import AIEngine

def main():
    if len(sys.argv) < 2:
        print("Cosmo Sentinel v1.0")
        print("Usage: python guardian.py [command] [args...]")
        print("\nCommands:")
        print("  build [target]   Run 'make [target]' and intercept errors")
        print("  analyze          Run static dependency analysis on the repository")
        print("  math [cmd]       Run the math engine (e.g. 'math add 2000000000 2000000000 int32')")
        print("  patch [args]     Run the patch engine (e.g. 'patch tool/hello/BUILD.mk TOOL_HELLO LIBC_CALLS')")
        print("  patch [args]     Run the patch engine (e.g. 'patch tool/hello/BUILD.mk TOOL_HELLO LIBC_CALLS')")
        print("  ai [error] [kw]  Ask the AI Engine (e.g. 'ai \"Perl C99 math error\" \"jtckdint overflow\"')")
        print("  auto-build [tgt] Run the autonomous build loop")
        print("  lint [filepath]  Validate C/C++/Rust code against Cosmopolitan philosophy")
        sys.exit(1)

    cmd = sys.argv[1]
    
    # We assume guardian.py is inside cosmo-gardian/ which is inside cosmopolitan/
    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))

    if cmd == 'build':
        target = sys.argv[2] if len(sys.argv) > 2 else None
        kb_path = os.path.join(os.path.dirname(__file__), 'knowledge', 'cosmopolitan.json')
        diag_engine = DiagnosticEngine(kb_path)
        compiler = CompilerWrapper(diag_engine)
        compiler.run_make(target, repo_root)

    elif cmd == 'analyze':
        print(f"[Cosmo Sentinel] Analyzing repository at {repo_root}...")
        analyzer = DependencyAnalyzer()
        for root, _, files in os.walk(repo_root):
            if 'BUILD.mk' in files:
                analyzer.parse_build_mk(os.path.join(root, 'BUILD.mk'))
                
        cycles = analyzer.find_cycles()
        if cycles:
            print("\n[ERROR] Found Circular Dependencies:")
            for cycle in cycles:
                print(" -> ".join(cycle))
        else:
            print("\n[OK] No circular dependencies found.")
            
        print("\n[Density Check]")
        for pkg, files in analyzer.pkg_files.items():
            if len(files) > 40:
                print(f"[WARNING] Package {pkg} is very dense ({len(files)} sources).")

        print("\n[Source Check]")
        missing = analyzer.verify_sources_exist(repo_root)
        if missing:
            print("[ERROR] Found Missing Source Files listed in BUILD.mk:")
            for pkg, f in missing:
                print(f"  - {f} (in package {pkg})")
        else:
            print("[OK] All source files listed in BUILD.mk exist.")

    elif cmd == 'math':
        math_engine = MathEngine()
        if len(sys.argv) < 3:
            print("Usage for math:")
            print("  math add <a> <b> [dtype]")
            print("  math align <size> [alignment]")
            sys.exit(1)
        subcmd = sys.argv[2]
        if subcmd == 'add':
            a = int(sys.argv[3])
            b = int(sys.argv[4])
            dtype = sys.argv[5] if len(sys.argv) > 5 else 'int32'
            res = math_engine.check_addition(a, b, dtype)
            print(f"\n[Math Engine] Addition Analysis:")
            if 'error' in res:
                print(res['error'])
            else:
                print(f"Operation: {res['operation']}")
                print(f"Real Sum: {res['real_sum']}")
                print(f"C Interpreted Value: {res['interpreted_c_value']} ({res['wrapped_sum_hex']})")
                print(f"Overflow: {'YES' if res['overflow_occurred'] else 'NO'}")
                print(f"\nDiagnosis: {res['explanation']}")
        elif subcmd == 'align':
            size = int(sys.argv[3])
            alignment = int(sys.argv[4]) if len(sys.argv) > 4 else 16
            res = math_engine.explain_alignment(size, alignment)
            print(f"\n[Math Engine] Alignment Analysis:")
            print(f"Original Size: {res['original_size']} bytes")
            print(f"Padding Needed: {res['padding_needed']} bytes")
            print(f"Aligned Size: {res['aligned_size']} bytes")
            print(f"\nDiagnosis: {res['explanation']}")
        else:
            print(f"[ERROR] Unknown math subcommand: {subcmd}")

    elif cmd == 'patch':
        if len(sys.argv) < 5:
            print("Usage: python guardian.py patch <filepath> <pkg_name> <missing_dep>")
            sys.exit(1)
        filepath = sys.argv[2]
        pkg_name = sys.argv[3]
        missing_dep = sys.argv[4]
        
        patch_engine = PatchEngine(repo_root)
        patch_file = patch_engine.generate_dependency_patch(filepath, pkg_name, missing_dep)
        
        if patch_file:
            print(f"\n[Patch Engine] [OK] Patch generated successfully!")
            print(f"Apply it using:\n  patch -p1 < {patch_file}")
        else:
            print(f"\n[Patch Engine] [ERROR] Failed to generate patch. Could not find target package {pkg_name}_DIRECTDEPS in {filepath}.")

    elif cmd == 'ai':
        if len(sys.argv) < 4:
            print("Usage: python guardian.py ai <error_summary> <keywords>")
            sys.exit(1)
        error_summary = sys.argv[2]
        keywords = sys.argv[3]
        
        ai_engine = AIEngine()
        response = ai_engine.analyze_issue(error_summary, keywords)
        
        print("\n--- AI Engine Response ---")
        print(response)
        print("--------------------------\n")

    elif cmd == 'auto-build':
        target = sys.argv[2] if len(sys.argv) > 2 else None
        auto_builder = AutoBuilder(repo_root)
        auto_builder.run_auto_build(target)

    elif cmd == 'lint':
        if len(sys.argv) < 3:
            print("Usage: python guardian.py lint <filepath>")
            sys.exit(1)
        filepath = sys.argv[2]
        if not os.path.exists(filepath):
            print(f"[Lint Error] File {filepath} not found.")
            sys.exit(1)
            
        expert = CosmoExpertModel()
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
            
        print(f"\n[Linting] Analyzing {filepath}...")
        if filepath.endswith(('.c', '.h')):
            issues = expert.validate_c_code(content)
        elif filepath.endswith(('.cpp', '.cc', '.hpp')):
            issues = expert.validate_cpp_code(content)
        elif filepath.endswith('.rs'):
            issues = expert.validate_rust_code(content)
        elif filepath.endswith('BUILD.mk'):
            issues = expert.validate_build_mk_content(content)
        else:
            print("Unsupported file type for linting.")
            sys.exit(1)
            
        if issues:
            print(f"❌ Found {len(issues)} issue(s) conflicting with Cosmopolitan philosophy:")
            for issue in issues:
                print(f"  - Ligne {issue.get('line', '?')}: {issue.get('message', issue)}")
                if 'suggestion' in issue:
                    print(f"    Suggestion: {issue['suggestion']}")
        else:
            print("✅ Code is perfectly compliant with Cosmopolitan no-std philosophy!")

    else:
        print(f"[ERROR] Unknown command: {cmd}")

if __name__ == '__main__':
    main()
