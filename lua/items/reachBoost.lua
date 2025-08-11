addItemType("reachBoost", ItemEnum.Other)

function ItemScripts.reachBoost.onUse(item, player, pos)
    enemy.goal = pos;
end
