
GrainTable.Count = 0;

function AllocGrain() 
	local c = GrainTable.Count;
	GrainTable.Count = c + 1
	return c;
end

function GrainInsert(Count, State, Delta)
	local hd = Delta / 2;
	local x = hd;
	local y = hd;

	local i = GrainTable.Count + 2;
	local iend = i + Count;
	GrainTable.Count = iend;
	local Attempt = 0;
	while i < iend do
		local Cell = State:at(x, y);
		Cell.grain = i;
		Cell.state = 1;
		i = i + 1;		
		
		x = x + Delta;
		if x >= WorldSize then
			x = hd;
			y = y + Delta;
		end
		
		if y >= WorldSize then
			print("No space to insert grains! ", i , iend);
			return 0;
		end
	end;
end
