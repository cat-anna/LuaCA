include "quickflush.lua"
include "ColorRand.lua"

WorldSize = 100;
GrainCount = 100;

function OnInitialize(State) 
	return 0;
end;
 
function initialize(Simulation) 
	Simulation:SetName("Naive grain growth");
	Simulation:SetWorldSize(WorldSize, WorldSize);
	Simulation:OnInitialize("OnInitialize");	
	Simulation:AddParameter("grain", Const.Types.Int);
	Simulation:AddParameter("neighborhood", Const.Types.Int);
	Simulation:AddParameter("largest", Const.Types.Int);
	
	local ph1 = Simulation:CreatePhase("Phase1");
	local ph1base = ph1
	ph1:SetDuration(1);
	ph1:SetStepCount(1000);--xmath.RandRange(5, 15));
	ph1:OnPhaseBegin("MCPhaseBegin");
	ph1:OnPhaseEnd("MCPhaseEnd");	
	ph1:AddStep("", "MCCellFunc", "MCStepBegin", "MCStepEnd");
	ph1:SetNeighborhoodMode(Const.Neighborhood.Mode.Moore);-- HexRandom
	ph1:SetNeighborhoodFuncType(Const.Neighborhood.Func.Basic); --Extended  BooleanStateCounter
	ph1:SetBoundaryMode(Const.Boundary.Cutted);
	-- ph1:OnClick("Phase1Click");
	
	local blender;

	blender = Simulation:AddBlender("Grain");
	blender:SetMode(Const.Blender.Mode.Index);
	blender:SetParameter("grain");
	RandomColors(blender, WorldSize * WorldSize + 1);	
	
	blender	= Simulation:AddBlender("neighborhood");
	blender:SetMode(Const.Blender.Mode.Index);
	blender:SetParameter("neighborhood");
	RandomColors(blender, 10);
	
	return 0;
end;

function MCCellFunc(ExecData, CellPrev, CellCurr, CellTable, Coord) 
	local c = CellTable:Count();

	c = c - 1;
	CellTable:SortParameter("grain", false);
	local grains = 0;
	local grainID = -1;--CellPrev.grain;
	
	local Max = -1;
	local MaxID = -1;
	local Cur = -1;
	local CurID = -1;
	
	for i = 0,c,1 do
		local itm = CellTable:At(i);
		if not (grainID == itm.grain) then
			grainID = itm.grain;
			grains = grains + 1;
		end 
		
		
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
	
	CellCurr.neighborhood = grains;
	CellCurr.largest = MaxID;
	
	return 0;
end

RandomCoords = { };
RandomCoords.Count = 0;

function MCPhaseBegin(State, Phase) 

	local index = 1;
	for ix = 0,WorldSize-1,1 do
		for iy = 0,WorldSize-1,1 do
			local cell = State:at(ix, iy);
			cell.state = 1;
			cell.grain = ix + iy * WorldSize;--xmath.RandRange(1, GrainCount+1);
			
			local coord = { x = ix, y = iy };
			RandomCoords[index] = coord;
			index = index + 1;
		end
	end
	RandomCoords.Count = index;
	return 0;
end;

function MCPhaseEnd(State, Phase) 


	return 0;
end;

PhaseChanged = true;

function MCStepBegin(State, Phase) 
	-- PhaseChanged = false;
	return 0;
end;

function MCStepEnd(State, Phase) 

	local size = RandomCoords.Count - 1;
	quickFlush(RandomCoords);
	for i=1,size,1 do
		local coord = RandomCoords[i];
		local cell = State:at(coord.x, coord.y);
	
		cell.grain = cell.largest;
	end;

	-- if not PhaseChanged then
		-- State:ForcePhaseEnd();
	-- end
	return 0;
end;
