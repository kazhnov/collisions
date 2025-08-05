addTileType("reach", Vector2(1, 1), Color(255,255,255), true)

function TileScripts.reach.onCreate(block)
    block.data.reach = 5
end

function TileScripts.reach.onEnter(block, entity)
    entity.reach = entity.reach + block.data.reach
end

function TileScripts.reach.onLeave(block, entity)
    entity.reach = entity.reach - block.data.reach
end


