#ifndef id83A8DEBE_CA84_487B_AD63A63AECADC9F9
#define id83A8DEBE_CA84_487B_AD63A63AECADC9F9

namespace App {

class cBitmapFont {
	struct cBFDHeader {
		cBFDHeader();
		unsigned Width, Height, CharWidth, CharHeight;
		unsigned char BeginingKey, KeyWidths[256];
	};

	GLuint m_Texture;
	cBFDHeader m_BFD;
public:
	cBitmapFont(void *bfd, Texture Tex);
	~cBitmapFont();
	enum {
		deBegin,
		deNextChar,
		deRT, deLT, deLB, deRB,
		deEnd,
	};
	struct cColorCaller {
		virtual void call(unsigned mode) = 0;
	};
	int FontHeight() const { return m_BFD.CharHeight; }
	float DrawEx(const char *Text, float height, cColorCaller* ExFun, const vec3 &Pos = vec3(0,0,0)) const;
	float Draw(const char *Text, float height, const vec3 &Pos = vec3(0,0,0)) const;
	void GenText(cRenderList & list, const char *Text, float height, const vec3 &Pos = vec3(0,0,0)){
		list.Begin();
		Draw(Text, height, Pos);
		list.End();
	}
	float TextWidth(const char *Text, float height) const;
};

} //namespace app

#endif // header
