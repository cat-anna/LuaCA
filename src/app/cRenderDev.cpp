/*
 * cRenderDev.cpp
 *
 *  Created on: 12-12-2013
 *      Author: Paweu
 */

#include "cRenderDev.h"

namespace Graphic {

cRenderDev::cRenderDev(int Width, int Height): m_PerspectiveMode(true), m_Width(Width), m_Height(Height) {
	// TODO Auto-generated constructor stub

}

cRenderDev::~cRenderDev() {
	// TODO Auto-generated destructor stub
}

void cRenderDev::SetOrthoMode() {
	if(!m_PerspectiveMode) return;
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, m_Width, m_Height, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	m_PerspectiveMode = false;
}

void cRenderDev::SetPerspectiveMode() {
	if(m_PerspectiveMode) return;
	glViewport(0,0,m_Width, m_Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)m_Width/(GLfloat)m_Height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glLoadIdentity();
	m_PerspectiveMode = true;
}

} /* namespace Graphic */
