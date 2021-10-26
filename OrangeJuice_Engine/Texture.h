#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Globals.h"

struct Texture
{
	Texture(const char* image_name);

	void LoadTexture();

	void DrawTexture();

};

#endif
