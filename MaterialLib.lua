 
Air = {
	Name = "Air",
	IsGass = true,
	Ro = 1.205,--dla 20 st C
	MolarMass = 0.02897,
	Cw = 1.005,
	Lambda = 0.025,
	--LepKin = 18.1e6,
	--LepDyn = 15.06e6,
	--Pr = 0.703,
};

Copper = {
	Name = "Copper",
	IsGass = false,
	Ro = 8920,--dla 20 st C
	Cw = 380,--
	MolarMass = 0,
	Lambda = 400,--przewodność cieplna
	-- LepKin = 18.1e6,--
	-- LepDyn = 15.06e6,--
	-- Pr = 0.703,--
};

Materials = { };
MaterialLib = {
	nil,
	nil,
	Count = 0,
	Alloc = function (Material) 
		local c = MaterialLib.Count;
		c = c + 1;
		Material.id = c;
		MaterialLib.Count = c;
		Materials[c] = Material;
		return c;
	end,
};

MaterialLib.Alloc(Air);
MaterialLib.Alloc(Copper);
