include "GameOfLifeRules.lua"

Count=0;

function SimInit(State) 
	local max = WorldSize * 20;
	if Count > 0 then max = Count; end;
	local half = WorldSize / 2;
	local halfSub = half - 10;
	for i=0,max,1 do
		local X = math.RandRange(-halfSub, halfSub) + half;
		local Y = math.RandRange(-halfSub, halfSub) + half;
		State:at(X, Y).state = 1;
	end;
	return 0;
end;
