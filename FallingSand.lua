
WorldSize = 100;
 
function initialize(Simulation) 
	Simulation:SetName("Falling sand");
	Simulation:SetWorldSize(WorldSize, WorldSize);
	Simulation:OnInitialize("OnInitialize");	
	
	local ph1 = Simulation:CreatePhase("Phase1");
	ph1:SetDuration(100);
	ph1:SetStepCount(10000);
	ph1:OnPhaseBegin("Phase1Begin");
	ph1:OnPhaseEnd("Phase1End");
	ph1:OnClick("Phase1Click");
	ph1:AddStep("", "CellFunc", "Phase1StepBegin", "Phase1StepEnd");
	ph1:SetNeighborhoodFuncType(Const.Neighborhood.Func.Null); --Extended  BooleanStateCounter
	
	local blender = Simulation:AddBlender("Sand");
	blender:SetMode(Const.Blender.Mode.Index); --  Normalized  ValueTruth  Index
	blender:SetParameter("state");
	blender:GenerateColorsByBits(1);

	return 0;
end;

function Phase1Click(State, Phase, Coord, Button)
	local cell = State:at(Coord.x, Coord.y)
	if cell.state > 0 then 
		cell.state = 0
	else
		cell.state = Button
	end
	return 0;
end

ActionTable = { -- [[
	function (Update) end,--0
	function (Update, x11, x21, x12, x22) Update(0, x21, x11, x22); end,--1
	function (Update, x11, x21, x12, x22) Update(x11, 0, x12, x21); end,--2
	function (Update, x11, x21, x12, x22) Update(0, 0, x11, x21); end,--3
	
	function (Update, x11, x21, x12, x22) Update(x11, x21, x12, x22); end,--4
	function (Update, x11, x21, x12, x22) Update(0, x21, x12, x11); end,--5
	
	function (Update, x11, x21, x12, x22) Update(x11, 0, x12, x21); end,--6
	
	function (Update, x11, x21, x12, x22) Update(x11, 0, x11, x21); end,--7
	
	function (Update, x11, x21, x12, x22) Update(x11, x21, x12, x22); end,--8
	
	function (Update, x11, x21, x12, x22) Update(0, x21, x11, x22); end,--9
	
	function (Update, x11, x21, x12, x22) Update(x11, 0, x21, x22); end,--10
	
	function (Update, x11, x21, x12, x22) Update(0, x21, x11, x22); end,--11
	
	function (Update, x11, x21, x12, x22) Update(x11, x21, x12, x22); end,--12
	
	function (Update, x11, x21, x12, x22) Update(x11, x21, x12, x22); end,--13
	function (Update, x11, x21, x12, x22) Update(x11, x21, x12, x22); end,--14
	function (Update, x11, x21, x12, x22) Update(x11, x21, x12, x22); end,--15
--]]
}

function CellFunc(ExecData, CellPrev, CellCurr, Coord) 
	local x = Coord.x
	local y = Coord.y
	
	local PrevCtx = ExecData.PrevCtx;
	local CurrCtx = ExecData.CurrCtx;	
	
	local dx = x + 1;
	if dx >= WorldSize then dx = 0 end
	local dy = y + 1;
	if dy >= WorldSize then dy = 0 end
	
	local c11 = PrevCtx:at(x, y)
	local c12 = PrevCtx:at(x, dy)
	local c22 = PrevCtx:at(dx, dy)
	local c21 = PrevCtx:at(dx, y)
	
	if c11.state == 3 and c12.state == 0 then
		c12.state = 1
	end
	
	if c21.state == 3 and c22.state == 0 then
		c22.state = 1
	end
	
		
	if TimeState then
		if (x % 2) == 1 or (y % 2) == 1 then
			return 0;
		end
	else
		if (x % 2) == 0 or (y % 2) == 0 then
			return 0;
		end
	end
	
	local Cluster = 0;
	
	local Nc11 = CurrCtx:at(x, y)
	local Nc12 = CurrCtx:at(x, dy)
	local Nc22 = CurrCtx:at(dx, dy)
	local Nc21 = CurrCtx:at(dx, y)
	
	local x11 = c11.state
	local x21 = c21.state
	local x12 = c12.state
	local x22 = c22.state
	
	if x11 == 1 then x11 = 1 else x11 = 0 end
	if x21 == 1 then x21 = 1 else x21 = 0 end
	if x12 == 1 then x12 = 1 else x12 = 0 end
	if x22 == 1 then x22 = 1 else x22 = 0 end
	
	if c11.state > 0 then Cluster = Cluster + 1; end
	if c21.state > 0 then Cluster = Cluster + 2; end
	if c12.state > 0 then Cluster = Cluster + 4; end
	if c22.state > 0 then Cluster = Cluster + 8; end
	
	local fun = ActionTable[Cluster+1];
	local Update = function (S1, S2, S3, S4) 
	--	print("update",x, y);
		if c11.state < 2 then Nc11.state = S1; end
		if c21.state < 2 then Nc21.state = S2; end
		if c12.state < 2 then Nc12.state = S3; end
		if c22.state < 2 then Nc22.state = S4; end
	end
	
	if fun == nil then
		print("null for ", Cluster)
	else
		--print("call for ", x, y, Cluster)
		fun(Update, x11, x21, x12, x22);
	end
	
	return 0;
end;

function OnInitialize(State) 

	for i = 0,WorldSize-1,1 do
		State:at(i, WorldSize-1).state = 2;
	end
--[[
	State:at(10, 10).state = 1;
	State:at(13, 12).state = 1;
	State:at(15, 14).state = 1;
	State:at(16, 15).state = 1;--]]
	
--	State:at(12, 10).state = 1;
	return 0;
end;

TimeState = false

function Phase1StepBegin(State, Phase) 
	TimeState = not TimeState;
	--print(TimeState);
	return 0;
end;

function Phase1StepEnd(State, Phase) 
--	State:ForcePhaseEnd();
	return 0;
end;

function Phase1Begin(State, Phase) 
	return 0;
end;

function Phase1End(State, Phase) 
	return 0;
end;
