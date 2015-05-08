include "EvenlyGrainInsert.lua"
include "BasicGrainGrowth.lua"
include "ColorRand.lua"

WorldSize = 100;
GrainCount = 30;
GrainRadius = 20;
Continous = false;

function OnInitialize(State) 
	GrainInsert(GrainCount, State, GrainRadius)
	return 0;
end;
 
function initialize(Simulation) 
	Simulation:SetName("Dynamic recrystalization");
	Simulation:SetWorldSize(WorldSize, WorldSize);
	CellCount = WorldSize * WorldSize;
	Simulation:OnInitialize("OnInitialize");	
	Simulation:AddParameter("grain", Const.Types.Int);
	Simulation:AddParameter("neighborhood", Const.Types.Int);
	Simulation:AddParameter("rod", Const.Types.Float);
	Simulation:AddParameter("recrystalized", Const.Types.Int);
	
	local blender;

	blender = Simulation:AddBlender("Grain");
	blender:SetMode(Const.Blender.Mode.Index);
	blender:SetParameter("grain");
	RandomColors(blender, 10000);	
	
	blender	= Simulation:AddBlender("recrystalized");
	blender:SetMode(Const.Blender.Mode.Index); 
	blender:SetParameter("recrystalized");
	blender:AddColor(1,0,0);
	blender:AddColor(0,1,0);
	blender:AddColor(0,0,1);
	blender:AddColor(1,1,1);
	
	blender	= Simulation:AddBlender("neighborhood");
	blender:SetMode(Const.Blender.Mode.Index);
	blender:SetParameter("neighborhood");
	RandomColors(blender, 10);

	
	local ph1 = Simulation:CreatePhase("Naive growth");
	ph1:SetDuration(1);
	ph1:SetStepCount(100);--xmath.RandRange(5, 15));
	ph1:AddStep("", "BasicGrainGrowthFunc", "BasicGrowthStepBegin", "BasicGrowthStepEnd");
	ph1:SetNeighborhoodMode(Const.Neighborhood.Mode.Pentagonal);
	ph1:SetNeighborhoodFuncType(Const.Neighborhood.Func.Basic);
	ph1:SetBoundaryMode(Const.Boundary.Cutted);

	local ph2 = Simulation:CreatePhase("DRX");
	ph2:SetDuration(100);
	ph2:SetStepCount(100000);
	ph2:SetNeighborhoodMode(Const.Neighborhood.Mode.Pentagonal);
	ph2:SetNeighborhoodFuncType(Const.Neighborhood.Func.Basic);
	ph2:SetBoundaryMode(Const.Boundary.Cutted);
	ph2:AddStep("", "DrxCellFunc", "DrxStepBegin", "DrxStepEnd");
	ph2:OnPhaseBegin("DrxPhaseBegin");
	
	return 0;
end;

function DrxCellFunc(ExecData, CellPrev, CellCurr, CellTable, Coord) 
	
	local c = CellTable:Count();

	c = c - 1;
	CellTable:SortParameter("grain", false);
	local grains = 0;
	
	local Max = -1;
	local MaxID = -1;
	local Cur = -1;
	local CurID = -1;
	
	local most = 0;
	local recrn = false;
	local grainID = -1;--CellPrev.grain;
	for i = 0,c,1 do
		local itm = CellTable:At(i);
		if not (grainID == itm.grain) then
			grainID = itm.grain;
			grains = grains + 1;
		end 
		
		if itm.recrystalized > 1 then 		
			recrn = true;
		end
		
		if itm.recrystalized > 1 then 	
			if CurID == itm.grain then
				Cur = Cur + 1;
			else	
				if Cur == Max then
					if xmath.BoolRand() then
						MaxID = itm.grain;
					end;
				else
					if Cur > Max then 
						MaxID = CurID;
						Max = Cur;		
					end;
				end;
				Cur = 1;				
				CurID = itm.grain;
				if MaxID == -1 then
					MaxID = CurID;
				end
			end;
		end;
	end;	
	
	CellCurr.neighborhood = grains;
	
	if (not Continous) and CellCurr.recrystalized >= 1 then
		return 0;
	end
	
	local crit = (CellCurr.rod >= DRX.CriticalRo) and (grains > 1);
	
	if crit then
		recrystalize(CellCurr, AllocGrain());
		return 0;
	end
	
	if recrn and (most < 1) then
		if MaxID > 0  then
			recrystalize(CellCurr, MaxID);
		end;
		return 0;
	end;

	return 0;
end

function recrystalize(Cell, newgrain)
	Cell.recrystalized = 3;		
	Cell.grain = newgrain;
	Cell.rod = DRX.ResetRo;
end;

PhaseTime = 0;

function DrxStepBegin(State, Phase) 
	PrvTime = PhaseTime;
	
	DRX.CurrentRo = GetRo(PhaseTime);--State:GetPhaseTime());
	DRX.StepRo = DRX.CurrentRo - DRX.PrevRo;
	if PhaseTime <= 0.001 then
		return 0;
	end;
	
	local Roi = DRX.StepRo;
	local pack = Roi / CellCount;
	local c = 0;
	
	while Roi > 0 do
		if pack > Roi then
			pack = Roi;
		end;
		local X = xmath.RandRange(0, WorldSize);
		local Y = xmath.RandRange(0, WorldSize);
		local item = State:at(X, Y);		

		local used ;
		if item.neighborhood > 1 then
			-- used = pack * ( (s / 4.0));	
			used = pack * 1.2;
		else
			used = pack * xmath.RandRange(3, 8) / 10;
		end
		item.rod = item.rod + used;
		Roi = Roi - used;
	end;
	
	for x = 0,WorldSize-1,1 do
		for y = 0,WorldSize-1,1 do
			local cell = State:at(x, y);
			
			if cell.recrystalized > 1 then 
				cell.recrystalized = cell.recrystalized - 1;
			end;
		end
	end	
	
	return 0;
end;

function DrxStepEnd(State, Phase) 
	PhaseTime = PhaseTime + 0.001;
	DRX.PrevRo = DRX.CurrentRo;
	return 0;
end;

function DrxPhaseBegin(State, Phase) 
	PhaseTime = 0;
	local init_ro = DRX.initial_ro;
	for x = 0,WorldSize-1,1 do
		for y = 0,WorldSize-1,1 do
			local cell = State:at(x, y);
			cell.rod = init_ro;
			cell.recrystalized = 0;
			cell.state = 1;
		end
	end

	CellCount = WorldSize * WorldSize;
	DRX.CriticalRo = DRX.MaxRo / CellCount;
	DRX.initial_ro = GetRo(0.001) / CellCount;
	DRX.ResetRo = DRX.initial_ro;
	return 0;
end;

function GetRo(Time)
	local A = DRX.A;
	local B = DRX.B;
	local AdB = A / B;
	return (AdB + (1 - AdB) * math.exp(-B * Time));
end;

DRX = {
	A = 86710969050178.5,
	B = 9.41268203527779,
	t = 900,
	initial_ro = 0, 
	CurrentRo = 0,
	PrevRo = 0,
	CriticalRo = 0,
	MaxRo = 4.21584E+12,
	ResetRo = 0,
};
