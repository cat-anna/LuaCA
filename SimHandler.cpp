/*
 * SimHandler.cpp
 *
 *  Created on: 11 maj 2014
 *      Author: Paweu
 */

#include "Simulation.h"
#include "Front.h"

#include <boost/interprocess/sync/scoped_lock.hpp>

SimHandler::SimHandler() {
	m_Simulation = new Simulation();
	m_PerfString = "??";
	m_StateString = "??";
}

SimHandler::~SimHandler() {
	m_Thread.interrupt();
	m_Thread.join();
	delete m_SimState;
	delete m_Simulation;
	glDeleteTextures(1, &m_Texture);
}

void SimHandler::RegenStateString(SimState* ss) {
	char buffer[128];
	auto phase = ss->CurrentPhase();
	auto blender = m_PhaseBlender.get();
	sprintf(buffer, "Simulation: '%s'    Phase: '%s'   View:'%s'",
			m_Simulation->GetName().c_str(),
			(phase ? phase->GetName().c_str() : "?"),
			(blender ? blender->GetParent()->GetName().c_str() : "?"));
	m_StateString = buffer;
}

int SimHandler::Initialize(SharedLuaContext Lua) {
	int ec;
	ec = m_Simulation->Initialize(Lua);
	if (ec)
		throw "Unable to load simulation!";
	m_SimState = m_Simulation->CreateSimulation(Lua);

	if (!m_SimState)
		throw "Unable to create simulation context!";

	m_SimState->SetOnNextPhase([this] (SimState *ss) {
		RegenStateString(ss);
	});
	SetBlenderIndex(0);
	m_SimState->Initialize();
	m_VectorTexture.resize(m_SimState->LinarSize());

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glm::uvec2 WorldSize(m_Simulation->GetWorldSize());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WorldSize[0], WorldSize[1], 0,
	GL_RGB, GL_FLOAT, &m_VectorTexture[0]);

	m_DoStep = false;
	m_Updated = false;
	m_Thread = boost::thread([this] () {ThreadMain();});
	return 0;
}

void SimHandler::Finalize() {
	m_Thread.interrupt();
	m_Thread.join();
}

void SimHandler::SetBlenderIndex(unsigned index) {
	m_BlenderIndex = index;
	m_Changed = true;
	if (m_BlenderIndex >= m_Simulation->BlenderCount())
		m_BlenderIndex = 0;
	m_PhaseBlender = m_Simulation->GetBlender(m_BlenderIndex)->GetInstance();
	RegenStateString(m_SimState);
}

void SimHandler::ThreadMain() {
	try {
		while (true) {
			try {
				while(!m_ActionQueue.empty()) {
					boost::mutex::scoped_lock l1(m_ActionMutex);
					m_ActionQueue.front()();
					m_ActionQueue.pop_front();
				}
				if(!m_DoStep) {
					boost::this_thread::interruption_point();
					boost::this_thread::sleep_for(
							boost::chrono::milliseconds(10));
					continue;
				}
				{
					boost::mutex::scoped_lock l1(m_mutex);
					m_DoStep = false;
					ProcessStep();
					m_Changed = true;
				}
			} catch (boost::thread_interrupted &) {
				throw;
			}catch (std::exception &e) {
				cerr << "Thread critical error: " << e.what() << "\n";
			}
			catch (...) {
				cerr << "Unknown thread critical error!\n";
			}
		}
	} catch (...) {
	}
}

void SimHandler::RegenColors() {
	if (!m_Changed)
		return;

	boost::mutex::scoped_lock l(m_TexMutex);

	m_Changed = false;
	m_PhaseBlender->InitStep();
	unsigned size = m_SimState->LinarSize();
	for (unsigned i = 0; i < size; ++i)
//		m_VectorTexture[i] = glm::vec3(i / (float)size);
		m_PhaseBlender->ProcessColor(m_VectorTexture[i], m_SimState->Cell(i));

	m_Updated = false;
}

void SimHandler::ProcessStep() {
	double ct = glfwGetTime();

	if (m_SimState->DoStep() == SimStateStepResult::Finished) {
		cout << "SIMULATION FINISHED!\n";
		App::cApplication::Instance()->SetAlarm(0);
	}
	RegenColors();

	m_LastCycleTime = (glfwGetTime() - ct) * 1000.0f;

	char buffer[128];
	sprintf(buffer, "Time: %.2f  StepTime: %.1fms  Lua:%.2fkb",
			m_SimState->GetCurrentTime(), m_LastCycleTime,
			m_SimState->GetLuaContext()->GetMemUsage() / 1024.0f);
	m_PerfString = buffer;
}

void SimHandler::HandleClick(const glm::uvec2 &coord, int Button) {
	auto phase = m_SimState->CurrentPhase();
	if (!phase)
		return;
	glm::uvec2 c(coord);
	boost::mutex::scoped_lock l(m_ActionMutex);
	m_ActionQueue.push_back([this, c, Button]() {
		m_SimState->CurrentPhase()->OnClick(m_SimState, c, Button);
		InvalidateColors();
	});
}
