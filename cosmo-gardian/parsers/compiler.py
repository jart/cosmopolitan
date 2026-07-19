import subprocess
import os
import sys

class CompilerWrapper:
    def __init__(self, diagnostic_engine):
        self.diagnostic = diagnostic_engine

    def run_make(self, target=None, repo_root='.'):
        cmd = ['make']
        if target:
            cmd.append(target)
            
        print(f"[Cosmo Sentinel] Running: {' '.join(cmd)} in {os.path.abspath(repo_root)}")
        
        # We capture stderr to analyze it, but we let stdout flow to the user usually.
        # For simplicity here, we capture both to parse completely.
        process = subprocess.Popen(
            cmd, 
            cwd=repo_root, 
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE,
            text=True
        )
        
        stdout, stderr = process.communicate()
        
        if process.returncode != 0:
            print("\n[Cosmo Sentinel] Build Failed! Analyzing errors...\n")
            diagnostics = self.diagnostic.analyze_compiler_error(stderr)
            
            if diagnostics:
                for d in diagnostics:
                    print(f"❌ {d['raw_error']}")
                    print(f"   💡 Explanation: {d['explanation'].replace(chr(10), chr(10)+'      ')}")
                    print(f"   🔧 Suggestion: {d['suggestion']}")
                    print("-" * 60)
            else:
                print("❌ Could not match errors to Cosmopolitan knowledge base. Raw output:")
                print(stderr[-2000:]) # print last 2000 chars to avoid flooding terminal
            sys.exit(process.returncode)
        else:
            print("[Cosmo Sentinel] Build Succeeded.")
