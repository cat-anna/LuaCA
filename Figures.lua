
function Glider1(State, X, Y)
	State:at(X, Y).state = 1;
	State:at(X+1, Y).state = 1;
	State:at(X+2, Y).state = 1;
	State:at(X, Y+1).state = 1;
	State:at(X+1, Y+2).state = 1;
end;

function Glider2(State, X, Y)
	State:at(X, Y+2).state = 1;
	State:at(X+1, Y+2).state = 1;
	State:at(X+2, Y+2).state = 1;
	State:at(X, Y+1).state = 1;
	State:at(X+1, Y).state = 1;
end;

function Cube2x2(State, X, Y) 
	State:at(X, Y+1).state = 1;
	State:at(X+1, Y+1).state = 1;
	State:at(X+1, Y).state = 1;
	State:at(X, Y).state = 1;
end;

function Line1x3(State, X, Y) 
	State:at(X, Y  ).state = 1;
	State:at(X, Y+1).state = 1;
	State:at(X, Y+2).state = 1;
end;
