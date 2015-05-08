/*
 ) * cChartStoreFront.cpp
 *
 *  Created on: 14-12-2013
 *      Author: Paweu
 */

#include <vector>
#include <string>
#include <map> 
#include "Front.h"
#include "ApiGenerator.h"

Front* Front::_Instance;

const char* PausedTxt = "PAUSED";
const char* HelpTxt = "Press F1 for help";
const char* RetHelpTxt = "Press F1 to return";

glm::uvec2 LTMargins(30, 30);
glm::uvec2 BRMargins(LTMargins);

Front::Front() :
		m_Font(0), m_StateFlags(0), m_UpperHelp(), m_LowerHelp() {
	if (_Instance)
		throw "Front instance already exists!";
	_Instance = this;

	m_Font = App::cApplication::Instance()->LoadFont("Arial.bfd", "Arial.tga");

	m_UpperHelp.push_back("LuaSim v0.1");
	m_UpperHelp.push_back("");
	m_UpperHelp.push_back("Help:");
	m_UpperHelp.push_back("Keys:");
	m_UpperHelp.push_back("\t1-9 - Select view ");
//	m_UpperHelp.push_back("\tR - Reset");
	m_UpperHelp.push_back("\tp - Pause");
	m_UpperHelp.push_back("\ts - Make step (works only when paused)");
	m_UpperHelp.push_back("\tUp/Down - Increase/Decrease cycles per second");
	m_UpperHelp.push_back("\tEsc - Exit");

	m_CyclesPerSec = 10;
	m_CellSize = 10;
}

Front::~Front() {
	delete m_Font;
}

int Front::Initialze(const std::vector<std::string> &Params) {
	std::vector<std::string> Scripts;

	for (auto &i : Params) {
		if (i == "--print-api") {
			ApiGenerator::PrintApi(std::cout);
			throw App::App_exit_success();
		}
	}

	SharedLuaContext Lua = SharedLuaContext(new LuaApi::LuaContext());
	if (!Lua->InitContext()) {
		cerr << "Unable to initialize LUA context!\n";
		return 1;
	}

	for (auto &i : Params) {
		static const int execlen = strlen("--exec=");
		if (!memcmp(i.c_str(), "--exec=", execlen)) {
			std::string code = i.substr(execlen, i.length() - execlen);
			Lua->LoadScriptCode(code.c_str());
			continue;
		}
		Lua->LoadScript(i.c_str());
		continue;
	}

	m_SimHandler.Initialize(Lua);
	App::cApplication::Instance()->SetAlarm(m_CyclesPerSec);
	return 0;
}

void Front::OnAlarm() {
	if (TestFlag(StateFlags::Paused) || TestFlag(StateFlags::HelpMode)) {
		if (TestFlag(StateFlags::StepMode)) {
			SetFlag(StateFlags::StepMode, false);
		} else {
			m_SimHandler.InvalidateColors();
			return;
		}
	}
	m_SimHandler.DoStep();
}

void Front::OnResize(cRenderDev& dev) {
	glm::uvec2 Screen(dev.GetW(), dev.GetH());
	glm::uvec2 Board(Screen);

	Board -= LTMargins;
	Board -= BRMargins;

	glm::vec2 Field(Board);
	glm::vec2 WorldSize(m_SimHandler.GetSimulation()->GetWorldSize());

	Field /= WorldSize;

	float cell = glm::min(Field[0], Field[1]);

	m_CellSize = static_cast<unsigned>(cell);

	m_BoardMargin = Screen;
	m_BoardMargin -= m_SimHandler.GetSimulation()->GetWorldSize() * m_CellSize;
	m_BoardMargin /= 2;
}

void Front::DoRender(float/* FrameTime*/, cRenderDev& dev) {
	dev.SetOrthoMode();
	glLoadIdentity();
	glColor3f(1, 1, 1);
	if (TestFlag(StateFlags::HelpMode)) {
		int y = 100;
		for (auto & i : m_UpperHelp) {
			m_Font->Draw(i.c_str(), 0, vec3(100, y, 0));
			y += (unsigned) (m_Font->FontHeight() * 1.5f);
		}
		y = dev.GetH() - 100;
		for (auto i = m_LowerHelp.rbegin(), j = m_LowerHelp.rend(); i != j;
				++i) {
			m_Font->Draw(i->c_str(), 0, vec3(100, y, 0));
			y -= (unsigned) (m_Font->FontHeight() * 1.5f);
		}
		m_Font->Draw(RetHelpTxt, 0,
				vec3(dev.GetW() - m_Font->TextWidth(RetHelpTxt, 0), 5, 0));
		return;
	}

	m_Font->Draw(HelpTxt, 0,
			vec3(dev.GetW() - m_Font->TextWidth(HelpTxt, 0), 5, 0));
	m_Font->Draw(m_SimHandler.GetStateString().c_str(), 0, vec3(5, 5, 0));
	m_Font->Draw(m_SimHandler.GetPerfString().c_str(), 0,
			vec3(5, 5 + m_Font->FontHeight(), 0));

	if (TestFlag(StateFlags::Paused))
		m_Font->Draw(PausedTxt, 0,
				vec3(dev.GetW() / 2 - m_Font->TextWidth(PausedTxt, 0) / 2, 5,
						0));

	glLoadIdentity();
	glTranslatef(m_BoardMargin[0], m_BoardMargin[1], 0);

	glm::uvec2 WorldSize(m_SimHandler.GetSimulation()->GetWorldSize());

	glColor3f(1, 0, 0);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINE_STRIP);
	glVertex2i(-1, -1);
	glVertex2i(m_CellSize * WorldSize[0] + 2, -1);
	glVertex2i(m_CellSize * WorldSize[0] + 2, m_CellSize * WorldSize[1] + 2);
	glVertex2i(-1, m_CellSize * WorldSize[1] + 2);
	glVertex2i(-1, -1);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	m_SimHandler.FlushTexture();
	glBindTexture(GL_TEXTURE_2D, m_SimHandler.GetTexture());

	glScalef(m_CellSize, m_CellSize, 1);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2i(0, 0);
	glTexCoord2f(0, 1); glVertex2i(0, WorldSize[1]);
	glTexCoord2f(1, 1); glVertex2i(WorldSize[0], WorldSize[1]);
	glTexCoord2f(1, 0); glVertex2i(WorldSize[0], 0);
	glEnd();
}

void Front::RegenLowerHelp() {
	auto mi = m_SimHandler.GetState()->QuerryStatusInfo();
	char buf[128] = { };
	m_LowerHelp.clear();

	sprintf(buf, "Steps done: %d", mi.StepsDone);
	m_LowerHelp.push_back(buf);

	sprintf(buf, "Lua context size: %.2f kb", mi.LuaSize / 1024.0f);
	m_LowerHelp.push_back(buf);

	sprintf(buf, "Parameter count: %d     Cell size %d bytes", mi.ParameterCount, mi.CellSize);
	m_LowerHelp.push_back(buf);

//	sprintf(buf, "Lua context size: %.2f kb", mi.LuaSize / 1024.0f);
//	m_LowerHelp.push_back(buf);

	m_LowerHelp.push_back("");
	m_LowerHelp.push_back("Build on " __DATE__ " at " __TIME__);
}

void Front::CustomerTouch(unsigned key) {
	if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
		int id = key - GLFW_KEY_0;
		--id;
		if (id < 0) id = 10;
		if (id >= (int) m_SimHandler.GetSimulation()->BlenderCount()) return;
		m_SimHandler.SetBlenderIndex(id);
		return;
	}

	switch (key) {
	case GLFW_KEY_R: //reset
		//m_CurrentChart->Reset();
		return;

	case GLFW_KEY_P:
		SetFlag(StateFlags::Paused, !TestFlag(StateFlags::Paused));
		return;

	case GLFW_KEY_S:
		SetFlag(StateFlags::StepMode, true);
		return;

	case GLFW_KEY_UP:
		if (m_CyclesPerSec < 100)
			++m_CyclesPerSec;
		App::cApplication::Instance()->SetAlarm(m_CyclesPerSec);
		break;
	case GLFW_KEY_DOWN:
		if (m_CyclesPerSec > 0)
			--m_CyclesPerSec;
		App::cApplication::Instance()->SetAlarm(m_CyclesPerSec);
		break;
	case GLFW_KEY_LEFT:
		break;
	case GLFW_KEY_RIGHT:
		break;

	case GLFW_KEY_F1: { //help
		SetFlag(StateFlags::HelpMode, !TestFlag(StateFlags::HelpMode));
		RegenLowerHelp();
		break;
	}
	}
}

void Front::DeleteInstance() {
	delete _Instance;
	_Instance = 0;
}

void Front::HandleClick(glm::uvec2 coord, int Button) {
	glm::uvec2 WorldSize(m_SimHandler.GetSimulation()->GetWorldSize());
	coord -= m_BoardMargin;
	coord /= m_CellSize;
	m_SimHandler.HandleClick(coord, Button + 1);
}

