include "RandomGrainInsert.lua"
include "BasicGrainGrowth.lua"

WorldSize = 100;
 
function initialize(Simulation) 
	Simulation:SetName("Naive grain growth");
	Simulation:SetWorldSize(WorldSize, WorldSize);
	Simulation:OnInitialize("OnInitialize");	
	Simulation:AddParameter("grain", Const.Types.Int);
	
	local ph1 = Simulation:CreatePhase("Phase1");
	local ph1base = ph1
	ph1:SetDuration(100);
	ph1:SetTimeSubDivs(100);--xmath.RandRange(5, 15));
	ph1:OnStepBegin("Phase1StepBegin");
	ph1:OnStepEnd("Phase1StepEnd");
	ph1:SetCellFunc("BasicGrainGrowthFunc");
	ph1:SetNeighborhoodMode(Const.Neighborhood.Mode.Pentagonal);-- HexRandom
	ph1:SetNeighborhoodFuncType(Const.Neighborhood.Func.Basic); --Extended  BooleanStateCounter
	ph1:SetBoundaryMode(Const.Boundary.Cutted);
	ph1:OnClick("Phase1Click");
	local ph1b = ph1:CreateBlender();
	ph1b:SetMode(Const.Blender.Mode.Index); --  Normalized  ValueTruth  Index
	ph1b:SetParameter("grain");
	--ph1b:GenerateColorsByBits(3);

--[[
1	{ "Neuman", (int)NeighborhoodMode::Neuman, },
	{ "Moore", (int)NeighborhoodMode::Moore, },
	{ "HexRandom", (int)NeighborhoodMode::HexRandom, },
	{ "HexLeft", (int)NeighborhoodMode::HexLeft, },
	{ "HexRight", (int)NeighborhoodMode::HexRight, },
	{ "Pentagonal", (int)NeighborhoodMode::Pentagonal, },

]]

	ph1b:AddColor(0, 0, 0);
	for i=0,500,1 do
		local r = xmath.RandRange(0, 256) / 256
		local g = xmath.RandRange(0, 256) / 256
		local b = xmath.RandRange(0, 256) / 256
		ph1b:AddColor(r, g, b);
	end
--[[	
	ph1 = Simulation:ClonePhase(ph1, "Phase2");
	ph1:SetTimeSubDivs(5);
	ph1:OnPhaseBegin("Phase2Begin");
	ph1:OnPhaseEnd("Phase2End");
	
--	Simulation:ClonePhase(ph1, "Phase3");
	
	local ph = Simulation:ClonePhase(ph1base, "Phase4");
	ph:SetTimeSubDivs(100)
	]]
	return 0;
end;

function Phase1Click(State, Phase, Coord, Button)
	local cell = State:at(Coord.x, Coord.y);
	if cell.state == 1 then 
		return 0;
	end
	cell.state = 1;
	cell.grain = AllocGrain();
	return 0;
end


function OnInitialize(State) 
	GrainInsert(25, State, 20)
	return 0;
end;

function Phase2Begin(State) 
	GrainInsert(150, State, 50)
	return 0;
end;

function Phase2End(State) 
	return 0;
end;

PhaseChanged = true;

function Phase1StepBegin(State, Phase) 
	PhaseChanged = false;
	return 0;
end;

function Phase1StepEnd(State, Phase) 
	if not PhaseChanged then
		State:ForcePhaseEnd();
	end
	return 0;
end;
