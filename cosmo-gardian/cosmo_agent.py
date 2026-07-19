import sys
import re
import os
import json

class DependencyAnalyzer:
    def __init__(self):
        self.graph = {}
        self.pkg_files = {}

    def parse_build_mk(self, filepath):
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception as e:
            return

        # Remove line continuations for easier parsing
        content = content.replace('\\\n', ' ')

        # Find PKGS += <pkg_name>
        pkgs = re.findall(r'PKGS\s*\+=\s*(\w+)', content)
        
        for pkg in pkgs:
            if pkg not in self.graph:
                self.graph[pkg] = []
            if pkg not in self.pkg_files:
                self.pkg_files[pkg] = []

            # Extract DIRECTDEPS
            deps_match = re.search(fr'{pkg}_DIRECTDEPS\s*=(.*?)(?:\n\w|\Z)', content, re.DOTALL)
            if deps_match:
                deps = deps_match.group(1).split()
                self.graph[pkg].extend(deps)
            
            # Count SRCS
            srcs_match = re.search(fr'{pkg}_SRCS\s*=(.*?)(?:\n\w|\Z)', content, re.DOTALL)
            if srcs_match:
                self.pkg_files[pkg] = srcs_match.group(1).split()

    def find_cycles(self):
        cycles = []
        visited = set()
        path = []

        def dfs(node):
            if node in path:
                cycle_start = path.index(node)
                cycle = path[cycle_start:] + [node]
                # Avoid duplicates
                if not any(set(cycle) == set(c) for c in cycles):
                    cycles.append(cycle)
                return
            if node in visited:
                return
            
            visited.add(node)
            path.append(node)
            for neighbor in self.graph.get(node, []):
                dfs(neighbor)
            path.pop()

        for node in self.graph:
            dfs(node)
        return cycles

class CosmoExpertModel:
    def __init__(self):
        self.knowledge_base = {
            "ape": "The Actually Portable Executable format uses a shell script header to embed multiple OS-specific binary formats in one file.",
            "no-std": "In Cosmopolitan, no-std implies avoiding standard glibc headers to prevent dynamic linking, using crt.h and runtime.h instead.",
            "alignment": "Stack alignment in APE must be 16-byte boundary compliant for SSE/AVX instructions across different kernels.",
            "binary_size": "Binary size can be reduced using -ffunction-sections and -fdata-sections to allow the linker to perform dead code elimination.",
            "pkgs": "The PKGS variable should list relative directory paths within the monorepo to other modules this library depends on.",
            "srcs": "The SRCS variable contains the source files (.c, .cc, .S) used to build the object files for the current module.",
            "hdrs": "The HDRS variable lists the public header files provided by the module."
        }
        self.include_pattern = re.compile(r'^\s*#\s*include\s+[<\"]([^>\"]+)[>\"]$')
        self.forbidden_c_headers = ['stdio.h', 'stdlib.h', 'string.h', 'pthread.h']
        self.forbidden_cpp_headers = ['iostream', 'fstream', 'string', 'vector', 'thread']
        self.forbidden_rust_keywords = ['use std::', 'extern crate std;']

    def ask_expert(self, query: str) -> str:
        query_lower = query.lower()
        for key, info in self.knowledge_base.items():
            if key in query_lower:
                return f"[CosmoExpert] {info}"
        return "[CosmoExpert] I'm specialized in APE, no-std, BUILD.mk, C, C++, and Rust code semantic validation."

    def validate_build_entry(self, var_name: str, value: str) -> list:
        issues = []
        items = value.split()
        if "SRCS" in var_name or "HDRS" in var_name:
            valid_exts = (".c", ".cc", ".cpp", ".S", ".h", ".inc")
            for item in items:
                if not item.endswith(valid_exts):
                    issues.append(f"Invalid extension for {var_name}: {item}")
        if "PKGS" in var_name:
            for item in items:
                if item.startswith("/"):
                    issues.append(f"Absolute path forbidden in PKGS: {item}")
                if not any(prefix in item for prefix in ["libc/", "third_party/", "tool/"]):
                    issues.append(f"Path likely outside Cosmopolitan structure in PKGS: {item}")
        return issues

    def validate_build_mk_content(self, content: str) -> list:
        issues = []
        if 'CFLAGS' in content:
            if '-ffunction-sections' not in content and '-fdata-sections' not in content:
                 issues.append("Missing -ffunction-sections / -fdata-sections for optimal dead code elimination.")
            if '-mno-red-zone' not in content and '-mpreferred-stack-boundary=4' not in content:
                 issues.append("Consider verifying stack alignment (-mpreferred-stack-boundary=4) or -mno-red-zone for kernel-level compatibility.")
        return issues

    def validate_c_code(self, c_code_content: str) -> list:
        issues = []
        lines = c_code_content.splitlines()
        for i, line in enumerate(lines, 1):
            match = self.include_pattern.match(line)
            if match:
                header = match.group(1)
                if header in self.forbidden_c_headers:
                    issues.append({
                        'type': 'ForbiddenIncludeC',
                        'line': i,
                        'message': f"Inclusion interdite : '{header}' viole la philosophie no-std de Cosmopolitan.",
                        'suggestion': f"Utilisez les primitives de la libc Cosmopolitan (ex: libc/runtime/runtime.h) au lieu de <{header}>."
                    })
        return issues

    def validate_cpp_code(self, cpp_code_content: str) -> list:
        issues = []
        lines = cpp_code_content.splitlines()
        for i, line in enumerate(lines, 1):
            match = self.include_pattern.match(line)
            if match:
                header = match.group(1)
                if header in self.forbidden_cpp_headers:
                    issues.append({
                        'type': 'ForbiddenIncludeCpp',
                        'line': i,
                        'message': f"Inclusion interdite : '{header}' viole la philosophie no-std/APE de Cosmopolitan C++.",
                        'suggestion': f"Évitez les en-têtes standard C++ (ex: <{header}>) qui introduisent des dépendances à libstdc++. Utilisez des alternatives légères ou les primitives de la libc Cosmopolitan."
                    })
            if 'std::' in line and not any(f'std::{h}' in line for h in ['size_t', 'nullptr_t']): 
                 issues.append({
                    'type': 'StdLibUsageCpp',
                    'line': i,
                    'message': f"Utilisation de la bibliothèque standard C++ détectée (par ex. 'std::').",
                    'suggestion': "Privilégiez les fonctions de la libc Cosmopolitan ou des wrappers légers pour maintenir la portabilité APE. Considérez l'utilisation de '-fno-exceptions -fno-rtti -nostdinc++' pour les builds Cosmopolitan."
                })
        return issues

    def validate_rust_code(self, rust_code_content: str) -> list:
        issues = []
        lines = rust_code_content.splitlines()
        for i, line in enumerate(lines, 1):
            if any(keyword in line for keyword in self.forbidden_rust_keywords):
                issues.append({
                    'type': 'ForbiddenStdUsageRust',
                    'line': i,
                    'message': f"Utilisation de la bibliothèque standard Rust (`std`) détectée.",
                    'suggestion': "Pour l'intégration Cosmopolitan, considérez une compilation 'no-std' de Rust et l'utilisation de `#[no_std]` avec des crates compatibles ou l'interfaçage via FFI avec la libc Cosmopolitan."
                })
            if 'println!' in line:
                 issues.append({
                    'type': 'ForbiddenMacroRust',
                    'line': i,
                    'message': f"Utilisation de la macro 'println!' détectée. Elle dépend de la bibliothèque standard.",
                    'suggestion': "Remplacez par des fonctions d'E/S de bas niveau via FFI à la libc Cosmopolitan, ou utilisez un mécanisme de journalisation no-std."
                })
        return issues

def analyze_repo(repo_root):
    analyzer = DependencyAnalyzer()
    expert = CosmoExpertModel()
    report = {"version": "3.1", "issues": []}

    print(f"Scanning repository at {repo_root}...")
    for root, dirs, files in os.walk(repo_root):
        if 'BUILD.mk' in files:
            build_path = os.path.join(root, 'BUILD.mk')
            analyzer.parse_build_mk(build_path)
            try:
                with open(build_path, 'r', encoding='utf-8', errors='ignore') as f:
                    mk_content = f.read()
                mk_issues = expert.validate_build_mk_content(mk_content)
                rel_path = os.path.relpath(build_path, repo_root)
                for issue in mk_issues:
                    report['issues'].append({'type': 'BuildFlagMissing', 'file': rel_path, 'message': issue})
            except Exception as e:
                pass

    print("Analyzing dependencies...")
    cycles = analyzer.find_cycles()
    for cycle in cycles:
        report['issues'].append({
             'type': 'CircularDependency',
             'message': ' -> '.join(cycle),
             'suggestion': 'Extract common interfaces or merge interdependent packages.'
        })

    for pkg, files in analyzer.pkg_files.items():
        if len(files) > 40: # Threshold for dense packages
            report['issues'].append({
                 'type': 'ParallelismSuggestion',
                 'message': f"Package {pkg} is very dense ({len(files)} sources).",
                 'suggestion': f"Consider subdividing {pkg} to improve make parallelism."
            })
    
    report_path = os.path.join(repo_root, 'cosmo-gardian', 'cosmo_v3_report.json')
    try:
        with open(report_path, 'w', encoding='utf-8') as f:
             json.dump(report, f, indent=2)
        print(f"Report successfully generated at {report_path}")
    except Exception as e:
        print(f"Failed to write report: {e}")

if __name__ == '__main__':
    expert = CosmoExpertModel()
    if len(sys.argv) > 1:
        command = sys.argv[1]

        if command == 'analyze_repo' and len(sys.argv) > 2:
            repo_root = sys.argv[2]
            analyze_repo(repo_root)
        elif command in ['validate_c_code', 'validate_cpp_code', 'validate_rust_code'] and len(sys.argv) > 2:
            code_content = sys.argv[2]
            # Unescape newlines for shell compatibility
            code_content = code_content.encode().decode('unicode_escape')

            if command == 'validate_c_code':
                results = expert.validate_c_code(code_content)
                if results:
                    print(f"[C-Code Validation Error] {results}")
                else:
                    print("[C-Code Validation] Code is valid.")
            elif command == 'validate_cpp_code':
                results = expert.validate_cpp_code(code_content)
                if results:
                    print(f"[C++ Code Validation Error] {results}")
                else:
                    print("[C++ Code Validation] Code is valid.")
            elif command == 'validate_rust_code':
                results = expert.validate_rust_code(code_content)
                if results:
                    print(f"[Rust Code Validation Error] {results}")
                else:
                    print("[Rust Code Validation] Code is valid.")
        elif len(sys.argv) == 3: # BUILD.mk validation
            var_name = sys.argv[1]
            value = sys.argv[2]
            results = expert.validate_build_entry(var_name, value)
            if results:
                print(f"[BUILD.mk Validation Error] {results}")
            else:
                print("[BUILD.mk Validation] Entry is valid.")
        else: # Expert query
            print(expert.ask_expert(command))
    else:
        print('Usage: python cosmo_agent.py [query]')
        print('       python cosmo_agent.py [VAR_NAME] [VALUE]')
        print('       python cosmo_agent.py validate_[lang]_code \'[CODE_STRING]\'')
        print('       python cosmo_agent.py analyze_repo [REPO_ROOT]')