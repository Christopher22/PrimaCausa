metropolis = {}
metropolis["images"] = { "Assets\\3D\\metropolis.obj" }
metropolis["EXPANSION_PROBABILITY"] = 0.4
metropolis["globalsLoaded"] = false

function metropolis.process(x, y, z)	
	local needs = {[PrimaCausa.RESOURCE.GRAIN] = false, [PrimaCausa.RESOURCE.WATER] = false, [PrimaCausa.RESOURCE.FISH] = false, [PrimaCausa.RESOURCE.STONE] = false, [PrimaCausa.RESOURCE.ORE] = false, [PrimaCausa.RESOURCE.URANIUM] = false}
	local freeTiles = {}
	
	Map.loadNeightbours(x, y, z, 2)
	local n_x, n_y = Map.getNeightbour()
	while n_x ~= nil do
		local resType = Map.getTile(n_x, n_y, z)
		if needs[resType] == false then
			needs[resType] = true
		elseif Map.getTile(n_x, n_y, 0) == PrimaCausa.TERRAIN.WATER then
			needs[PrimaCausa.RESOURCE.WATER] = true
		end
		
		if resType ~= PrimaCausa.SETTLEMENT.VILLAGE and resType ~= PrimaCausa.SETTLEMENT.TOWN and resType ~= PrimaCausa.SETTLEMENT.CITY and resType ~= PrimaCausa.SETTLEMENT.METROPOLIS then
			freeTiles[#freeTiles + 1] = { x = n_x, y = n_y }
		end
		n_x, n_y = Map.getNeightbour()
	end
	
	local divinityChange = 0
	for _, fullfilled in pairs(needs) do
		if fullfilled then
			divinityChange = divinityChange + 30
		else
			divinityChange = divinityChange - 60
		end
	end
	PrimaCausa.updateDivinity(divinityChange)
	
	if needs[PrimaCausa.RESOURCE.GRAIN] and needs[PrimaCausa.RESOURCE.WATER] and needs[PrimaCausa.RESOURCE.FISH] and needs[PrimaCausa.RESOURCE.STONE] and needs[PrimaCausa.RESOURCE.ORE] and needs[PrimaCausa.RESOURCE.URANIUM] then
		if metropolis["globalsLoaded"] == false then
			settlement = Tiles.load("town")
			settlement()
			
			settlement = Tiles.load("city")
			settlement()
			
			settlement = Tiles.load("village")
			settlement()
			metropolis["globalsLoaded"] = true
		end
		
		local max_probability = 0
		local max_probability_tile = nil
		for _, free in ipairs(freeTiles) do
			local probability = 0
			
			Map.loadNeightbours(free.x, free.y, z, 1)
			local n_x, n_y = Map.getNeightbour()
			while n_x ~= nil do
				local resType = Map.getTile(n_x, n_y, z)
				if resType == PrimaCausa.SETTLEMENT.VILLAGE then probability = probability + village["EXPANSION_PROBABILITY"]
				elseif resType == PrimaCausa.SETTLEMENT.TOWN then probability = probability + town["EXPANSION_PROBABILITY"]
				elseif resType == PrimaCausa.SETTLEMENT.CITY then probability = probability + city["EXPANSION_PROBABILITY"]
				elseif resType == PrimaCausa.SETTLEMENT.METROPOLIS then probability = probability + metropolis["EXPANSION_PROBABILITY"]
				end
				n_x, n_y = Map.getNeightbour()
			end
			
			if probability >= max_probability then
				max_probability = probability
				max_probability_tile = free
			end
		end
		
		if math.random() <= max_probability then
			Map.setTile(max_probability_tile.x, max_probability_tile.y, z, PrimaCausa.SETTLEMENT.METROPOLIS)
			Map.addNextTile(max_probability_tile.x, max_probability_tile.y, z)
		end
	end
	
	Map.addNextTile(x, y, z)
end