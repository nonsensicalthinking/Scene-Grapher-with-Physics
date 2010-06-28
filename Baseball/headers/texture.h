/*
 * texture.h
 *
 *  Created on: Jun 22, 2010
 *      Author: brof
 */

#include "Bitmap.h"
#include <GL/gl.h>
#include <GL/glu.h>

#ifndef TEXTURE_H_
#define TEXTURE_H_

typedef struct texture_s	{
	Bitmap *bmp;
	GLuint texID;
}texture_t;





inline bool loadTextureIntoGL(string str, texture_t *tex)	{
//	tex = new texture_t;

	tex->bmp = new Bitmap();

	if( tex->bmp->loadBMP(str) )
		cout << "Bitmap loaded: " << str << endl;
	else	{
		cout << "Couldn't load bitmap: " << tex->bmp->error << endl;
		return false;
	}

	GLuint tid;
	glGenTextures(1, &tid);

	tex->texID = tid;

	cout << "tid: " << tid << " tex->texID: " << tex->texID << endl;

	glBindTexture(GL_TEXTURE_2D, tex->texID);

	/* actually generate the texture */
	glTexImage2D(GL_TEXTURE_2D, 0, 3, tex->bmp->width,
			tex->bmp->height, 0, GL_RGB, GL_UNSIGNED_BYTE,
			tex->bmp->data);

	/* enable linear filtering */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}






#endif /* TEXTURE_H_ */
