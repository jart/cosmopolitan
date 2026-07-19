import re
import os

class DependencyAnalyzer:
    def __init__(self):
        self.graph = {}
        self.pkg_files = {}

    def parse_build_mk(self, filepath):
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception:
            return

        content = content.replace('\\\n', ' ')
        pkgs = re.findall(r'PKGS\s*\+=\s*(\w+)', content)
        
        for pkg in pkgs:
            if pkg not in self.graph:
                self.graph[pkg] = []
            if pkg not in self.pkg_files:
                self.pkg_files[pkg] = []

            deps_match = re.search(fr'{pkg}_DIRECTDEPS\s*=(.*?)(?:\n\w|\Z)', content, re.DOTALL)
            if deps_match:
                deps = deps_match.group(1).split()
                self.graph[pkg].extend(deps)
            
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

    def verify_sources_exist(self, repo_root):
        missing = []
        for pkg, files in self.pkg_files.items():
            for f in files:
                # Remove any macro patterns like $(...) or architecture tags if present
                clean_f = re.sub(r'\$\(.*?\)|\b(?:aarch64|x86_64)\b/?', '', f).strip()
                if not clean_f: continue
                # In Cosmopolitan, files are often listed without a prefix, or with a full path.
                # Just check if it's in the repo_root or relative to the package itself.
                # For simplicity, we just check if it exists relative to repo_root.
                full_path = os.path.join(repo_root, clean_f)
                if not os.path.exists(full_path):
                    # Try to find it broadly or just assume missing
                    missing.append((pkg, f))
        return missing
