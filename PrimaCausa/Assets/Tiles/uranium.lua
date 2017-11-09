uranium = {}
uranium["images"] = { "Assets\\Graphic\\Resource\\uranium.png" }

function uranium.process(x,y,z)
end

function uranium.getCosts(x, y)
	local tile = Map.getTile(x, y, 0)
	if tile == PrimaCausa.TERRAIN.MOUNTAIN or tile == PrimaCausa.TERRAIN.DESERT then
		return 500
	else
		return -1
	end
end