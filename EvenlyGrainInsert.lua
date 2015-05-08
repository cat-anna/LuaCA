
GrainTable = { }
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
	local Attempt = 0;
	while i < iend do
		local X = xmath.RandRange(0, WorldSize-1);
		local Y = xmath.RandRange(0, WorldSize-1);
		
		local CanSet = true;
		local Cell = State:at(X, Y);
		if i > 1 then
			if Cell.state == 1 then
				CanSet = false
			else
				if Attempt < 50 then
					for j=1,i-1,1 do
						local dx = GrainTable[j].x - X;
						local dy = GrainTable[j].y - Y;
						local r = math.sqrt(dx*dx + dy*dy);
						if r < Radius then
							CanSet = false;
							Attempt = Attempt + 1;
							break;				
						end
					end;
				else
					print("50 attempts were made in order to insert grain " .. i);
				end
			end;
		end;
		if CanSet then
			Cell.grain = i;
			Cell.state = 1
			Attempt = 0
			GrainTable[i] = { };
			GrainTable[i].x = X;
			GrainTable[i].y = Y;
			i = i + 1;
		end
	end;
end
