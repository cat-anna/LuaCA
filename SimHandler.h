/*
 * SimHandler.h
 *
 *  Created on: 11 maj 2014
 *      Author: Paweu
 */

#ifndef SIMHANDLER_H_
#define SIMHANDLER_H_

#include <boost/thread.hpp>
#include <boost/chrono.hpp>

class SimHandler {
public:
	SimHandler();
	~SimHandler();

	int Initialize(SharedLuaContext Lua);
	void Finalize();
	void DoStep() { m_DoStep = true; }
	void InvalidateColors() { m_Changed = true; }
	void SetBlenderIndex(unsigned index);

	void FlushTexture() {
		if(m_Updated || m_TexMutex.try_lock()) return;
		glBindTexture(GL_TEXTURE_2D, m_Texture);
		const glm::uvec2& WorldSize = m_Simulation->GetWorldSize();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WorldSize[0], WorldSize[1], 0,
				GL_RGB, GL_FLOAT, &m_VectorTexture[0]);
		m_Updated = true;
		m_TexMutex.unlock();
	}

	DefineREADAcces(StateString, std::string)
	DefineREADAcces(PerfString, std::string)
	DefineREADAcces(Texture, GLuint)

	void HandleClick(const glm::uvec2 &coord, int Button);

	Simulation *GetSimulation() const { return m_Simulation; }
	SimState *GetState() const { return m_SimState; }

	const glm::vec3& GetColorAt(unsigned i, unsigned j) const {
		return m_VectorTexture[m_SimState->LinarPosition(i, j)];
	}
private:
	std::string m_StateString, m_PerfString;
	float m_LastCycleTime;
	volatile bool m_DoStep, m_Changed, m_Updated;
	unsigned m_BlenderIndex;

	void ProcessStep();
	void RegenColors();

	Simulation *m_Simulation;
	SimState* m_SimState;
	std::shared_ptr<PhaseBlenderInstance> m_PhaseBlender;
	std::vector<glm::vec3> m_VectorTexture;
	GLuint m_Texture;

	boost::thread m_Thread;
	boost::mutex m_mutex;
	boost::mutex m_TexMutex, m_ActionMutex;
	void ThreadMain();
	void RegenStateString(SimState* ss);

	typedef std::function<void()> ActionFunction;
	std::list<ActionFunction> m_ActionQueue;
};

#endif // SIMHANDLER_H_
