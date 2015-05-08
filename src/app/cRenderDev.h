/*
 * cRenderDev.h
 *
 *  Created on: 12-12-2013
 *      Author: Paweu
 */

#ifndef CRENDERDEV_H_
#define CRENDERDEV_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
using namespace glm;

typedef GLuint Texture;

namespace Graphic {

class cRenderDev {
protected:
	bool m_PerspectiveMode;
	int m_Width, m_Height;
public:
	cRenderDev(int Width, int Height);
	~cRenderDev();

	void SetOrthoMode();
	void SetPerspectiveMode();

	int GetW() const { return m_Width; }
	int GetH() const { return m_Height; }
};

class cRenderList {
	GLuint m_ListHandle;
public:
	cRenderList(const cRenderList& lst) = delete;
	cRenderList(): m_ListHandle(0) { }
	~cRenderList(){ Clear(); };
	void Clear(){
		glDeleteLists(m_ListHandle, 1);
		m_ListHandle = 0;
	}
	void Begin(){
		if(m_ListHandle) Clear();
		m_ListHandle = glGenLists(1);
		glNewList(m_ListHandle, GL_COMPILE);
	}
	void End(){	glEndList(); }
	bool IsReady() const { return m_ListHandle != 0; };
	void Call() const { glCallList(m_ListHandle); };
};

} /* namespace Graphic */

using Graphic::cRenderDev;
using Graphic::cRenderList;

#endif /* CRENDERDEV_H_ */
