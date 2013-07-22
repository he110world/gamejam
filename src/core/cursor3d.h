#ifndef cursor3d_h__
#define cursor3d_h__

struct Cursor3D
{
	float x,y,z;
	float axisx,axisy,axisz,angle;
	float scalex,scaley,scalez;
};

// 3d cursor (current translation/rotation/scale)
void moveTo( float x, float y, float z );
void rotateTo( float axisx, float axisy, float axisz, float angle );
void scaleTo( float x, float y, float z );

extern Cursor3D g_cursor3d;

#endif // cursor3d_h__
