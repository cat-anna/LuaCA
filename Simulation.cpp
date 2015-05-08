
#include "Simulation.h"


Simulation::Simulation():
		m_Name("Unnamed simulation"),
		m_Parameters(this) {
}

Simulation::~Simulation() {
}

int Simulation::Initialize(SharedLuaContext Lua) {
	std::cout << "Initializing simulation\n";
	int ret = Lua->RunFunction("initialize", this);
	if(ret) return ret;
	m_Parameters.RegisterCellContext(Lua);
	return ret;
}

SimParameter* Simulation::AddParameter(const char* Name, int Type) {
	ParameterType type = ParameterTypeMap::Cast(Type);
	if(type == ParameterTypeMap::GetDefaultValue()) {
		cerr << "Simulation '" << m_Name << "': Unknown variable type! Definition ignored!\n";
		return 0;
	}
	cout << "Adding parameter '" << Name << "' with type '" << ParameterTypeMap::ToString(type) << "'\n";
	return m_Parameters.CreateParameter(Name, type).get();
}

SimPhase* Simulation::CreatePhase(const char* Name) {
	cout << "Creating simulation phase: '" << Name << "'\n";
	std::shared_ptr<SimPhase> sp = std::shared_ptr<SimPhase>(new SimPhase(Name));
	m_SimPhases.push_back(sp);
	return sp.get();
}

SimPhase* Simulation::ClonePhase(SimPhase* Phase, const char *NewName) {
	cout << "Cloning simulation phase: '" << Phase->GetName() << "', new name: '" << NewName << "'\n";
	std::shared_ptr<SimPhase> sp = std::shared_ptr<SimPhase>(new SimPhase(*Phase, NewName));
	m_SimPhases.push_back(sp);
	return sp.get();
}

SimState* Simulation::CreateSimulation(SharedLuaContext Lua) const {
	if(m_SimPhases.empty()) {
		cerr << "Cannot create simulation without any phases!\n";
		return 0;
	}
	if(m_BlenderStore.empty()) {
		cerr << "Cannot create simulation without any phase blenders!\n";
		return 0;
	}
	return new SimState(this, Lua);
}

void Simulation::SetWorldSize(int w, int h) {
	cout << "Changed world size to (" << w << ", " << h << ")\n";
	m_WorldSize[0] = w;
	m_WorldSize[1] = h;
}

void Simulation::SetOnInitialize(const char* Func) {
	cout << "Simulation '" << m_Name << "': changing OnInitialize to script function '" << Func<< "'\n";
	m_OnInitialize = SimState::CreateCallTo(Func);
}

void Simulation::SetOnFinalize(const char* Func) {
	cout << "Simulation '" << m_Name << "': changing OnFinalize to script function '" << Func<< "'\n";
	m_OnFinalize = SimState::CreateCallTo(Func);
}

//----------------------------------------------------------------------------------------------------------

PhaseBlender *Simulation::AddBlender(const char* Name) {
	cout << "Simulation '" << m_Name << "': Creating new color blender\n";
	auto pb = std::shared_ptr<PhaseBlender>(new PhaseBlender(this, Name));
	m_BlenderStore.push_back(pb);
	return pb.get();
}

void Simulation::AddStdBlender(int which, const char* Name) {
	StdColorBlender type = StdColorBlenderMap::Cast(which);
	if(type == StdColorBlenderMap::GetDefaultValue()) {
		cout << "Simulation '" << m_Name << "': Wrong default blender id, defaulting to StateTruth " << which << "\n";
		type = StdColorBlender::StateTruth;
	} else
		cout << "Simulation '" << m_Name << "': adding standard color blender: '" << StdColorBlenderMap::ToString(type) << "'\n";
	auto pb = PhaseBlender::GetStdColorBlender(this, type, Name);
	m_BlenderStore.push_back(pb);
	//return pb.get();
}
