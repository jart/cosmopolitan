import os
import subprocess
import time
from parsers.compiler import CompilerWrapper
from engines.diagnostic import DiagnosticEngine
from engines.patch_engine import PatchEngine

class AutoBuilder:
    def __init__(self, repo_root):
        self.repo_root = repo_root
        kb_path = os.path.join(os.path.dirname(__file__), '..', 'knowledge', 'cosmopolitan.json')
        self.diag_engine = DiagnosticEngine(kb_path)
        self.compiler = CompilerWrapper(self.diag_engine)
        self.patch_engine = PatchEngine(repo_root)
        
    def run_auto_build(self, target=None, max_iterations=3):
        print("\n=======================================")
        print("🤖 [Auto-Build] Initiating Autonomous Loop")
        print(f"Target: {target if target else 'all'}")
        print("=======================================\n")
        
        for iteration in range(1, max_iterations + 1):
            print(f"[Loop {iteration}/{max_iterations}] Launching 'make'...")
            
            cmd = ['make']
            if target:
                cmd.append(target)
            
            try:
                result = subprocess.run(cmd, cwd=self.repo_root, capture_output=True, text=True)
            except Exception as e:
                print(f"[Auto-Build] Failed to execute make: {e}")
                return False

            if result.returncode == 0:
                print("\n✅ [Auto-Build] Success! Compilation completed perfectly.")
                return True
            
            print("❌ [Auto-Build] Compilation failed. Analyzing error...")
            error_msg = result.stderr if result.stderr else result.stdout
            
            diagnosis = self.diag_engine.analyze(error_msg)
            
            if diagnosis and diagnosis.get('action') == 'patch_dependency':
                missing_dep = diagnosis['missing_dep']
                print(f"🧠 [Auto-Build] Diagnostic found a missing dependency: {missing_dep}")
                
                # Deduce build path from target for the prototype
                if target:
                    # e.g., target = 'tool/hello/hello.com', we want 'tool/hello/BUILD.mk'
                    parts = target.split('/')
                    if len(parts) >= 2:
                        dir_path = "/".join(parts[:2])
                        build_mk_path = f"{dir_path}/BUILD.mk"
                        pkg_name = dir_path.replace('/', '_').upper()
                    else:
                        build_mk_path = target
                        pkg_name = target.upper()
                        
                    print(f"🛠️ [Auto-Build] Generating patch for {build_mk_path} (PKG: {pkg_name})...")
                    patch_file = self.patch_engine.generate_dependency_patch(build_mk_path, pkg_name, missing_dep)
                    
                    if patch_file:
                        print(f"🔧 [Auto-Build] Applying patch {patch_file} automatically...")
                        patch_cmd = f"patch -p1 < {patch_file}"
                        try:
                            subprocess.run(patch_cmd, shell=True, cwd=self.repo_root, check=True)
                            print("✅ [Auto-Build] Patch applied successfully. Re-launching build loop...\n")
                            time.sleep(1)
                            continue # Restart the loop
                        except Exception as e:
                            print(f"[Auto-Build] Failed to apply patch: {e}")
                            return False
                    else:
                        print("[Auto-Build] Could not generate a valid patch.")
                        return False
                else:
                    print("[Auto-Build] Cannot determine exact BUILD.mk from generic target.")
                    return False
            else:
                print("[Auto-Build] Diagnostic Engine could not find an automatic fix.")
                return False
                
        print("\n❌ [Auto-Build] Reached maximum iterations without success.")
        return False
