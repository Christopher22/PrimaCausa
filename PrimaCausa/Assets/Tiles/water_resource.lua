water_resource = {}
water_resource["images"] = { "Assets\\Graphic\\Resource\\water.png" }

function water_resource.process(x,y)
end

function water_resource.getCosts(x, y)
	if Map.getTile(x, y, 0) ~= PrimaCausa.TERRAIN.WATER then
		return 500
	else
		return -1
	end
end