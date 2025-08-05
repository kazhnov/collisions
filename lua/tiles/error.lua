addTileType("error", Vector2(1, 1), Color(255, 255, 255), false)

function TileScripts.error.onCreate(tile)
    print("yay ^^")
end

function TileScripts.error.onDelete(tile)
    print("nyoo >:c")
end