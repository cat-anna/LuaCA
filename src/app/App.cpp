#include "App.h"

#include <ctime>
#include <FreeImage.h>
#include <cassert>
#include <fstream>

namespace App {

static void glfw_error_callback(int error, const char* description) {
	cerr << "GLFW error: code=" << error << " descr='" << description << "'";
}

cApplication* cApplication::_Instance;

cApplication::cApplication(int argc, char **argv) :
		m_Window(), m_AppIsRunning(0), m_Flags(), m_RenderDev(800, 600) {
	if (_Instance)
		throw "cApplication instance already exists!";
	_Instance = this;

	srand(time(NULL));

	m_LastFPS = 60; //min goal
	m_GoalFPS = 60;
	m_FPSCounter = 0;
	m_AlarmDelay = m_LastRenderTime = 0;
	m_Flags = 0;
	m_LastAlarmTime = 0;
	m_CurrAlarmTicks = 0;

	for (int i = 1; i < argc; ++i)
		m_Params.push_back(argv[i]);

//	if(argc > 1) {
//		std::string arg = argv[1];
//
//		if(arg == "--print-api") {
//			m_Params["work-mode"] = "print-api";
//		} else {
//			m_Params["work-mode"] = "simulation";
//			m_Params["script"].swap(arg);
//		}
//	}

	glfwSetErrorCallback(&glfw_error_callback);
	if (!glfwInit())
		throw "!glfw";
}

cApplication::~cApplication() {
}

void cApplication::SetTitle(const string& Name) {
	string title = "LuaSim";
	if (!Name.empty()) {
		title += ": ";
		title += Name;
	}
	glfwSetWindowTitle(m_Window, title.c_str());
}

void cApplication::DeleteInstance() {
	delete _Instance;
	_Instance = 0;
}

void cApplication::SetAlarm(unsigned TicksPerSec) {
	if (TicksPerSec == 0) {
		m_AlarmDelay = 0;
		return;
	}
	m_AlarmDelay = 1.0f / static_cast<unsigned>(TicksPerSec);
}

void cApplication::SetFlags(unsigned Flags, bool Enable) {
	if (Enable)
		m_Flags |= Flags;
	else
		m_Flags &= ~Flags;
}

void cApplication::Initialize() {

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	m_Window = glfwCreateWindow(m_RenderDev.GetW(), m_RenderDev.GetH(),
			"Window", 0, 0);
	if (!m_Window)
		throw "!Window";
	glfwMakeContextCurrent(m_Window);
	if (glewInit() != GLEW_OK)
		throw "!glew";

	glfwSwapInterval(1);
	glfwSetWindowUserPointer(m_Window, this);
	glfwSetKeyCallback(m_Window, glfw_key_callback);
	glfwSetWindowCloseCallback(m_Window, glfw_close_callback);
	glfwSetMouseButtonCallback(m_Window, glfw_mouse_callback);
	glfwSetWindowFocusCallback(m_Window, glfw_focus_callback);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.4f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	try {
		new Front();
		int ec = Front::Instance()->Initialze(m_Params);
		if (ec != 0)
			throw ec;
		SetTitle("");
		m_AppIsRunning = 1;
	} catch (int ec) {
		std::cerr << "Unable to load simulation! code:" << ec << "\n";
		//throw  ec;
	} catch (const App_exit_success &) {
	} catch (...) {

	}
}

void cApplication::Finalize() {
	Front::DeleteInstance();
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

#define YeldFlag(F, V) AddLog("Flag '" #F "' val: " << std::boolalpha << ((V & F) > 0))

void cApplication::glfw_key_callback(GLFWwindow* window, int key, int/* scancode*/,
		int action, int/* mods*/) {
	if (action == GLFW_REPEAT)
		return;
	cApplication* app = ((cApplication*) glfwGetWindowUserPointer(window));
	bool press = action == GLFW_PRESS;
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (!press)
			return;
		app->m_AppIsRunning = 0;
		return;
	}
	if (press)
		Front::Instance()->CustomerTouch(key);
}

void cApplication::glfw_close_callback(GLFWwindow* window) {
	cApplication* app = ((cApplication*) glfwGetWindowUserPointer(window));
	app->m_AppIsRunning = 0;
}

void cApplication::glfw_mouse_callback(GLFWwindow *window, int button,
		int action, int /*mods*/) {
	if(action != GLFW_PRESS) return;
//	cApplication* app = ((cApplication*)glfwGetWindowUserPointer(window));
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	glm::uvec2 pos((unsigned)x, (unsigned)y);
	Front::Instance()->HandleClick(pos, button);
}

void cApplication::glfw_focus_callback(GLFWwindow* /*window*/, int /*focus*/) {
//	cApplication* app = ((cApplication*)glfwGetWindowUserPointer(window));
}

void cApplication::Render() {
}

int cApplication::Run() {
	Initialize();
	if (!m_AppIsRunning)
		return 0;
	double flt = glfwGetTime();
	double rlt = flt;
	double fw = 1 / static_cast<float>(m_GoalFPS);
	float LastAlarm = 0;
	int ldfps = 0;
	Front::Instance()->OnResize(m_RenderDev);
	while (m_AppIsRunning) {
		glfwPollEvents();

		double ct = glfwGetTime();
		if (ct - flt >= 1.0) {
			flt = ct;
			m_LastFPS = m_FPSCounter;
			m_FPSCounter = 0;
			m_LastAlarmTicks = m_CurrAlarmTicks;
			m_CurrAlarmTicks = 0;
			int dfps = m_GoalFPS - m_LastFPS;
			int dabs = abs(dfps);
			if (dabs > 5 && ldfps < 6)
				dfps = 0;
			fw -= (dfps * 0.0001f) * ((ldfps == 0 && dabs == 1) ? 0.1f : 1.0f);
			if (fw < 0)
				fw = 0;
			ldfps = dabs;
		}
		if (ct - rlt >= fw) {
			rlt = ct;
			++m_FPSCounter;
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();
			glColor3f(0, 0, 0);
			Front::Instance()->DoRender(fw, m_RenderDev);
			glfwSwapBuffers(m_Window);
		}
		if (m_AlarmDelay > 0) {
			ct = glfwGetTime();
			if (ct - LastAlarm > m_AlarmDelay) {
				++m_CurrAlarmTicks;
				LastAlarm = ct;
				Front::Instance()->OnAlarm();
				m_LastAlarmTime = glfwGetTime() - ct;
			}
		}
		boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
	}
	Finalize();
	return 0;
}

cBitmapFont* cApplication::LoadFont(const string& BFDFile,
		const string& TexFile) {
	ifstream inp(BFDFile.c_str(), ios::in | ios::binary);
	inp.seekg(0);
	if (!inp)
		throw "!Font inp";
	inp.seekg(0, ios::end);
	int len = inp.tellg();
	inp.seekg(0);
	char *data = new char[len + 1];
	inp.readsome(data, len);
//	if(inp.readsome(data, len) != len) throw "!bfdsize";
	inp.close();
	auto b = new cBitmapFont(data, LoadTexture(TexFile.c_str()));
	delete data;
	return b;
}

void RGBToBGR(FIBITMAP *dib) {
	BYTE *bits = FreeImage_GetBits(dib);
	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);

	for (int i = 0, j = height * width; i < j; ++i) {
		BYTE b = bits[0];
		bits[0] = bits[2];
		bits[2] = b;
		bits += 3;
	}
}

void RGBAToBGRA(FIBITMAP *dib) {
	BYTE *bits = FreeImage_GetBits(dib);
	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);

	for (int i = 0, j = height * width; i < j; ++i) {
		BYTE b = bits[0];
		bits[0] = bits[2];
		bits[2] = b;
		bits += 4;
	}
}

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	cerr << "FreeImage [Format: " << FreeImage_GetFormatFromFIF(fif)
			<< "] error: " << message << endl;
}

bool FILoadTexture(const void* Data, unsigned DataLen, GLint MipmapLevel) {
	FIBITMAP *dib = 0;
	FreeImage_SetOutputMessage(FreeImageErrorHandler);

	FIMEMORY *fim = FreeImage_OpenMemory((BYTE*) Data, DataLen);
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(fim);
	dib = FreeImage_LoadFromMemory(fif, fim, 0);
	FreeImage_CloseMemory(fim);

	if (!dib)
		return false;

	BYTE* bits(0);
	unsigned int width(0), height(0);
	GLuint ImgType;

	FreeImage_FlipVertical(dib);
	//	FreeImage_FlipHorizontal(dib);
	switch (FreeImage_GetBPP(dib)) {
	case 32:
		RGBAToBGRA(dib);
		ImgType = GL_RGBA;
		break;
	case 24:
		RGBToBGR(dib);
		ImgType = GL_RGB;
		break;
	default: {
		FIBITMAP *dib24 = FreeImage_ConvertTo24Bits(dib);
		FreeImage_Unload(dib);
		dib = dib24;
		RGBToBGR(dib);
		ImgType = GL_RGB;
	}
	}

	bits = FreeImage_GetBits(dib);
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	assert((bits != 0) && (width != 0) && (height != 0));
	glTexImage2D(GL_TEXTURE_2D, MipmapLevel, ImgType, width, height, 0, ImgType,
			GL_UNSIGNED_BYTE, bits);
	FreeImage_Unload(dib);
	return true;
}

GLuint cApplication::LoadTexture(const char *TexFile) {
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	ifstream inp(TexFile, ios::in | ios::binary);
	inp.seekg(0, ios::end);
	unsigned len = inp.tellg();
//	cerr << "LT " << TexFile << "  " << len << endl;
	inp.seekg(0);

	char *buf = new char[len + 1];
	inp.read(buf, len);
	inp.close();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if (!FILoadTexture(buf, len, 0)) {
		cerr << "Unable to load texture " << TexFile << endl;
	}
	delete buf;
	return tex;
}

} //namespace app
