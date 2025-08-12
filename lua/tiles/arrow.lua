addTileType("arrow", Vector2(1, 1), Color(255,255,255), true) --Создаю новый тип "а"

function TileScripts.arrow.onCreate(block) 
    block.data.speed = 10
end

function TileScripts.arrow.onStanding(block, entity) --Функция запускается когда игрок встаёт на тайл
    local pos = block.pos
    pos.x = pos.x + 1
    game:putTile(pos, "arrow") --Создаю правее от этого тайла ещё один такой же
    entity.vel = Vector2(block.data.speed, entity.vel.y)
    print(entity.vel)
end
