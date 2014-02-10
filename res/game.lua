function register_blocktypes(types)
   local air = BlockTypeInfo.new()
   air.visible = false
   air.solid = false
   types:makeType("air", air)

   local stone = BlockTypeInfo.new()
   stone.face_texes:fill(3)
   types:makeType("stone", stone)

   local dirt = BlockTypeInfo.new()
   dirt.face_texes:fill(2)
   types:makeType("dirt", dirt)

   local grass = BlockTypeInfo.new()
   grass.face_texes:fill(0)
   grass.face_texes[4] = 1
   grass.face_texes[5] = 2
   types:makeType("grass", grass)
end
