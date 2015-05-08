include "GameOfLifeRules.lua"

function SimInit(State) 
	Glider1(State, 50, 50);
	Glider1(State, 90, 10);
	Glider2(State, 45, 50);
	Glider2(State, 70, 70);
	return 0;
end;
