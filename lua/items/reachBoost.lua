addItemType("reachBoost", ItemEnum.Other)

function ItemScripts.reachBoost.onUse(item, player, pos)
    player.reach = player.reach + 5
end
