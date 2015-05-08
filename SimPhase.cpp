
#include "Simulation.h"
#include <algorithm>

//---------------------------------------

SimPhase::SimPhase(const char* Name):
		m_Name(Name),
		m_Duration(10),
		m_Neighborhood(NeighborhoodMode::Default),
		m_NeighborhoodFuncType(NeighborhoodFuncType::Default),
		m_BoundaryMode(BoundaryMode::Default),
		m_IgnoreInactive(true),
		m_StepCount(1) {
	SetNeighborhoodFunc(m_NeighborhoodFuncType);
	SetNeighborhoodModeFunc(m_Neighborhood);

	m_BoundaryFunc = &SimPhase::CircledNeighborhoodTranslation;
}

SimPhase::SimPhase(const SimPhase& other, const char* Name): SimPhase(other) {
	m_Name = Name;
}

void SimPhase::SetNeighborhoodModeFunc(NeighborhoodMode type) {
	switch (type) {
	case NeighborhoodMode::Neuman:
		m_NeighborhoodModeFunc = &SimPhase::NeumanNeighborhoodMode;
		m_Neighborhood = NeighborhoodMode::Neuman;
		break;
	case NeighborhoodMode::HexRandom:
		m_NeighborhoodModeFunc = &SimPhase::HexRandomNeighborhoodMode;
		break;
	case NeighborhoodMode::HexLeft:
		m_NeighborhoodModeFunc = &SimPhase::HexLeftNeighborhoodMode;
		break;
	case NeighborhoodMode::HexRight:
		m_NeighborhoodModeFunc = &SimPhase::HexRightNeighborhoodMode;
		break;
	case NeighborhoodMode::Pentagonal:
		m_NeighborhoodModeFunc = &SimPhase::PentagonalNeighborhoodMode;
		break;
	case NeighborhoodMode::Unknown:
		type = NeighborhoodMode::Moore;
		//no break;
	case NeighborhoodMode::Moore:
		m_NeighborhoodModeFunc = &SimPhase::MooreNeighborhoodMode;
		break;
	}
	m_Neighborhood = type;
}

void SimPhase::SetNeighborhoodFunc(NeighborhoodFuncType type) {
	switch (type) {
	case NeighborhoodFuncType::Null:
		m_NeighborhoodFunc = &SimPhase::NullCellFunc;
		break;
	case NeighborhoodFuncType::Unknown:
		type = NeighborhoodFuncType::Basic;
		// no break;
	case NeighborhoodFuncType::Basic:
		m_NeighborhoodFunc = &SimPhase::BasicCellFunc;
	}
	m_NeighborhoodFuncType = type;
}

void SimPhase::InitPhase(SimState *) {
//	auto ws = ss->GetSize();
//	unsigned cc = ws[0] * ws[1];
}

//--------------------------------- Script fun ---------------------------------

PhaseStep* SimPhase::AddStep(LuaApi::TableDispatcher &t) {
	const char *Name = t.getString("Name", "NoName");
	const char *CellFunc = t.getString("CellFunc", "");
	const char *OnBegin = t.getString("OnBegin", "");
	const char *OnEnd = t.getString("OnEnd", "");
	char buf[1024] = { };
	sprintf(buf, "(%s, %s, %s|%s)", Name, CellFunc, OnBegin, OnEnd);
	cout << "Phase '" << m_Name << "': Adding new step: '" << buf << "'\n";
	m_Steps.push_back(PhaseStep(Name, CellFunc, OnBegin, OnEnd));
	return &m_Steps.back();
}

void SimPhase::SetOnPhaseBegin(const char* FunName) {
	cout << "Phase '" << m_Name << "': Changing OnPhaseBegin to script function '" << FunName<< "'\n";
	SimState::CreateCallTo<>(m_OnPhaseBegin, FunName);
}

void SimPhase::SetOnPhaseEnd(const char* FunName) {
	cout << "Phase '" << m_Name << "': Changing OnPhaseEnd to script function '" << FunName<< "'\n";
	SimState::CreateCallTo<>(m_OnPhaseEnd, FunName);
}

void SimPhase::SetOnClick(const char* FunName) {
	cout << "Phase '" << m_Name << "': Changing OnClick to script function '" << FunName<< "'\n";
	m_OnClick = SimState::CreateCallTo<const SimPhase*, const glm::uvec2*, int>(FunName);
}

void SimPhase::SetDuration(float f) {
	cout << "Phase '" << m_Name << "': Changing duration to " << f << "\n";
	m_Duration = f;
}

void SimPhase::SetStepCount(int f) {
	cout << "Phase '" << m_Name << "': Changing step count to " << f << "\n";
	m_StepCount = f;
}

void SimPhase::SetStepLength(float f) {
	m_StepCount = m_Duration / f;
	cout << "Phase '" << m_Name << "': Changing step count to " << m_StepCount << "\n";
}

void SimPhase::SetNeighborhoodMode(int Mode) {
	NeighborhoodMode type = NeighborhoodModeMap::Cast(Mode);
	if(type == NeighborhoodModeMap::GetDefaultValue()) {
		type = NeighborhoodMode::Default;
		cout << "Phase '" << m_Name << "': Unknown neighborhood mode. Defaulting to '" << NeighborhoodModeMap::ToString(type) << "'\n";
	}
	cout << "Phase '" << m_Name << "': Changing neighborhood mode to '" << NeighborhoodModeMap::ToString(type) << "'\n";
	SetNeighborhoodModeFunc(type);
}

void SimPhase::SetNeighborhoodFuncType(int Mode) {
	NeighborhoodFuncType type = NeighborhoodFuncTypeMap::Cast(Mode);
	if(type == NeighborhoodFuncTypeMap::GetDefaultValue()) {
		type = NeighborhoodFuncType::Basic;
		cout << "Phase '" << m_Name << "': Unknown neighborhood func type. Defaulting to '" << NeighborhoodFuncTypeMap::ToString(type) << "'\n";
	}
//	else
//		cout << "Phase '" << m_Name << "': Changing neighborhood func type to '" << NeighborhoodFuncTypeMap::ToString(type) << "'\n";
	SetNeighborhoodFunc(type);
}

void SimPhase::SetBoundaryMode(int Mode) {
	BoundaryMode type = BoundaryModeMap::Cast(Mode);
	if(type == BoundaryModeMap::GetDefaultValue()) {
		type = BoundaryMode::Default;
		cout << "Phase '" << m_Name << "': Unknown boundary mode. Defaulting to '" << BoundaryModeMap::ToString(type)  << "'\n";
	} else
		cout << "Phase '" << m_Name << "': Changing neighborhood func type to '" << BoundaryModeMap::ToString(type) << "'\n";

	switch(type) {
	case BoundaryMode::Cutted:
		m_BoundaryFunc = &SimPhase::CuttedNeighborhoodTranslation;
		break;
	case BoundaryMode::Bounced:
		m_BoundaryFunc = &SimPhase::BouncedNeighborhoodTranslation;
		break;
	case BoundaryMode::Unknown:
		type = BoundaryMode::Circled;
		/* no break */
	case BoundaryMode::Circled:
		m_BoundaryFunc = &SimPhase::CircledNeighborhoodTranslation;
		break;
	}
	m_BoundaryMode = type;
}

void SimPhase::IgnoreInactive(bool value) {
	cout << "Phase '" << m_Name << "': Changing ignore inactive cell state to " << std::boolalpha << value << "\n";
	m_IgnoreInactive = value;
}

//--------------------------------- Main Logic ---------------------------------

SimPhaseStepResult SimPhase::ProcessStep(SimState* ss) const {
	if(!m_NeighborhoodFunc || !m_NeighborhoodModeFunc) {
		cerr << "Phase '" << m_Name << "': Neighborhood functions were not set! Cannot continue!\n";
		return SimPhaseStepResult::CriticalError;
	}

	PhaseStepCommonData pscd;
	pscd.State = ss;
	pscd.WorldSize = ss->GetSize();
	pscd.FirstWorld = ss->GetFirstWorld();
	pscd.SecondWorld = ss->GetSecondWorld();

	ExecutionData ed;

	glm::uvec2 &WSize = pscd.WorldSize;
	glm::uvec2 &Pos = ed.CellCoords;
	for(Pos[0] = 0; Pos[0] < WSize[0]; ++Pos[0]) {
		ed.LinearCoord = Pos[0];
		for(Pos[1] = 0; Pos[1] < WSize[1]; ++Pos[1], ed.LinearCoord += WSize[1]) {
			ss->GetCellVector(ed.LinearCoord).clear();
			(this->*m_BoundaryFunc)(pscd, ed);
		}
	}

	for(int step = 0, steps = m_Steps.size(); step < steps; ++step) {
		const PhaseStep &ph = m_Steps[step];
		pscd.Step = &ph;
		if(!ph.GetOnBegin().empty())
			ss->MakeCall(ph.GetOnBegin(), this);

		ss->GetLuaContext()->GetFunction(ph.GetCellFunc().c_str());
		if(!ph.GetCellFunc().empty()) {
			pscd.CellFuncName = ph.GetCellFunc().c_str();
			for(Pos[0] = 0; Pos[0] < WSize[0]; ++Pos[0]) {
				ed.LinearCoord = Pos[0];
				for(Pos[1] = 0; Pos[1] < WSize[1]; ++Pos[1], ed.LinearCoord += WSize[1]) {
					ed.Second = pscd.SecondWorld->at(ed.LinearCoord);
					ed.First = pscd.FirstWorld->at(ed.LinearCoord);
					ed.Cells = &ss->GetCellVector(ed.LinearCoord);
					SimPhaseStepResult r = (this->*m_NeighborhoodFunc)(pscd, ed);
					if(r != SimPhaseStepResult::Ok) {
						cerr << "Phase '" << m_Name << "': NeighborhoodFunc error " << (int)r << " at  (" << Pos[0] << ", " << Pos[1] << ")\n";
					}
				}
			}
		}
		if(!ph.GetOnEnd().empty())
			ss->MakeCall(ph.GetOnEnd(), this);
	}
	return SimPhaseStepResult::Ok;
}

//--------------------------------- Boundary Mode Func ---------------------------------

void SimPhase::CircledNeighborhoodTranslation(PhaseStepCommonData& Commondata, ExecutionData & ExecData) const {
	const IVec2Vector& NeighborhoodVec = (m_NeighborhoodModeFunc)(Commondata);
	glm::ivec2 WSize(Commondata.WorldSize);

	auto &CellVector = Commondata.State->GetCellVector(ExecData.LinearCoord);
	for(unsigned i = 0, j = NeighborhoodVec.size(); i < j; ++i) {
		glm::ivec2 pos = NeighborhoodVec[i];
		pos += ExecData.CellCoords;

		if(pos[0] < 0) pos[0] += WSize[0];
		else
			if(pos[0] >= WSize[0]) pos[0] -= WSize[0];
		if(pos[1] < 0) pos[1] += WSize[1];
		else
			if(pos[1] >= WSize[1]) pos[1] -= WSize[1];

		auto cell = Commondata.FirstWorld->at(Commondata.State->LinarPosition(pos));
		if(m_IgnoreInactive && cell->State == 0) continue;
		CellVector.Add(cell, pos);
	}
}

void SimPhase::CuttedNeighborhoodTranslation(PhaseStepCommonData& Commondata, ExecutionData & ExecData) const {
	const IVec2Vector& NeighborhoodVec = (m_NeighborhoodModeFunc)(Commondata);
	glm::ivec2 WSize(Commondata.WorldSize);
	auto &CellVector = Commondata.State->GetCellVector(ExecData.LinearCoord);
	for(unsigned i = 0, j = NeighborhoodVec.size(); i < j; ++i) {
		glm::ivec2 pos = NeighborhoodVec[i];
		pos += ExecData.CellCoords;

		if(pos[0] < 0 || pos[0] >= WSize[0]) continue;
		if(pos[1] < 0 || pos[1] >= WSize[1]) continue;

		auto cell = Commondata.FirstWorld->at(Commondata.State->LinarPosition(pos));
		if(m_IgnoreInactive && cell->State == 0) continue;
		CellVector.Add(cell, pos);
	}
}

void SimPhase::BouncedNeighborhoodTranslation(PhaseStepCommonData& Commondata, ExecutionData & ExecData) const {
	CuttedNeighborhoodTranslation(Commondata, ExecData);
}

//--------------------------------- Neighborhood Func ---------------------------------

SimPhaseStepResult SimPhase::BasicCellFunc(PhaseStepCommonData& data, ExecutionData & ed) const {
	data.State->GetLuaContext()->Call(&ed, ed.First, ed.Second, ed.Cells);
	return SimPhaseStepResult::Ok;
}

SimPhaseStepResult SimPhase::NullCellFunc(PhaseStepCommonData& data, ExecutionData & ed) const {
	data.State->GetLuaContext()->Call(data.CellFuncName, &ed, ed.First, ed.Second);
	return SimPhaseStepResult::Ok;
}

//--------------------------------- Neighborhood Mode ---------------------------------
#include "Neighborhood.h"

const IVec2Vector& SimPhase::PentagonalNeighborhoodMode(PhaseStepCommonData&) {
	return *Neighborhood::Pentagonal[rand() & 0x3];
}

const IVec2Vector& SimPhase::HexLeftNeighborhoodMode(PhaseStepCommonData&) {
	return Neighborhood::HexagonalLeftInstance;
}

const IVec2Vector& SimPhase::HexRightNeighborhoodMode(PhaseStepCommonData&) {
	return Neighborhood::HexagonalRightInstance;
}

const IVec2Vector& SimPhase::HexRandomNeighborhoodMode(PhaseStepCommonData&) {
	if(rand() & 1) return Neighborhood::HexagonalLeftInstance;
	else return Neighborhood::HexagonalRightInstance;
}

const IVec2Vector& SimPhase::MooreNeighborhoodMode(PhaseStepCommonData&) {
	return Neighborhood::MooreInstance;
}

const IVec2Vector& SimPhase::NeumanNeighborhoodMode(PhaseStepCommonData&) {
	return Neighborhood::NeumanInstance;
}
