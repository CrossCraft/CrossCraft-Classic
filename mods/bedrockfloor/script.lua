function onTick()

    x = Player.getX()
    y = Player.getY() - 2
    z = Player.getZ()

    blk = World.getBlk(x, y, z)

    if not (blk == 0) then
        World.setBlk(x, y, z, 7)
    end
end
