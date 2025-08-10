addItemType("put", ItemEnum.Other)

function ItemScripts.put.onUse(item, player, pos)
    local vector = Vector2(pos.x, pos.y)
    vector.x = vector.x - player.pos.x
    vector.y = vector.y - player.pos.y
    local length = math.sqrt(vector.x*vector.x + vector.y*vector.y)
    if (length == 0) then
        return
    end
    vector.x = vector.x/length
    vector.y = vector.y/length
    local position = Vector2(player.pos.x, player.pos.y)

    local lengthSoFar = 0
    while lengthSoFar <= length do
        position.x = position.x + vector.x*0.5
        position.y = position.y + vector.y*0.5
        lengthSoFar = lengthSoFar + 0.5
        game:putTile(position, "error")
    end

end