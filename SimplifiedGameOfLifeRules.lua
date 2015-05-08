include "Figures.lua"

WorldSize = 100;
 
function initialize(Simulation) 
	Simulation:SetName("Modiffied Game of life");
	Simulation:SetWorldSize(WorldSize, WorldSize);
	Simulation:OnInitialize("SimInit");	

	local ph1 = Simulation:CreatePhase("Phase1");
	ph1:SetDuration(100);
	ph1:SetTimeSubDivs(1000);
	ph1:SetCellFunc("GameOfLifeCellFunc");
	ph1:SetNeighborhoodMode(Const.Neighborhood.Mode.Moore);
	ph1:SetNeighborhoodFuncType(Const.Neighborhood.Func.Basic);
	ph1:UseStdColorBlender(Const.Blender.Std.StateTruth);
	ph1:SetBoundaryMode(Const.Boundary.Cutted);
	return 0;
end;

function GameOfLifeCellFunc(ExecData, CellPrev, CellCurr, Cells) 
	local Count = Cells:Count();
	if Count == 2 or Count == 3 then
		CellCurr.state = 1;
	else
		CellCurr.state = 0;
	end;
	return 0;
end;
 