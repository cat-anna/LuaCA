include "MaterialLib.lua"

WorldSize = 100;
CellSize = 0.01; --metry
TimeStep = 0.1;

--[[
kroki:
	1. wymiana temperatury
		zmiana temp -> zmiana ciśnienia(v = const)
	
	2. wymiana ciśnienia
		zmiana ciśnienia -> zmiana masy i temperatury
		
--]]

function initialize(Simulation) 
	Simulation:SetName("Temperature flow");
	Simulation:SetWorldSize(WorldSize, WorldSize);
	CellCount = WorldSize * WorldSize;
	Simulation:AddParameter("temp", Const.Types.Float);
	Simulation:AddParameter("pressure", Const.Types.Float);
	Simulation:AddParameter("mass", Const.Types.Float);
	Simulation:AddParameter("material", Const.Types.Int);
	
	-- Simulation:AddParameter("heating", Const.Types.Float);	
	
	local blender = Simulation:AddBlender("Temperature");
	blender:SetMode(Const.Blender.Mode.Range);
	blender:SetParameter("temp");
	blender:SetOptions({ Adaptive=false, min=0, max=1020 });
	blender:AddColor(0, 0, 1);
	blender:AddColor(0, 1, 0);
	blender:AddColor(1, 0, 0);
	
	local blender = Simulation:AddBlender("Mass");
	blender:SetMode(Const.Blender.Mode.Range);
	blender:SetParameter("mass");
	blender:SetOptions({ Adaptive=false, min=0, max=1e-5 });
	blender:AddColor(0, 0, 1);
	blender:AddColor(0, 1, 0);
	blender:AddColor(1, 0, 0);

	local blender = Simulation:AddBlender("Pressure");
	blender:SetMode(Const.Blender.Mode.Range);
	blender:SetParameter("pressure");
	blender:SetOptions({ Adaptive=true, min = 101300, max = 102325 });
	blender:AddColor(0, 0, 1);
	blender:AddColor(0, 1, 0);
	blender:AddColor(1, 0, 0);
	
	local blender = Simulation:AddBlender("Material");
	blender:SetMode(Const.Blender.Mode.Index); 
	blender:SetParameter("material");
	blender:GenerateColorsByBits(2);
	
	local blender = Simulation:AddBlender("State viewer");
	blender:SetMode(Const.Blender.Mode.Index); 
	blender:SetParameter("state");
	blender:GenerateColorsByBits(1);
	
	local ph1 = Simulation:CreatePhase("Flow");
	ph1:SetDuration(100);
	ph1:SetStepCount(100 / TimeStep);
	ph1:IgnoreInactive(false);
	
	ph1:AddStep({ 
		Name = "",
		CellFunc = "TemperatureProcess",
		OnBegin = "TempBegin", 
		-- OnEnd = "TempEnd", 
	});
	ph1:AddStep({ 
		Name = "",
		CellFunc = "PressureProcess",
		-- OnBegin = "stepBegin", OnEnd = "stepEnd", 
	});
	
	ph1:OnPhaseBegin("phaseBegin");
	ph1:OnPhaseEnd("phaseEnd");
	ph1:OnClick("Phase1Click");
	ph1:SetNeighborhoodMode(Const.Neighborhood.Mode.Neuman);
	ph1:SetNeighborhoodFuncType(Const.Neighborhood.Func.Basic);
	ph1:SetBoundaryMode(Const.Boundary.Cutted);
	
	CellSurface = CellSize * CellSize;
	CellVolume = CellSurface * CellSize;
	
	return 0;
end;

function place(State, x, y, h, t)
	local item;
	local hd = h / 2;
	for i=-hd,hd,1 do
		item = State:at(x+i, y + 0);
		RegState(State, x+i, y+0, 2);
		item.temp = t;
		SetMaterial(item, Copper);
	end;
	
	
end

function phaseBegin(State, Phase) 
	local item;
	
	for i=0,WorldSize-1,1 do
		for j=0,WorldSize-1,1 do
			item = State:at(i, j);
			SetMaterial(item, Air);
			item.temp = 20;
			item.pressure = BeginConditions.pressure;
		end;
	end;
	
	-- for i = -20, 20, 20 do
		-- place(State, 50+i, 50, 30, 20);
	-- end;
	local wsh = WorldSize/2;
	RegState(State, wsh, wsh, 3);
	return 0;
end

function Phase1Click(State, Phase, Coord, Button)
	local cell = State:at(Coord.x, Coord.y)
	print("x:".. Coord.x .. " y:" .. Coord.y, "material:"..Materials[cell.material].Name, "temp:" .. cell.temp, "pressure:".. cell.pressure, "mass:" .. cell.mass);
	return 0;
end

function TempBegin(State, Phase) 
	local count = DynStates.Count;

	for i = 1,count,1 do
		local vec = DynStates[i];
		local cell = State:at(vec.x, vec.y);

		local stateid = cell.state;
		local state = States[cell.state];
		if not (state == nil) then
			state.update(cell, state);
		end
	end
	
	local Mass = { 0, 0, 0, 0, };
	for i=0,WorldSize-1,1 do
		for j=0,WorldSize-1,1 do
			item = State:at(i, j);
			local m = item.material;
			Mass[m] = Mass[m] + item.mass;
		end;
	end;
	print(State:Time(), Materials[1].Name .. ":" .. Mass[1], Materials[2].Name .. ":" .. Mass[2]);--, Materials[3].Name .. ":" .. Mass[3]);

	return 0;
end

function TemperatureProcess(ExecData, this, CellCurr, CellTable) 
	local c = CellTable:Count() - 1;
	
	local this_temp = this.temp;
	local dT = 0;
	local this_mat = this.material;
	local this_matdata = Materials[this_mat];

	for i = 0,c,1 do
		local itm = CellTable:At(i);
		local delta = itm.temp - this_temp;
		local coeff;
		if itm.material == this_mat then
			coeff = this_matdata.Lambda;
		else
			coeff = 50;			
		end
		dT = dT + delta * coeff * TimeStep * CellSize ;--CellSurface / d;
	end
	CellCurr.temp = this_temp + dT;
	
	
	-- 8.314 - stała gazowa
	if this_matdata.IsGass then
		local dP = (this.mass / this_matdata.MolarMass) / CellVolume * 8.314 * dT;-- (this_temp + 273.15);
		this.pressure = this.pressure + dP;
	end 
	
	return TemperatureProcess;
end

function sign(x) 
	if x < 0 then return -1; end;
	if x > 0 then return 1; end;
	return 0;
end

function PressureProcess(ExecData, this, CellCurr, CellTable) 
	local this_pressure = this.pressure;
	local this_temp = this.temp;
	local this_coord = ExecData.Coord;	

	local this_mat = this.material;
	local this_matdata = Materials[this_mat];
	
	local dP = 0;
	local dm = 0;
	local dT = this.mass * this_matdata.Cw * this_temp;	
	
	if not (this_matdata.IsGass) then
		return 0;
	end
	-- print("asdas");
	-- local molar = (this.mass / this_matdata.MolarMass);
	
	local c = CellTable:Count() - 1;
	for i = 0,c,1 do
		local itm = CellTable:At(i);
		if itm.material == this_mat then
			local delta = itm.pressure - this_pressure;
			local coeff = 1;
			local mass_coeff = 1;
			local itm_coord = CellTable:PosAt(i);
			local avg_temp = 273.15 + math.max(this.temp, itm.temp);
			local dy = 0;
			
			if this_coord.x == itm_coord.x then
				
				dy = this_coord.y - itm_coord.y;
				
				-- if not (dy == 0) then
					-- delta = delta + -9.81 * dy;
				-- end;
			end			
			
			-- print("tst", delta, dy);
			
			if sign(delta) == -dy then
				local l1 = math.min(itm.temp, this_temp);
				local l2 = math.max(itm.temp, this_temp);		
				local c = (l2 / l1);
				delta = delta * math.min(c, 2);--9.81 *3 dy * 100;
			end
			
			local mass_move = 0;
			
			if math.abs(delta) > 10 then 
				mass_coeff = 2;
			end;

			mass_move = delta * this_matdata.MolarMass * CellVolume / (8.314 * avg_temp);
			mass_move = mass_move * mass_coeff * TimeStep;
			
			if mass_move > 0 then
			--jeśli masa odpływa, ciepła nie ubywa
				dT = dT + mass_move * this_matdata.Cw * item.temp;
			end
			
			dm = dm + mass_move;
			dP = dP + delta * coeff * TimeStep;
		end
	end
	dm = dm + this.mass;
	CellCurr.pressure = this_pressure + dP;
	CellCurr.mass = dm;
	CellCurr.temp = dT / (this_matdata.Cw * dm);
	return PressureProcess;
end

DynStates = { 
	Count = 0
};

function RegState(State, ax, ay, id) 
	local c = DynStates.Count;
	c = c + 1;
	DynStates.Count = c;
	local vec = { x = ax, y = ay, state = id };
	DynStates[c] = vec;
	State:at(ax, ay).state = id;
end

States = { }
States[2] = {
	temp = 100,
	update = function(cell, state_data)
		cell.temp = state_data.temp;	
	end
}

States[3] = {
	temp = 1000,
	pressure = 101325 * 1.1;
	update = function(cell, state_data)
		cell.temp = state_data.temp;	
		cell.pressure = state_data.pressure;	
	end
}


function SetMaterial(Cell, Material)
	Cell.material = Material.id;
	Cell.mass = Material.Ro * math.pow(CellSize, 3);
end;

BeginConditions = {
	Temp = 20,
	pressure = 101325,
};
