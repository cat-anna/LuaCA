include "Figures.lua" 

function Phase1Begin(State) 
	return 0;
end;

function Phase1End(State) 
	return 0;
end;
 
function initialize(Simulation) 
	Simulation:SetName("Test simulation");
	Simulation:SetWorldSize(100, 100);
	Simulation:OnInitialize("OnInitialize");	
	
--types:   int byte float
--	Simulation:AddParameter("state", "int");
	
	local ph1 = Simulation:CreatePhase("Phase1");
	ph1:SetDuration(100);
	ph1:SetTimeStep(0.1);
	ph1:OnPhaseBegin("Phase1Begin");
	ph1:OnPhaseEnd("Phase1End");
	ph1:SetCellFunc("GameOfLifeCellFunc");
	ph1:SetNeighborhoodMode("Moore");
	ph1:SetNeighborhoodFuncType("BooleanStateCounter");
--[[
	local ph2 = Simulation:CreatePhase("Phase2");
	ph2:SetDuration(10);
	ph2:SetTimeStep(1);
	ph2:OnPhaseBegin("Phase2Begin");
	ph2:OnPhaseEnd("Phase2End");
	ph2:SetNeighborhoodMode("Moore");
	ph2:SetNeighborhoodFuncType("BooleanStateCounter");	
]]
	return 0;
end;

function OnInitialize(State) 
	Line1x3(State, 50, 50);
	return 0;
end;


function GameOfLifeCellFunc(CellPrev, CellCurr, Count) 
	if Count < 2 or Count > 3 then
		CellCurr.state = 0;
		return 0;
	end;
	
	CellCurr.state = 1;
	return 0;
end;


