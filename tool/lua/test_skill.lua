-- test skill module

local skill = require("cosmo.skill")

-- Test that module loads
assert(skill, "skill module should load")
assert(type(skill.install) == "function", "skill.install should be a function")
assert(type(skill.generate_skill_md) == "function", "skill.generate_skill_md should be a function")
assert(type(skill.generate_docs) == "function", "skill.generate_docs should be a function")

-- Test doc generation (also tests dynamic module discovery)
local docs, modules = skill.generate_docs()
assert(type(docs) == "table", "generate_docs should return docs table")
assert(type(modules) == "table", "generate_docs should return modules table")

-- Check that modules were discovered
assert(modules[""], "should discover top-level functions (empty prefix)")
assert(modules["unix"], "should discover unix module")

-- Check that docs were generated
assert(docs["cosmo.md"], "should generate cosmo.md")
assert(docs["cosmo-unix.md"], "should generate cosmo-unix.md")

-- Check cosmo.md has content (not empty)
local cosmo_doc = docs["cosmo.md"]
assert(cosmo_doc:match("# cosmo"), "cosmo.md should have module header")
assert(#cosmo_doc > 100, "cosmo.md should have substantial content")

-- Check unix docs has content
local unix_doc = docs["cosmo-unix.md"]
assert(unix_doc:match("# cosmo%.unix"), "cosmo-unix.md should have module header")
assert(unix_doc:match("fork") or unix_doc:match("open"), "cosmo-unix.md should have function docs")

-- Test SKILL.md generation with discovered modules
local skill_content = skill.generate_skill_md(modules)
assert(skill_content:match("^%-%-%-"), "SKILL.md should start with frontmatter")
assert(skill_content:match("name: cosmo%-lua"), "SKILL.md should have correct name")
assert(skill_content:match("whilp/cosmopolitan"), "SKILL.md should reference whilp/cosmopolitan")
assert(skill_content:match("cosmo%.md"), "SKILL.md should reference cosmo.md")
assert(skill_content:match("cosmo%-unix%.md"), "SKILL.md should reference cosmo-unix.md")

print("all skill tests passed")
