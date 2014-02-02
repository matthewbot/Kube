function register_blocktypes(types)
   print('register_blocktypes')
   local air = BlockTypeInfo.new()
   air.visible = false
   air.solid = false
   print('calling makeType')
   types:makeType("air", air)
   print('leaving makeType')

   local stone = BlockTypeInfo.new()
   stone.face_texes:fill(3)
   types:makeType("stone", stone)

   local dirt = BlockTypeInfo.new()
   dirt.face_texes.fill(2)
   types:makeType("dirt", dirt)

   local grass = BlockTypeInfo.new()
   grass.face_texes.fill(0)
   types:makeType("grass", grass)
   print('end register_blocktypes')
end
