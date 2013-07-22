#include "cursor3d.h"

Cursor3D g_cursor3d = {0.f,0.f,0.f,
					   1.f,0.f,0.f,0.f,
					   0.f,0.f,0.f};

//////////////////////////////////////////////////////////////////////////
void moveTo( float x, float y, float z )
{
	g_cursor3d.x = x;
	g_cursor3d.y = y;
	g_cursor3d.z = z;
}

void rotateTo( float axisx, float axisy, float axisz, float angle )
{
	g_cursor3d.axisx = axisx;
	g_cursor3d.axisy = axisy;
	g_cursor3d.axisz = axisz;
	g_cursor3d.angle = angle;
}

void scaleTo( float x, float y, float z )
{
	g_cursor3d.scalex = x;
	g_cursor3d.scaley = y;
	g_cursor3d.scalez = z;
}