
//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

// Source altered and distributed from https://github.com/AdrienHerubel/imgui

#ifndef IMGUI_H
#define IMGUI_H

enum imguiMouseButton
{
        IMGUI_MBUT_LEFT = 0x01, 
        IMGUI_MBUT_RIGHT = 0x02, 
};

enum imguiTextAlign
{
        IMGUI_ALIGN_LEFT,
        IMGUI_ALIGN_CENTER,
        IMGUI_ALIGN_RIGHT,
};

#define imguiRGBA(r,g,b,a) ((r) | (g << 8) | (b << 16) | (a << 24))

void imguiBeginFrame(int mx, int my, unsigned char mbut, int scroll);
void imguiEndFrame();

int imguiBeginScrollArea(const char* name, int x, int y, int w, int h, int* scroll);
void imguiEndScrollArea();

void imguiIndent();
void imguiUnindent();
void imguiSeparator();
void imguiSeparatorLine();

int imguiButton(const char* text, int enabled);
int imguiItem(const char* text, int enabled);
int imguiCheck(const char* text, int checked, int enabled);
int imguiCollapse(const char* text, const char* subtext, int checked, int enabled);
void imguiLabel(const char* text);
void imguiValue(const char* text);
int imguiSlider(const char* text, float* val, float vmin, float vmax, float vinc, int enabled);

void imguiDrawText(int x, int y, int align, const char* text, unsigned int color);
void imguiDrawLine(float x0, float y0, float x1, float y1, float r, unsigned int color);
void imguiDrawRoundedRect(float x, float y, float w, float h, float r, unsigned int color);
void imguiDrawRect(float x, float y, float w, float h, unsigned int color);

#endif // IMGUI_H