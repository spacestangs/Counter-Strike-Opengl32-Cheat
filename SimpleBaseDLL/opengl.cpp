#include "pch.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment(lib, "OpenGL32.lib")

typedef void(APIENTRY* glBegin_t)(GLenum);
typedef void(APIENTRY* glEnd_t)(void);
typedef void(APIENTRY* glVertex3fv_t)(const GLfloat* v);
typedef void(APIENTRY* glClear_t)(GLbitfield);

glBegin_t pglBegin = NULL;
glEnd_t pglEnd = NULL;
glVertex3fv_t pglVertex3fv = NULL;
glClear_t pglClear = NULL;

GLenum g_glMode;


bool bfl4sH;
static GLint bt[4];
int sky = 1;
bool nosky = false;
bool bSmoke = false;
BOOL bSky = FALSE;


void APIENTRY Hooked_glBegin(GLenum mode)
{
	g_glMode = mode;

	//wallhack
	if (GetKeyState(VK_NUMPAD1)) {
		if (mode == GL_TRIANGLES || mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN)
		{
			glDepthRange(0, 0.5);

		}
		else {
			glDepthRange(0.5, 1);
		}
	}

	//perfect wallhack
	if (GetKeyState(VK_NUMPAD2)) {

		if (!(mode == GL_TRIANGLES || mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN || mode == GL_QUADS))
		{
			GLfloat curcol[4];
			glGetFloatv(GL_CURRENT_COLOR, curcol);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(curcol[0], curcol[1], curcol[2], 0.5f);
			glClearColor(0.0f, 0.0f, 0.0f, 0.05f);
		}
	}

	if (GetKeyState(VK_NUMPAD3)) {

		// smoke
		if (mode == GL_QUADS)
		{
			GLfloat smokecol[4];
			glGetFloatv(GL_CURRENT_COLOR, smokecol);
			if ((smokecol[0] == smokecol[1]) && (smokecol[0] == smokecol[2]) && (smokecol[0] != 0.0) && (smokecol[0] != 1.0) && (smokecol[0] >= 0.6) && (smokecol[0] <= 0.7)) { //works fine with csnz, didnt test in 1.6 tho
				glColor4f(smokecol[0], smokecol[1], smokecol[2], 0.01);
				bSmoke = true;
			}
			else {
				bSmoke = false;
			}

		}
	}
	//black sky
	if (GetKeyState(VK_NUMPAD4)) {
		bSky = (mode == GL_QUADS);
	}
	//LAMBERT

	if (GetKeyState(VK_NUMPAD5)) {

		if (mode == GL_TRIANGLE_STRIP)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		}
	}
	//no flash
	if (GetKeyState(VK_NUMPAD6)) {

		if (mode == GL_QUADS) {
			GLfloat curcol[4];
			glGetFloatv(GL_CURRENT_COLOR, (float*)&curcol);
			if ((curcol[0] = 1.0f) && (curcol[1] == 1.0f) && (curcol[2] == 1.0f)) {
				glColor4f(curcol[0], curcol[1], curcol[2], 0.05f);
			}
		}
	}

	//night mode
	if (GetKeyState(VK_NUMPAD7)) {

		if (mode == GL_POLYGON)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
			pglBegin(mode);
		}
	}
	//white walls
	if (GetKeyState(VK_NUMPAD8)) {

		if (mode == GL_TRIANGLES || mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN || mode == GL_POLYGON)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		}
	}
	if (GetKeyState(VK_NUMPAD9)) {

		// Inverted Models
		if (mode == GL_TRIANGLE_STRIP)
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	}



	(*pglBegin)(mode);

}



void sys_glVertex2f(GLfloat x, GLfloat y)
{
	if (bfl4sH)
	{
		if (y == bt[3])
		{
			GLfloat fcurcol[4];
			glGetFloatv(GL_CURRENT_COLOR, fcurcol);
			glColor4f(fcurcol[0], fcurcol[1], fcurcol[2], 0.01f);
		}
	}
	(glVertex2f) (x, y);
}



void APIENTRY Hooked_glEnd(void)
{

	(*pglEnd)();
}

 void APIENTRY Hooked_glVertex3fv(GLfloat* v)
{
	if (bSky)return;
	if (bSmoke)return;
	(*pglVertex3fv)(v);
 }

void APIENTRY Hooked_glClear(GLbitfield mask)
{
	if (mask == GL_DEPTH_BUFFER_BIT)

	{
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		(*pglClear)(mask);
	}
	(*pglClear)(mask);
}

void* DetourFunc(BYTE* src, const BYTE* dst, const int len)
{
	BYTE* jmp = (BYTE*)malloc(len + 5);
	DWORD dwback;
	VirtualProtect(src, len, PAGE_READWRITE, &dwback);
	memcpy(jmp, src, len);
	jmp += len;
	jmp[0] = 0xE9;
	*(DWORD*)(jmp + 1) = (DWORD)(src + len - jmp) - 5;
	src[0] = 0xE9;
	*(DWORD*)(src + 1) = (DWORD)(dst - src) - 5;
	VirtualProtect(src, len, dwback, &dwback);
	return (jmp - len);
}

void HookOpenGL(void)
{
	HMODULE hmOpenGL = GetModuleHandleA("opengl32.dll");
	pglBegin = (glBegin_t)DetourFunc((LPBYTE)GetProcAddress(hmOpenGL, "glBegin"), (LPBYTE)&Hooked_glBegin, 5);
	pglEnd = (glEnd_t)DetourFunc((LPBYTE)GetProcAddress(hmOpenGL, "glEnd"), (LPBYTE)&Hooked_glEnd, 6);
	pglVertex3fv = (glVertex3fv_t)DetourFunc((LPBYTE)GetProcAddress(hmOpenGL, "glVertex3fv"), (LPBYTE)&Hooked_glVertex3fv, 5);
	pglClear = (glClear_t)DetourFunc((LPBYTE)GetProcAddress(hmOpenGL, "glClear"), (LPBYTE)&Hooked_glClear, 5);
	// gl hooks hier
}