/*
 * cChartStoreFront.h
 *
 *  Created on: 14-12-2013
 *      Author: Paweu
 */

#ifndef CCHARTSTOREFRONT_H_
#define CCHARTSTOREFRONT_H_

#include <App.h>
#include "Simulation.h"
#include "SimHandler.h"

class Front {
public:
	Front();
	~Front();

	int Initialze(const std::vector<std::string> &Params);

	void OnResize(cRenderDev &dev);
	void DoRender(float FrameTime, cRenderDev &dev);
	void CustomerTouch(unsigned key);

	void OnAlarm();

	void HandleClick(glm::uvec2 coord, int Button);

	static Front* Instance() { return _Instance; }
	static void DeleteInstance();
private:
	static Front* _Instance;
	App::cBitmapFont *m_Font;
	SimHandler m_SimHandler;
	std::vector<std::string> m_UpperHelp, m_LowerHelp;

	unsigned m_CyclesPerSec;

	unsigned m_CellSize;
	glm::uvec2 m_BoardMargin;

	unsigned m_StateFlags;
	enum class StateFlags : unsigned{
		HelpMode 	= 0x00001,
		Paused 		= 0x00002,
		StepMode	= 0x00010,
	};
	bool TestFlag(StateFlags flag) const {
		return m_StateFlags & static_cast<unsigned>(flag);
	}
	void SetFlag(StateFlags flag, bool state) {
		if(state) m_StateFlags |= static_cast<unsigned>(flag);
		else m_StateFlags &= ~static_cast<unsigned>(flag);
	}

	void RegenLowerHelp();
};

#endif /* CCHARTSTOREFRONT_H_ */
