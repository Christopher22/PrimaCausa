ore = {}
ore["images"] = { "Assets\\Graphic\\Resource\\ore.png" }
function ore.process(x,y)
end

function ore.getCosts(x, y)
	if Map.getTile(x, y, 0) ~= PrimaCausa.TERRAIN.WATER then
		return 500
	else
		return -1
	end
end