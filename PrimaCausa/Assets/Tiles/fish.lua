fish = {}
fish["images"] = { "Assets\\Graphic\\Resource\\fish.png" }
function fish.process(x,y)
end

function fish.getCosts(x, y)
	if Map.getTile(x, y, 0) == PrimaCausa.TERRAIN.WATER then
		return 500
	else
		return -1
	end
end