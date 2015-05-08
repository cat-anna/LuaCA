/*
 * SimulationParameters.cpp
 *
 *  Created on: 8 maj 2014
 *      Author: Immethis
 */

#include <cstdlib>
#include <cstring>
#include "Simulation.h"

template<class R, class T>
R inline UnionCast(T t) {
	union {
		T t;
		R r;
	} u;
	u.t = t;
	return u.r;
}

const ParameterTypeMap ParameterTypeMapInstance;

const unsigned ParameterTypeSize[] = {
		0,
		sizeof(LuaTypes::Byte),
		sizeof(LuaTypes::Int),
		sizeof(LuaTypes::Float),
		0,
};

const char SimParameter::InternalFlags[] = "InternalFlags";
const char SimParameter::State[] = "state";
//------------------------------------------------

WorldContext::WorldContext(): m_Memory(0), m_Size(0) {
}

WorldContext::WorldContext(unsigned CellSize, const glm::uvec2 &Size): m_Memory(0), m_CellSize(CellSize), m_Size(Size) {
	unsigned size = CellSize * LinearSize();
	m_Memory = (char*)malloc(size);
	memset(m_Memory, 0, size);
}

WorldContext::~WorldContext() {
	free(m_Memory);
	m_Memory = 0;
}

void WorldContext::Copy(const WorldContext* src) {
	memcpy(m_Memory, src->m_Memory, LinearSize() * m_CellSize);
}

//------------------------------------------------

SimulationParameters::SimulationParameters(Simulation *Sim) :
		m_Simulation(Sim),
		m_CellSize(0){

	SpawnParameter(SimParameter::InternalFlags, ParameterType::Int, true);
	SpawnParameter(SimParameter::State, ParameterType::Int, false);
}

SimulationParameters::~SimulationParameters() {
}

void SimulationParameters::RegisterCellContext(SharedLuaContext LuaContext) const {
	cout << "SimulationParameters '" << m_Simulation->GetName() << "': Registering simulation variables\n";
	cout << "SimulationParameters '" << m_Simulation->GetName() << "': Each cell has " << m_CellSize << " bytes and " << m_ParamTable.size() << " variables\n";
	auto Api = luabridge::getGlobalNamespace(LuaContext->GetLuaState()).beginNamespace("api");

	auto CellClass = Api.beginClass<CellContext>("Cell");

	for(auto &it: m_ParamTable) {
		if(it->GetIsInternal()) continue;

		switch(it->GetType()) {
		case ParameterType::Byte:
			CellClass.addData(it->GetName().c_str(), UnionCast<LuaTypes::Byte CellContext::*>(it->GetByteOffset()));
			break;
		case ParameterType::Int:
			CellClass.addData(it->GetName().c_str(), UnionCast<LuaTypes::Int CellContext::*>(it->GetByteOffset()));
			break;
		case ParameterType::Float:
			CellClass.addData(it->GetName().c_str(), UnionCast<LuaTypes::Float CellContext::*>(it->GetByteOffset()));
			break;
		default:
			cerr << "Simulation parameter " << it->GetName() << " has unknown type!\n";
		}
	}
}

std::shared_ptr<SimParameter> SimulationParameters::SpawnParameter(const char* Name, ParameterType Type, bool Internal) {
	if(Type == ParameterType::Undefined) {
		cerr << "Simulation Parameter cannot have undefined type!\n";
		return 0;
	}

	unsigned offset = m_CellSize;
	unsigned size = ParameterTypeSize[static_cast<unsigned>(Type)];
	m_CellSize += size;

	std::shared_ptr<SimParameter> sp(new SimParameter(Name, offset, Type, Internal));

	m_ParamTable.push_back(sp);
	m_ParamMap.insert(std::make_pair(Name, sp));

	return sp;
}

std::shared_ptr<SimParameter> SimulationParameters::CreateParameter(const char* Name, ParameterType Type) {
	return SpawnParameter(Name, Type, false);
}

const std::shared_ptr<SimParameter> SimulationParameters::GetParameter(const char* Name) const {
	auto it = m_ParamMap.find(Name);
	if(it == m_ParamMap.end()) {
		cerr << "There is no parameter named '" << Name << "'\n";
		return 0;
	}
	return it->second;
}

const std::shared_ptr<SimParameter> SimulationParameters::GetParameter(unsigned index) const {
	if(index >= m_ParamTable.size()) {
		cerr << "Provided index exceeds parameter count: '" << index << "'\n";
		return 0;
	}
	return m_ParamTable[index];
}

unsigned SimulationParameters::GetCellContextSize(const glm::uvec2 &Size) const {
	return m_CellSize * Size[0] * Size[1];
}

SharedWorldContext SimulationParameters::CreateCellContext(const glm::uvec2& Size) const {
	cout << "Creating world context with size " << GetCellContextSize(Size) << " bytes\n";
	return SharedWorldContext(new WorldContext(m_CellSize, Size));
}
