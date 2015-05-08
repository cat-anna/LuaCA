
GrainTable = { };
GrainTable.Count = 0;

function AllocGrain() 
	local c = GrainTable.Count;
	GrainTable.Count = c + 1
	return c;
end

function GrainInsert(Count, State, Radius)
	local i = GrainTable.Count + 1;
	local iend = i + Count;
	GrainTable.Count = iend;
	while i < iend do
		local X = xmath.RandRange(0, WorldSize-1);
		local Y = xmath.RandRange(0, WorldSize-1);
		
		local CanSet = true;
		local Cell = State:at(X, Y);
		if Cell.state == 1 then
			CanSet = false;
		end;

		if CanSet then
			Cell.grain = i;
			Cell.state = 1
			i = i + 1;
		end
	end;
end
