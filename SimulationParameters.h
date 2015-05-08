/*
 * SimulationParameters.h
 *
 *  Created on: 8 maj 2014
 *      Author: Immethis
 */
#pragma once
#ifndef SIMULATIONPARAMETERS_H_
#define SIMULATIONPARAMETERS_H_

enum class ParameterType {
	Undefined, Byte, Int, Float,
};

struct LuaTypes {
	typedef LUA_INTEGER Int;
	typedef LUA_NUMBER Float;
	//typedef LUA_BOOLEAN Bool;
	typedef char Byte;
};

struct ParameterTypeMap : public InstancedEnumConverter<ParameterType, std::string> {
	ParameterTypeMap() {
		Add("undefined", type::Undefined);
		Add("byte", type::Byte);
		Add("int", type::Int);
		Add("float", type::Float);
	}
};

struct CellContext {
	unsigned InternalData;
	int State;
	char byes[16];
	template <class T>
	T& GetValue(unsigned ByteOffset) {
		return *reinterpret_cast<T*>(((char*)this) + ByteOffset);
	}
	template <class T>
	const T& GetValue(unsigned ByteOffset) const {
		return *reinterpret_cast<T*>(((char*)this) + ByteOffset);
	}
};

struct WorldContext {
	WorldContext();
	WorldContext(unsigned CellSize, const glm::uvec2 &Size);
	~WorldContext();

	CellContext* at(unsigned Linear) {
		return reinterpret_cast<CellContext*>(m_Memory + Linear * m_CellSize);
	}

	CellContext* atXY(int x, int y) {
		unsigned pos = LinearPosition(x, y) * m_CellSize;
		return reinterpret_cast<CellContext*>(m_Memory + pos);
	}

	void Copy(const WorldContext* src);

	unsigned LinearPosition(unsigned x, unsigned y) const { return x + y * m_Size[0]; }
	unsigned LinearPosition(const glm::uvec2& pos) const { return pos[0] + pos[1] * m_Size[0]; }
	unsigned LinearPosition(const glm::ivec2& pos) const { return pos[0] + pos[1] * m_Size[0]; }
	unsigned LinearSize() const { return m_Size[0] * m_Size[1]; }
private:

	char *m_Memory;
	unsigned m_CellSize;
	glm::uvec2 m_Size;
};

typedef std::shared_ptr<WorldContext> SharedWorldContext;

class SimParameter {
public:
	SimParameter(const char* Name, unsigned ByteOffset, ParameterType pt, bool IsInternal) :
			m_IsInternal(IsInternal),
			m_ByteOffset(ByteOffset),
			m_Name(Name),
			m_Type(pt) { }

	DefineREADAcces(Name, string)
	DefineREADAcces(ByteOffset, unsigned)
	DefineREADAcces(Type, ParameterType)
	DefineREADAcces(IsInternal, bool)

	static const char InternalFlags[];
	static const char State[];
private:
	bool m_IsInternal;
	unsigned m_ByteOffset;
	string m_Name;
	ParameterType m_Type;
};

class SimulationParameters {
public:
	typedef std::unordered_map<std::string, std::shared_ptr<SimParameter>> ParameterMap;
	typedef std::vector<std::shared_ptr<SimParameter>> PrameterVector;

	std::shared_ptr<SimParameter> CreateParameter(const char* Name, ParameterType Type);
	const std::shared_ptr<SimParameter> GetParameter(const char* Name) const;
	const std::shared_ptr<SimParameter> GetParameter(unsigned index) const;

	unsigned GetCellContextSize(const glm::uvec2 &Size) const;
	SharedWorldContext CreateCellContext(const glm::uvec2 &Size) const;
	DefineREADAcces(CellSize, unsigned)
	unsigned GetParamCount() const { return m_ParamTable.size(); }
	void RegisterCellContext(SharedLuaContext LuaContext) const;

	SimulationParameters(Simulation *Sim);
	~SimulationParameters();
private:
	Simulation *m_Simulation;
	std::shared_ptr<SimParameter> SpawnParameter(const char* Name, ParameterType Type, bool Internal);

	ParameterMap m_ParamMap;
	PrameterVector m_ParamTable;

	unsigned m_CellSize;
};

#endif /* SIMULATIONPARAMETERS_H_ */
