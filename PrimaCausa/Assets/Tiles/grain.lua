grain = {}
grain["images"] = { "Assets\\Graphic\\Resource\\grain.png" }

function grain.process(x,y)
end

function grain.getCosts(x, y)
	if Map.getTile(x, y, 0) == PrimaCausa.TERRAIN.PLAIN then
		return 500
	else
		return -1
	end
end