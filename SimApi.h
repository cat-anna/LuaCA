#pragma once

namespace SimApi {

int RandRange(int rmin, int rmax);
bool BoolRand();

struct LuaConst_t{
	const char *Name;
	int Value;
};

struct LuaConstNamespace_t {
	const char* Name;
	LuaConstNamespace_t *SubNamespaces;
	LuaConst_t *Constants;

	template <class RET, class INPUT>
	RET Process(INPUT inp) const {
		auto n = inp.beginNamespace(Name);
		if(Constants) {
		for(LuaConst_t *i = Constants; i->Name; ++i)
			n.addVariable(i->Name, &i->Value, false);
		}
		if(SubNamespaces) {
			return SubNamespaces[0].ProcessTable<RET>(n).endNamespace();
		}
		return n.endNamespace();
	}
	template <class RET, class INPUT>
	RET ProcessTable(INPUT inp) const {
		if(!Name) return inp;
		return (this + 1)->ProcessTable<RET>(Process<RET>(inp));
	}
};

//-----------------

extern LuaConstNamespace_t RootNamespace[];

#ifndef IGNORE_SIMAPI

void InitSimApi(ApiInitializer Initializer) {

	auto api1 = Initializer
	.addFunction("include", &LuaApi::LuaContext::LoadAdditionalScript);

	auto api2 = RootNamespace[0].ProcessTable<ApiInitializer>(api1);
	api2.beginNamespace("api")
//  	.addCFunction("AddLog", &Core::cScriptEngine::Lua_AddLog)
//	RegisterLuaFunction(&cScriptEngine::Lua_AddLog, "AddLog", root.);

	.beginClass<ExecutionData>("ExecutionData")
		.addData("Coord", &ExecutionData::Coord)
		.addData("CellPrev", &ExecutionData::First)
		.addData("CellCurr", &ExecutionData::Second)
		.addData("CellTable", &ExecutionData::Cells)
	.endClass()

  	.beginClass<SimState>("SimState")
  		.addFunction("at", &SimState::GetCell)
  		.addFunction("Time", &SimState::GetCurrentTime)
		.addFunction("PhaseTime", &SimState::GetPhaseTime)
  		.addFunction("ForcePhaseEnd", &SimState::ForcePhaseEnd)
//  		.addFunction("ForcePause", &SimState::ForcePause)
  	.endClass()

  	.beginClass<SimParameter>("SimParameter")
  	.endClass()

  	.beginClass<SimPhase>("SimPhase")
  		.addFunction("OnPhaseBegin", &SimPhase::SetOnPhaseBegin)
  		.addFunction("OnPhaseEnd", &SimPhase::SetOnPhaseEnd)
  		.addFunction("AddStep",  LuaApi::TableCallDispatcherEx<SimPhase, PhaseStep*, &SimPhase::AddStep>::GetInstance())
  		.addFunction("OnClick", &SimPhase::SetOnClick)

  		.addFunction("SetDuration", &SimPhase::SetDuration)
  		.addFunction("SetStepLength", &SimPhase::SetStepLength)
 		.addFunction("SetStepCount", &SimPhase::SetStepCount)
//  		.addFunction("GetDuration", &SimPhase::GetDuration)
//  		.addFunction("GetTimeSubDivs", &SimPhase::GetStepCount)

  		.addFunction("IgnoreInactive", &SimPhase::IgnoreInactive)
  		.addFunction("SetNeighborhoodMode", &SimPhase::SetNeighborhoodMode)
  		.addFunction("SetNeighborhoodFuncType", &SimPhase::SetNeighborhoodFuncType)
  		.addFunction("SetBoundaryMode", &SimPhase::SetBoundaryMode)
//  		.addFunction("GetNeighborhoodMode", &SimPhase::GetNeighborhoodMode)
//  		.addFunction("GetNeighborhoodFuncType", &SimPhase::GetNeighborhoodFuncType)
//  		.addFunction("GetBoundaryMode", &SimPhase::GetBoundaryMode)
  	.endClass()

  	.beginClass<PhaseStep>("PhaseStep")
  	.endClass()

  	.beginClass<CellVector>("CellVector")
		.addFunction("At", &CellVector::ScriptAt)
		.addFunction("PosAt", &CellVector::PosAt)
		.addFunction("Count", &CellVector::Count)
		.addFunction("SortParameter", &CellVector::SortParameter)
  	.endClass()

  	.beginClass<WorldContext>("WorldContext")
  		.addFunction("at", &WorldContext::atXY)
  	.endClass()

  	.beginClass<Simulation>("Simulation")
  		.addFunction("SetName", &Simulation::SetName)
  		.addFunction("SetWorldSize", &Simulation::SetWorldSize)
  		.addFunction("AddParameter", &Simulation::AddParameter)
  		.addFunction("CreatePhase", &Simulation::CreatePhase)
  		.addFunction("ClonePhase", &Simulation::ClonePhase)
  		.addFunction("OnInitialize", &Simulation::SetOnInitialize)
  		.addFunction("OnFinalize", &Simulation::SetOnFinalize)

  		.addFunction("AddBlender", &Simulation::AddBlender)
  		.addFunction("AddStdBlender", &Simulation::AddStdBlender)
  	.endClass()

  	.beginClass<PhaseBlender>("PhaseBlender")
  		.addFunction("SetMode", &PhaseBlender::SetMode)
  		.addFunction("SetParameter", &PhaseBlender::SetParameter)
  		.addFunction("AddColor", &PhaseBlender::AddColor)
  		.addFunction("AddColor4", &PhaseBlender::AddColor4)
  		.addFunction("GenerateColorsByBits", &PhaseBlender::GenerateColorsByBits)
  		.addCFunction("SetOptions", LuaApi::TableCallDispatcher<PhaseBlender, &PhaseBlender::SetParameters>::GetInstance())
//  		.addFunction("GenerateColors", &PhaseBlender::GenerateColors)
  	.endClass()

   	.beginClass<PhaseStepCommonData>("PhaseStepData")
		.addData("State", &PhaseStepCommonData::State)
		.addData("Phase", &PhaseStepCommonData::Phase)
  	.endClass()

  	.beginClass<glm::uvec2>("uvec2")
  		.addData("x", &glm::uvec2::x)
  		.addData("y", &glm::uvec2::y)
  	.endClass()

//	.beginClass<c3DPoint>("c3DPoint")
//		.addConstructor<void(*)(float, float, float)>()
//		.Constructor()
//		.Constructor<c3DPoint>()
//		.Constructor<float>()
//		.Member("x", &c3DPoint::x)
//		.Member("y", &c3DPoint::y)
//		.Member("z", &c3DPoint::z)
//	.endClass()

  	.endNamespace()

	//luabridge::getGlobalNamespace(m_Lua)
	.beginNamespace("xmath")
//		.addFunction("sin", &sin)
//		.addFunction("randomseed", &srand)
//		.addFunction("rand", &rand)
		.addFunction("RandRange", &RandRange)
		.addFunction("BoolRand", &BoolRand)
//		.addFunction("randomf", &randomf)
	.endNamespace()

//	.beginNamespace("glfw")
//		.addFunction("GetTime", &glfwGetTime)
//	.endNamespace()

	;
}

#endif

}//namespace SimApi
