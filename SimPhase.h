#pragma once

enum class SimPhaseStepResult {
	Ok, CriticalError,
};

typedef std::vector<glm::ivec2> IVec2Vector;

struct PhaseStep {
	PhaseStep(const char* Name, const char* CellFunc, const char* OnBegin, const char* OnEnd):
		m_Name(Name), m_OnBegin(OnBegin), m_OnEnd(OnEnd), m_CellFunc(CellFunc) {}

	DefineREADAcces(Name, std::string);
	DefineREADAcces(OnBegin, std::string);
	DefineREADAcces(OnEnd, std::string);
	DefineREADAcces(CellFunc, std::string);
private:

	std::string m_Name, m_OnBegin, m_OnEnd, m_CellFunc;
};

struct PhaseStepCommonData {
	SimState *State;
	SimPhase *Phase;
	const PhaseStep *Step;
	glm::uvec2 WorldSize;
	const char* CellFuncName;
	WorldContext *FirstWorld;
	WorldContext *SecondWorld;
};

struct ExecutionData {
	glm::uvec2 CellCoords;
	unsigned LinearCoord;

	glm::uvec2 *Coord;
	CellContext *First, *Second;
	CellVector *Cells;

	ExecutionData(): CellCoords(), LinearCoord(), Coord(&CellCoords) {

	}
};

class SimPhase {
public:
	SimPhase(const char* Name);
	SimPhase(const SimPhase& other, const char* Name);

	DefineREADAcces(Name, std::string)
	DefineRWAcces(Duration, float)
	DefineRWAcces(StepCount, int)

	ScriptFunResult OnPhaseBegin(SimState *ss) const { if(m_OnPhaseBegin) return m_OnPhaseBegin(ss, this); return ScriptFunResult::Ok; }
	ScriptFunResult OnPhaseEnd(SimState *ss) const { if(m_OnPhaseEnd) return m_OnPhaseEnd(ss, this); return ScriptFunResult::Ok; }
	ScriptFunResult OnClick(SimState *ss, const glm::uvec2 &coord, int btn) const { if(m_OnClick) return m_OnClick(ss, this, &coord, btn); return ScriptFunResult::Ok; }

	SimPhaseStepResult ProcessStep(SimState *ss) const;
	void SetNeighborhoodFunc(NeighborhoodFuncType type);
	void SetNeighborhoodModeFunc(NeighborhoodMode type);
	void InitPhase(SimState *ss);

//for scripts
	void SetOnPhaseBegin(const char* FunName);
	void SetOnPhaseEnd(const char* FunName);
	void SetOnStepBegin(const char* FunName);
	void SetOnStepEnd(const char* FunName);
	void SetOnClick(const char* FunName);
	void SetCellFunc(const char* FunName);
	PhaseStep* AddStep(LuaApi::TableDispatcher &t);
	const char* GetOnPhaseBegin() const { return m_OnPhaseBegin.GetName().c_str(); }
	const char* GetOnPhaseEnd() const { return m_OnPhaseEnd.GetName().c_str(); }
	void SetDuration(float f);
	void SetStepCount(int f);
	void SetStepLength(float f);
	void SetNeighborhoodMode(int Mode);
	void SetNeighborhoodFuncType(int Mode);
	void SetBoundaryMode(int Mode);
	void IgnoreInactive(bool value);
	int GetNeighborhoodMode() const { return static_cast<int>(m_Neighborhood); }
	int GetNeighborhoodFuncType() const { return static_cast<int>(m_NeighborhoodFuncType); }
	int GetBoundaryMode() const { return static_cast<int>(m_BoundaryMode); }
private:
	std::string m_Name;
	SimStatePhaseScriptFunCaller m_OnPhaseBegin, m_OnPhaseEnd;
	std::vector<PhaseStep> m_Steps;
	SimClickScriptFunCaller m_OnClick;
	float m_Duration;
	int m_StepCount;
	bool m_IgnoreInactive;
	NeighborhoodMode m_Neighborhood;
	NeighborhoodFuncType m_NeighborhoodFuncType;
	BoundaryMode m_BoundaryMode;

	typedef SimPhaseStepResult(SimPhase::*NeighborhoodFunc)(PhaseStepCommonData&, ExecutionData &) const;
	NeighborhoodFunc m_NeighborhoodFunc;
	SimPhaseStepResult NullCellFunc(PhaseStepCommonData& Commondata, ExecutionData & ExecData) const;
	SimPhaseStepResult BasicCellFunc(PhaseStepCommonData& Commondata, ExecutionData & ExecData) const;

	typedef const IVec2Vector&(*NeighborhoodModeFunc)(PhaseStepCommonData&);
	NeighborhoodModeFunc m_NeighborhoodModeFunc;
	static const IVec2Vector& MooreNeighborhoodMode(PhaseStepCommonData&);
	static const IVec2Vector& NeumanNeighborhoodMode(PhaseStepCommonData&);
	static const IVec2Vector& HexRandomNeighborhoodMode(PhaseStepCommonData&);
	static const IVec2Vector& HexLeftNeighborhoodMode(PhaseStepCommonData&);
	static const IVec2Vector& HexRightNeighborhoodMode(PhaseStepCommonData&);
	static const IVec2Vector& PentagonalNeighborhoodMode(PhaseStepCommonData&);

	typedef void(SimPhase::*BoundaryCellFunc)(PhaseStepCommonData& Commondata, ExecutionData & ExecData) const;
	BoundaryCellFunc m_BoundaryFunc;
	void CircledNeighborhoodTranslation(PhaseStepCommonData& Commondata, ExecutionData & ExecData) const;
	void CuttedNeighborhoodTranslation(PhaseStepCommonData& Commondata, ExecutionData & ExecData) const;
	void BouncedNeighborhoodTranslation(PhaseStepCommonData& Commondata, ExecutionData & ExecData) const;
};

