-- Helper module for testing is_main()
-- When require()'d, is_main() should return false
local cosmo = require("cosmo")

local M = {}

-- Capture the is_main() result at module load time
M.is_main_when_required = cosmo.is_main()

function M.check_is_main()
  return cosmo.is_main()
end

return M
