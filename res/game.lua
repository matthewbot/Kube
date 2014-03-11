function register_blocktypes(types, visuals)
   local air = BlockTypeInfo:new()
   air.solid = false
   local air_id = types:makeType("air", air).id

   print('a')
   local stone = BlockTypeInfo:new()
   local stone_id = types:makeType("stone", stone).id
   local stone_vis = SimpleBlockVisualInfo:newFromFilename("stone.png")
   visuals:makeVisual(stone_id, stone_vis:toBlockVisualInfo())
   print('b')

   local dirt = BlockTypeInfo:new()
   local dirt_id = types:makeType("dirt", dirt).id
   local dirt_vis = SimpleBlockVisualInfo:newFromFilename("dirt.png")
   visuals:makeVisual(dirt_id, dirt_vis:toBlockVisualInfo())

   local grass = BlockTypeInfo:new()
   local grass_id = types:makeType("grass", grass).id
   local grass_vis = SimpleBlockVisualInfo:new()
   grass_vis.face_tex_filenames:fill("grass_side.png")
   grass_vis.face_tex_filenames[4] = "grass.png"
   grass_vis.face_tex_filenames[5] = "dirt.png"
   visuals:makeVisual(grass_id, grass_vis:toBlockVisualInfo())
end
