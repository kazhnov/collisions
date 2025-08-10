addItemType("delete", ItemEnum.Other)

function ItemScripts.delete.onUse(item, player, pos) 
    local x = -player.reach
    local y = -player.reach

    while y < player.reach+1 do
        while x < player.reach+1 do
            if math.sqrt(x*x+y*y) <= player.reach then
                local position = Vector2(pos.x+x, pos.y+y)
                game:putTile(position, "void")
                print(x, y)
            end
            x = x + 1
        end
        y = y + 1
    end
end