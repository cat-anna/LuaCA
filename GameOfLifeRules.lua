include "Figures.lua"
WorldSize = 100;
 
function initialize(Simulation) 
	Simulation:SetName("Game of life");
	Simulation:SetWorldSize(WorldSize, WorldSize);
	Simulation:OnInitialize("SimInit");	
	
	Simulation:AddStdBlender(Const.Blender.Std.StateTruth, "Life");

	local ph1 = Simulation:CreatePhase("Phase1");
	ph1:SetDuration(100);
	ph1:SetStepCount(1000);
	ph1:SetCellFunc("GameOfLifeCellFunc");
	ph1:SetNeighborhoodMode(Const.Neighborhood.Mode.Moore);
	ph1:SetNeighborhoodFuncType(Const.Neighborhood.Func.Basic);
	
--[[
local ph1b = ph1:CreateBlender();
	ph1b:SetMode(Const.Blender.Mode.ValueTruth); --  Normalized  ValueTruth  Index
	ph1b:SetParameter("state");
--	ph1b:GenerateColorsByBits(3);

	ph1b:AddColor4(0, 0, 0, 0.2);
	ph1b:AddColor4(1, 1, 1, 0.8);
--]]
	return 0;
end;

function GameOfLifeCellFunc(ExecData, CellPrev, CellCurr, Cells) 
	local Count = Cells:Count();
	if Count < 2 or Count > 3 then
		CellCurr.state = 0;
		return 0;
	end;
	if CellPrev.state == 0 and Count == 3 then
		CellCurr.state = 1;
	end;
	return 0;
end;
 