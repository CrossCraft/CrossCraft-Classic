# Modding Overview

All mods must be stored as individual directories in the mods/ folder.
Each mod must have a mod.txt and script.lua file.

## Mod.txt

mod.txt must contain entries followed by a zero or non-zero value.
These entries indicate the existence of functions in the script.lua (zero = does not have, non-zero = does have).


`hasStart`: Whether or not the script contains the onStart() function

`hasUpdate`: Whether or not the contains the onUpdate() function

`hasTick`: Whether or not the contains the onTick() function

`hasBreak`: Whether or not the contains the onBreak(x, y, z, blk) function

`hasPlace`: Whether or not the contains the onPlace(x, y, z, blk) function

## Script.lua

script.lua may contain various functions, variables, and these variables (for now) are guaranteed not to be overriden by other mods.
script.lua can have any of the various functions which are called upon events occuring.


`onStart()`: Called upon mod initialization

`onUpdate()`: Called upon every refresh cycle of the application -- this is essentially called every frame currently

`onTick()`: Called upon every world tick (4 per second)

`onBreak(x, y, z, blk)`: Called when the player breaks a block and provides the x, y, z coordinate of block broken and what the block was.

`onPlace(x, y, z, blk)`: Called when the player places a block and provides the x, y, z coordinate of block placed and what the block placed is.

## Libraries

All standard lua libraries should be available. In addition, CrossCraft-Classic provides two helper libraries, `World` and `Player`


World contains `World.setBlock(x, y, z, blk)` and `World.getBlock(x, y, z)`. 

`World.setBlock(x, y, z, blk)`: Will immediately set a block in the world and rebuild the chunk. BLK is the block ID to be set. These follow the classic ID scheme.

`World.getBlock(x, y, z)`: Will return a value of what block is at a given position in the world.

---

Player contains `Player.get[X/Y/Z]()` and `Player.set[X/Y/Z](v)`

`Player.getX(), Player.getY(), Player.getZ()` return the X/Y/Z positions of the player

`Player.setX(v), Player.setY(v), Player.setZ(v)` updates and teleport the player to the X/Y/Z coordinates set by argument value v.

## Version Compatibility

CrossCraft-Classic mods are forward-compatible to CrossCraft-SurvivalTest.
