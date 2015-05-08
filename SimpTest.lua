include "SimplifiedGameOfLifeRules.lua"

WorldSize = 100;

function SimInit(State) 
	Cube2x2(State, WorldSize / 2, WorldSize / 2);
	return 0;
end;
