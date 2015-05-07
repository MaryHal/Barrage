-- BulletLua Test Script 2

-- These variables are local to this file.
local offset1 = 36.1
local offset2 = -36.4

local dir1 = offset1
local dir2 = offset2

local test02
test02 = {
   main = function ()
      local turn = getTurn()
      local rank = getRank()

      launch(dir1, 0.8, test02.fade)
      launch(dir2, 0.8, test02.fade)
      dir1 = dir1 + offset1
      dir2 = dir2 + offset2

      if (turn == 600) then
         kill()
      end
   end,

   fade = function ()
      local turn = getTurn()
      if (turn == 130) then
         vanish()
      end
   end
}
return test02
