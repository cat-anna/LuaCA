
PhaseChanged = true;

function BasicGrowthStepBegin(State, Phase) 
	PhaseChanged = false;
	return 0;
end;

function BasicGrowthStepEnd(State, Phase) 
	if not PhaseChanged then
		State:ForcePhaseEnd();
	end
	return 0;
end;

function BasicGrainGrowthFunc(ExecData, CellPrev, CellCurr, CellTable, Coord) 
	local c = CellTable:Count();
	if CellPrev.state > 0 or c == 0 then
		return 0;
	end;

	c = c - 1;
	CellTable:SortParameter("grain", false);
	local Max = 0;
	local MaxID = 0;
	local Cur = -1;
	local CurID = -1;
	for i = 0,c,1 do
		local itm = CellTable:At(i);
		if CurID == itm.grain then
			Cur = Cur + 1;
		else
			if Cur == Max then
				if xmath.BoolRand() then
					MaxID = CurID;
				end;
			else
				if Cur > Max then 
					MaxID = CurID;
					Max = Cur;		
				end;
			end;
			Cur = 1;
			CurID = itm.grain;
		end;
	end;
	if Cur > Max then
		MaxID = CurID;
	end;
	CellCurr.state = 1;
	CellCurr.grain = MaxID;
	PhaseChanged = true;
	return 0;
end;
