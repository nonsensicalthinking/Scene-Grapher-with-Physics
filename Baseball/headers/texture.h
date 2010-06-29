/*
 * texture.h
 *
 *  Created on: Jun 22, 2010
 *      Author: brof
 *
 *
 *
 *  TODO For this file
 *	TODO Make loadJPEG function so we can use alpha layers
 *	TODO Make loadPNG function so we can use alpha layers
 *
 *
 *
 *
 */

#include "Bitmap.h"
#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>

#ifndef TEXTURE_H_
#define TEXTURE_H_

using namespace std;

class TextureManager	{

	map<string,GLuint> textures;

public:
	TextureManager()	{

	}

	~TextureManager()	{
		unloadAllTextures();
	}


	bool bindTexture(string texturePath)	{
		map<string,GLuint>::iterator tex = textures.find(texturePath);

		if( tex != textures.end() )	{
			glBindTexture(GL_TEXTURE_2D, tex->second);
			return true;
		}

		return false;
	}


	bool loadBitmap(string str)	{
		Bitmap* bmp = new Bitmap();

		if( bmp->loadBMP(str) )
			cout << "Bitmap loaded: " << str << endl;
		else	{
			cout << "Couldn't load bitmap: " << bmp->error << endl;
			return false;
		}

		GLuint tid;
		glGenTextures(1, &tid);
		glBindTexture(GL_TEXTURE_2D, tid);

		/* actually generate the texture */
		glTexImage2D(GL_TEXTURE_2D, 0, 3, bmp->width,
				bmp->height, 0, GL_RGB, GL_UNSIGNED_BYTE,
				bmp->data);

		/* enable linear filtering */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		textures[str] = tid;

		// remove the bitmap data, its in opengl now
		delete bmp;
		return true;
	}

	void unloadTexture(string path)	{
		textures.erase(path);
	}

	void unloadAllTextures()	{
		textures.clear();
	}

};








#endif /* TEXTURE_H_ */
