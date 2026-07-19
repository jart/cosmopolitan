import os
import difflib

class PatchEngine:
    def __init__(self, repo_root):
        self.repo_root = repo_root
        self.patch_dir = os.path.join(repo_root, 'cosmo-gardian', 'patches')
        if not os.path.exists(self.patch_dir):
            os.makedirs(self.patch_dir)

    def generate_dependency_patch(self, rel_filepath, pkg_name, new_dep):
        abs_filepath = os.path.join(self.repo_root, rel_filepath)
        if not os.path.exists(abs_filepath):
            return None

        with open(abs_filepath, 'r', encoding='utf-8') as f:
            lines = f.readlines()

        new_lines = []
        in_deps = False
        inserted = False

        target_var = f"{pkg_name}_DIRECTDEPS"
        
        for line in lines:
            if target_var in line and "=" in line:
                in_deps = True
                new_lines.append(line)
                continue
            
            if in_deps and not inserted:
                # Add the dependency right after the declaration
                new_lines.append(f"\t\t{new_dep} \\\n")
                inserted = True
                in_deps = False
                
            new_lines.append(line)

        if not inserted:
            return None

        diff = list(difflib.unified_diff(
            lines, 
            new_lines, 
            fromfile=f"a/{rel_filepath}", 
            tofile=f"b/{rel_filepath}"
        ))

        patch_name = f"fix_{pkg_name}_{new_dep}.patch".replace('/', '_').lower()
        patch_file = os.path.join(self.patch_dir, patch_name)
        with open(patch_file, 'w', encoding='utf-8') as f:
            f.writelines(diff)

        # Return the relative path so the user can easily copy-paste
        return os.path.relpath(patch_file, self.repo_root)
