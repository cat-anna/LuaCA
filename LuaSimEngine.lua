 
-------------------------------------------------------
 
function NeumanNeighborhoodMode() 
	return { {0, -1}, {-1,  0}, {1,  0}, {0,  1}, };
end

-------------------------------------------------------

function CircledTranslation(Engine)

end

-------------------------------------------------------

function ExtendedMode(Engine)

end

-------------------------------------------------------
 
function InitPhaseEngine(Phase, Simulation)
	engine = { }
	
	engine.OnPhaseBegin = _G[Phase:GetOnPhaseBegin()]
	engine.OnPhaseEnd = _G[Phase:OnPhaseEnd()]
	engine.OnStepBegin = _G[Phase:OnStepBegin()]
	engine.OnStepEnd = _G[Phase:OnStepEnd()]
	engine.CellFunc = _G[Phase:GetCellFunc()]
	
	local NeighborhoodMode = Phase:GetNeighborhoodMode()
	local NeighborhoodFuncType = Phase:GetNeighborhoodFuncType()
	local BoundaryMode = Phase:GetBoundaryMode()
	
	engine.NeighborhoodTranslationFunc=CircledTranslation
	engine.NeighborhoodFunc=ExtendedMode
	engine.NeighborhoodModeFunc=NeumanNeighborhoodMode
 
	return engine
end
 
function ProcessStep(Engine, State, Phase) 
	Engine.OnStepBegin(State, Phase)
	
	
	
	Engine.OnStepEnd(State, Phase)
end

