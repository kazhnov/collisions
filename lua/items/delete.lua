addItemType("delete", ItemEnum.Other)

function ItemScripts.delete.onUse(item, player, pos) 
    local y = -player.reach

    while (y < player.reach+1.) do
        local x = -player.reach
        while (x < player.reach+1.) do
            if math.sqrt(x^2.+y^2.) <= player.reach then
                local position = Vector2(pos.x+x, pos.y+y)
                game:putTile(position, "void")
            end
            x = x + 1
        end
        y = y + 1
    end
end