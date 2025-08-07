addItemType("reachBoost", ItemEnum.Other)

function ItemScripts.reachBoost.onUse(item, player, pos)
    player.color.r = math.random(255)
    player.color.g = math.random(255)
    player.color.b = math.random(255)
end
