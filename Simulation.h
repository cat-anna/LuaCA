#pragma once

#include <iostream>

using std::cout;
using std::cerr;
using std::string;

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

#include <glm\glm.hpp>

#define AbstractREADAcces(NAME, TYPE) virtual const TYPE &Get##NAME() const = 0;
#define VirtualREADAcces(NAME, TYPE) virtual const TYPE &Get##NAME() const { return m_##NAME; }

#define DefineREADAcces(NAME, TYPE) const TYPE &Get##NAME() const { return m_##NAME; }
#define DefineWRITEAcces(NAME, TYPE) const TYPE &Set##NAME(const TYPE &val) { m_##NAME = val; return val; }
#define DefineRWAcces(NAME, TYPE) TYPE &Get##NAME() { return m_##NAME; }

#define DefineFULLAcces(NAME, TYPE)	DefineRWAcces(NAME, TYPE) DefineREADAcces(NAME, TYPE) DefineWRITEAcces(NAME, TYPE)

#define DefineREADAccesPTR(NAME, TYPE) const TYPE *Get##NAME() const { return m_##NAME; }

#define DefineFlagChecker(FLAGS, FLAG, NAME) bool Is##NAME() const { return (FLAGS & (FLAG)) == (FLAG); }
#define DefineFlagSetter(FLAGS, FLAG, NAME) void Set##NAME(bool Value) { SetFlags(FLAGS, FLAG, Value); }

#include "EnumConverter.h"

class PhaseBlender;
class SimState;
class SimParameter;
class Simulation;
class SimPhase;
class SimulationParameters;
struct CellContext;

#include "Types.h"
#include "LuaContext.h"
using LuaApi::SharedLuaContext;
#include "PhaseBlender.h"
#include "SimulationParameters.h"
#include "src/simulation/SimHdr.h"
#include "SimState.h"
#include "SimPhase.h"

class Simulation {
public:
	Simulation(const Simulation&) = delete;
	Simulation();
	~Simulation();

	typedef std::vector<std::shared_ptr<SimPhase>> SimPhaseVector;

//for scripts
	void SetName(const char* Name) { m_Name = Name; }
	SimParameter* AddParameter(const char* Name, int Type);
	void SetOnInitialize(const char* Func);
	void SetOnFinalize(const char* Func);
	void SetWorldSize(int w, int h);
	SimPhase* CreatePhase(const char* Name);
	SimPhase* ClonePhase(SimPhase* Phase, const char *NewName);

	PhaseBlender *AddBlender(const char* Name);
	void AddStdBlender(int which, const char* Name);

	const SimPhaseVector& Simulations() const { return m_SimPhases; }
	SimulationParameters& GetParameters() { return m_Parameters; }
	const SimulationParameters& GetParameters() const { return m_Parameters; }

	int Initialize(SharedLuaContext Lua);
	SimState* CreateSimulation(SharedLuaContext Lua) const;
		
	DefineREADAcces(Name, std::string);
	DefineREADAcces(WorldSize, glm::uvec2)
	DefineREADAcces(SimPhases, SimPhaseVector)

	ScriptFunResult OnInitialize(SimState *ss) const { if(m_OnInitialize) return m_OnInitialize(ss); return ScriptFunResult::Ok; }
	ScriptFunResult OnFinalize(SimState *ss) const { if(m_OnFinalize) return m_OnFinalize(ss); return ScriptFunResult::Ok; }

	unsigned BlenderCount() const { return m_BlenderStore.size(); }
	SharedPhaseBlender GetBlender(unsigned index) { return m_BlenderStore[index]; }
private:
	std::vector<SharedPhaseBlender> m_BlenderStore;
	std::string m_Name;
	SimPhaseVector m_SimPhases;
	SimulationParameters m_Parameters;
	
	glm::uvec2 m_WorldSize;
	SimStateScriptFunCaller m_OnInitialize, m_OnFinalize;
};

