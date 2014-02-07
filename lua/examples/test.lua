
local M     = bm.newmachine()

--M:runterraintest()
M:poweron(800, 600, 0, "8x8")
M:waitforquit()
M:poweroff()
M:poweron(200, 200, 0, "8x8")
M:waitforquit()

-- Note: the machine is automatically destroyed
