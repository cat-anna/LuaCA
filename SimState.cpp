#include "Simulation.h"

SimState::SimState(const Simulation *Sim, SharedLuaContext Lua): m_Simulation(Sim), m_CurrentTime(0) {
	cout << "Creating instance of simulation '" << Sim->GetName() << "'\n";
	m_SimPhaseIndex = 0;
	m_Lua = Lua;
	m_RemainPhaseIterations = 0;
	m_StepsDone = 0;
}

SimStateStepResult SimState::Initialize() {
	cout << "SimState " << m_Simulation->GetName() << ": Initializing simulation\n";

	m_Size = m_Simulation->GetWorldSize();

	m_FirstMemory = m_Simulation->GetParameters().CreateCellContext(m_Size);
	m_SecondMemory = m_Simulation->GetParameters().CreateCellContext(m_Size);

	m_CellNeighborhood.clear();
	m_CellNeighborhood.reserve(LinarSize() + 1);
	for(unsigned i = 0, j = LinarSize(); i < j; ++i){
		m_CellNeighborhood.push_back(CellVector(&m_Simulation->GetParameters()));
		m_CellNeighborhood.back().reserve(20);
	}

	m_Simulation->OnInitialize(this);
	TakeMeToNextPhase(true);

	return SimStateStepResult::Ok;
}

SimStateStepResult SimState::TakeMeToNextPhase(bool InitFirst) {
	unsigned NextSimIndex = m_SimPhaseIndex;
	if(!InitFirst) ++NextSimIndex;

	if(m_SimPhase) m_SimPhase->OnPhaseEnd(this);

	if(NextSimIndex >= m_Simulation->GetSimPhases().size()) {
		return SimStateStepResult::Finished;
	}
	m_SimPhaseIndex = NextSimIndex;

	m_SimPhase = m_Simulation->GetSimPhases()[m_SimPhaseIndex];
	m_TimeStep = m_SimPhase->GetDuration() / m_SimPhase->GetStepCount();

	m_RemainPhaseIterations = static_cast<unsigned>(m_SimPhase->GetStepCount());

	cout << "SimState " << m_Simulation->GetName() << ": Beginning phase " << m_SimPhaseIndex << " which has " << m_RemainPhaseIterations << " steps\n";
	cout << "Memory used by LUA: " << (GetLuaContext()->GetMemUsage() / 1024.0f) << "kb\n";

	m_SimPhase->InitPhase(this);
	m_SimPhase->OnPhaseBegin(this);
	OnNextPhase();
	m_SecondMemory->Copy(m_FirstMemory.get());
	return SimStateStepResult::Ok;
}

SimStateStepResult SimState::DoStep() {
	if(m_RemainPhaseIterations == 0) {
		SimStateStepResult r = TakeMeToNextPhase(false);
		if(r == SimStateStepResult::Finished)
			return r;
	}

	--m_RemainPhaseIterations;
	m_CurrentTime += m_TimeStep;

	m_SimPhase->ProcessStep(this);
	m_FirstMemory->Copy(m_SecondMemory.get());
	++m_StepsDone;
	return SimStateStepResult::Ok;
}

StatusInfo SimState::QuerryStatusInfo() const {
	StatusInfo mi;
	mi.SingleTableSize = m_Simulation->GetParameters().GetCellContextSize(m_Size);
	mi.LuaSize = GetLuaContext()->GetMemUsage();
	mi.StepsDone = m_StepsDone;
	mi.ParameterCount = m_Simulation->GetParameters().GetParamCount();
	mi.CellSize = m_Simulation->GetParameters().GetCellSize();
//TODO
	return mi;
}

CellContext* SimState::GetCell(int x, int y) {
	if(x < 0 || y < 0) {
		throw LuaApi::LuaPanic("Script attempts to access cell with negative index!");
	}
	unsigned linear = LinarPosition(static_cast<unsigned>(x), static_cast<unsigned>(y));
	CellContext *c = m_FirstMemory->at(linear);
//	cout << "get cell " << x << "   " << y << "\n";
	return c;
}

void SimState::ForcePhaseEnd() {
	cout << "Forcing end of phase: " << m_SimPhase->GetName() << ". Skipping " << m_RemainPhaseIterations << " iterations\n";
	m_RemainPhaseIterations = 0;
}

void  SimState::ForcePause() {

}

