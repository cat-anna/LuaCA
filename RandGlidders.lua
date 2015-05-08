include "GameOfLifeRules.lua"

Count=0

function SimInit(State) 
	local max = WorldSize / 5;
	if Count > 0 then
		max = Count;
	end
	for i=1,max,1 do
		local X = xmath.RandRange(5, WorldSize-5);
		local Y = xmath.RandRange(5, WorldSize-5);
		if xmath.BoolRand() == 0 then 
			Glider1(State, X, Y);
		else
			Glider2(State, X, Y);
		end;
	end;
	return 0;
end;
