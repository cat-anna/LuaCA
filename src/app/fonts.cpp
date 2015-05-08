#include "app.h"

#include <fstream>
#include <cstring>

namespace App {

cBitmapFont::cBitmapFont(void *bfd, Texture Tex){
	memcpy(&m_BFD, bfd, sizeof(m_BFD));
	m_Texture = Tex;
}

cBitmapFont::~cBitmapFont(){
//TODO: kill texture
}

float cBitmapFont::TextWidth(const char *Text, float height) const {
	if(!Text) return 0;
	float x = 0;
	float h = static_cast<float>(m_BFD.CharWidth), w;
	if(height > 0) h = height;
	w = h;
	float w_mult = w / static_cast<float>(m_BFD.CharWidth);
	unsigned delta = m_BFD.BeginingKey;
	while(*Text){
		if(*Text == '\t'){
			x += m_BFD.CharWidth * w_mult;
			++Text;
			continue;
		}
		unsigned kid = static_cast<unsigned>(*Text) - delta;
		x += (m_BFD.KeyWidths[kid] + 1) * w_mult;
		++Text;
	}
	return x;
}

float cBitmapFont::Draw(const char *Text, float height, const vec3 &Pos) const{
	if(!Text) return 0;

	float x = Pos[0], y = Pos[1]/*, z = Pos.z*/;
	float h = static_cast<float>(m_BFD.CharWidth), w;
	if(height > 0) h = height;
	w = h;
	float w_mult = w / static_cast<float>(m_BFD.CharWidth);
	
	unsigned fx = m_BFD.Width  / m_BFD.CharWidth;
//	unsigned fy = m_BFD.Height / m_BFD.CharHeight;
	float Cx = m_BFD.Width  / static_cast<float>(m_BFD.CharWidth);
	float Cy = m_BFD.Height / static_cast<float>(m_BFD.CharHeight);
	float dw = 1 / Cx;
	float dh = 1 / Cy;
	unsigned delta = m_BFD.BeginingKey;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBegin(GL_QUADS);	
	while(*Text){
		if(*Text == '\t'){
			x += m_BFD.CharWidth * w_mult;
			++Text;
			continue;
		}
		unsigned kid = static_cast<unsigned>(*Text) - delta;
		if(kid > 255) kid = fx;
		unsigned kol  = kid % fx;
		unsigned line = kid / fx;
		float u = kol  / Cx;
		float v = line / Cy;
		 
		glTexCoord2f(u + dw, v     );		glVertex2f(x + w, y    ); 
		glTexCoord2f(u     , v     );		glVertex2f(x    , y    ); 
		glTexCoord2f(u     , v + dh);		glVertex2f(x    , y + h); 
		glTexCoord2f(u + dw, v + dh);		glVertex2f(x + w, y + h); 

		x += (m_BFD.KeyWidths[kid] + 1) * w_mult;
		++Text;
	}
	glEnd();
	glPopAttrib();
	return x - Pos[0];
}

float cBitmapFont::DrawEx(const char *Text, float height, cColorCaller* ExFun, const vec3 &Pos) const{
	if(!Text || ! ExFun) return 0;

	float x = Pos[0], y = Pos[1]/*, z = Pos.z*/;
	float h = static_cast<float>(m_BFD.CharWidth), w;
	if(height > 0) h = height;
	w = h;
	float w_mult = w / static_cast<float>(m_BFD.CharWidth);
	
	unsigned fx = m_BFD.Width  / m_BFD.CharWidth;
//	unsigned fy = m_BFD.Height / m_BFD.CharHeight;
	float Cx = m_BFD.Width  / static_cast<float>(m_BFD.CharWidth);
	float Cy = m_BFD.Height / static_cast<float>(m_BFD.CharHeight);
	float dw = 1 / Cx;
	float dh = 1 / Cy;
	unsigned delta = m_BFD.BeginingKey;
//	typedef void(*ExFun_p)(void *Param,  unsigned Type)

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBegin(GL_QUADS);	
	ExFun->call(deBegin);
	while(*Text){
		if(*Text == '\t'){
			x += m_BFD.CharWidth * w_mult;
			++Text;
			continue;
		}
		unsigned kid = static_cast<unsigned>(*Text) - delta;
		if(kid > 255) kid = fx;
		unsigned kol  = kid % fx;
		unsigned line = kid / fx;
		float u = kol  / Cx;
		float v = line / Cy;
		ExFun->call(deNextChar);
		
		ExFun->call(deRT); glTexCoord2f(u + dw, v     );		glVertex2f(x + w, y    ); 
		ExFun->call(deLT); glTexCoord2f(u     , v     );		glVertex2f(x    , y    ); 
		ExFun->call(deLB); glTexCoord2f(u     , v + dh);		glVertex2f(x    , y + h); 
		ExFun->call(deRB); glTexCoord2f(u + dw, v + dh);		glVertex2f(x + w, y + h); 

		x += (m_BFD.KeyWidths[kid] + 1) * w_mult;
		++Text;
	}
	ExFun->call(deEnd);
	glEnd();
	glPopAttrib();
	return x - Pos[0];
}

cBitmapFont::cBFDHeader::cBFDHeader(){
	Width = Height = CharWidth = CharHeight = 0;
	BeginingKey = 0;
	memset(KeyWidths, 0, 256);
}

} //namespace app
