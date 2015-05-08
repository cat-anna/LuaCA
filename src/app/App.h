#ifndef GameAppH
#define GameAppH
/*
bool inline TestFlags(unsigned src, unsigned flags){
	return (src & flags) == flags;
}

void inline SetFlags(unsigned &src, unsigned flags, bool set){
	if(set)	src |= flags;
	else src &= ~flags;
}*/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

#include "cRenderDev.h"
#include "fonts.h"

#include <Front.h>

namespace App {

struct App_exit_success {};

class cApplication {
	static cApplication* _Instance;
protected:
	GLFWwindow *m_Window;
	volatile unsigned m_AppIsRunning;
	volatile unsigned m_Flags;
	unsigned m_GoalFPS;
	
	cRenderDev m_RenderDev;

	int m_LastFPS;
	int m_FPSCounter, m_CurrAlarmTicks;
	float m_LastRenderTime;
	float m_LastAlarmTime;
	float m_AlarmDelay;
	int m_AlarmCounter, m_LastAlarmTicks;
	
	void ResizeOpenGL(int Width, int Height);

	void Render();
	void Initialize();
	void Finalize();
	
	std::vector<std::string> m_Params;
	
	static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void glfw_close_callback(GLFWwindow* window);
	static void glfw_focus_callback(GLFWwindow* window, int focus);
	static void glfw_mouse_callback(GLFWwindow *window, int button, int action, int mods);
public:
	cApplication(int argc, char **argv);
	~cApplication();

	int Run();
	
	void SetFlags(unsigned Flags, bool state);
	
	void SetTitle(const string& Name);

	void SetAlarm(unsigned TicksPerSec);

	GLuint LoadTexture(const char *TexFile);
	cBitmapFont* LoadFont(const string& BFDFile, const string& TexFile);

	static cApplication* Instance() { return _Instance; }
	static void DeleteInstance();
};

}//namespace App

#endif
