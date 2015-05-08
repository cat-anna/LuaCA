#pragma once

enum ScriptFunResult {
	Ok,
	Abort,
	Error,
};

enum class SimStateStepResult {
	Ok,
	Finished,
	Error,
	CriticalError,
};

struct StatusInfo {
	float SingleTableSize;
	unsigned TableCount;
	float LuaSize;
	unsigned StepsDone, ParameterCount, CellSize;
};

template <class ...Arguments>
struct TemplateFunCaller {
	typedef std::function<ScriptFunResult(SimState*, Arguments...)> FuncType;
	TemplateFunCaller(const std::string& Name, FuncType fun) :m_Name(Name), m_fun(fun) { }
	TemplateFunCaller() {}
	const std::string GetName()const { return m_Name; }
	ScriptFunResult operator()(SimState* ss, Arguments ...args) const { return m_fun(ss, args...); }
	operator bool() const { return static_cast<bool>(m_fun); }
private:
	std::string m_Name;
	FuncType m_fun;
};

typedef TemplateFunCaller<> SimStateScriptFunCaller;
typedef TemplateFunCaller<const SimPhase*, const glm::uvec2 *, int> SimClickScriptFunCaller;
typedef TemplateFunCaller<const SimPhase*> SimStatePhaseScriptFunCaller;

class SimState {
public:
	SimState(const Simulation *Sim, SharedLuaContext Lua);

	SimStateStepResult Initialize();
	SimStateStepResult DoStep();

	DefineREADAcces(CurrentTime, float)
	DefineREADAcces(PhaseTime, float)
	DefineREADAcces(Size, glm::uvec2)
	DefineREADAcces(SimPhaseIndex, unsigned)
	SharedLuaContext GetLuaContext() const { return m_Lua; }
	const Simulation *GetSimulation() const { return m_Simulation; }

	WorldContext *GetFirstWorld() { return m_FirstMemory.get(); }
	WorldContext *GetSecondWorld() { return m_SecondMemory.get(); }

	CellContext *Cell(const glm::uvec2& pos) { return m_FirstMemory->at(LinarPosition(pos)); }
	CellContext *Cell(const glm::ivec2& pos) { return m_FirstMemory->at(LinarPosition(pos)); }
	CellContext *Cell(unsigned x, unsigned y) { return m_FirstMemory->at(LinarPosition(x, y)); }
	CellContext *Cell(unsigned Linear) { return m_FirstMemory->at(Linear); }

	template <class T> void SetOnNextPhase(T t) { m_OnNextPhase = t; };

	template <class ...Args>
	ScriptFunResult MakeCall(const std::string& function, Args... args) {
		int r = GetLuaContext()->RunFunction(function.c_str(), this, args...);
		if(r != 0) {
			cout << "SimState: Call to function " << function << " ended with error code " << r << "\n";
			return ScriptFunResult::Error;
		}
		return ScriptFunResult::Ok;
	}

	StatusInfo QuerryStatusInfo() const;

	template<class ... Args>
	static void CreateCallTo(TemplateFunCaller<Args...> &Pattern, const char* FunName) {
		Pattern = CreateCallTo<Args...>(FunName);
	}

	template <class ...Args>
	static TemplateFunCaller<Args...> CreateCallTo(const char* FunName) {
		//	cout << "SimState: Creating call to script function " << FunName << "\n";
			string FName = FunName;
			return TemplateFunCaller<Args...>(FunName,
					[FName] (SimState* ss, Args... args) -> ScriptFunResult {
				return ss->MakeCall(FName, args...);
			});
		}

	std::vector<CellVector>& GetCellNeighborhood() { return m_CellNeighborhood; }
	CellVector& GetCellVector(unsigned which) { return m_CellNeighborhood[which]; }

//for scripts
	CellContext* GetCell(int x, int y);
	SimPhase* CurrentPhase() const { return m_SimPhase.get(); }
	void ForcePhaseEnd();
	void ForcePause();

	unsigned LinarPosition(unsigned x, unsigned y) const { return x + y * m_Size[0]; }
	unsigned LinarPosition(const glm::uvec2& pos) const { return pos[0] + pos[1] * m_Size[0]; }
	unsigned LinarPosition(const glm::ivec2& pos) const { return pos[0] + pos[1] * m_Size[0]; }
	unsigned LinarSize() const { return m_Size[0] * m_Size[1]; }
private:
	std::shared_ptr<SimPhase> m_SimPhase;
	unsigned m_SimPhaseIndex;
	const Simulation *m_Simulation;
	std::function<void(SimState*)> m_OnNextPhase;

	float m_CurrentTime, m_PhaseTime;
	float m_TimeStep;
	unsigned m_RemainPhaseIterations, m_StepsDone;
	std::vector<CellVector> m_CellNeighborhood;

	SharedLuaContext m_Lua;

	SharedWorldContext m_FirstMemory, m_SecondMemory;
	glm::uvec2 m_Size;

	SimStateStepResult TakeMeToNextPhase(bool InitFirst = false);

	void OnNextPhase() { if(m_OnNextPhase) m_OnNextPhase(this); }
};
