/*
 * MaterialManager.h
 *
 *  Created on: Jun 22, 2010
 *      Author: Derek Brooks
 *
 *
 *
 *  TODO For this file
 *	TODO Make loadJPEG function so we can use alpha layers
 *	TODO Make loadPNG function so we can use alpha layers
 *
 *	TODO Incorporate this class into a Material manager instead
 *		 of texture manager so that we can enable materials from
 *		 here as well.
 *
 *
 *
 */
#include "shared.h"
#include "Bitmap.h"
#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>

#ifndef MATERIAL_MANAGER_H_
#define MATERIAL_MANAGER_H_

using namespace std;

typedef struct material_s	{
	char materialName[128];

	vec3_t Ka;
	vec3_t Kd;
	vec3_t Ks;

	float d;	// Alpha
	float Ns;
	float Ni;
	int illum;

	char map_Kd[MAX_FILE_LENGTH];
}material_t;



class MaterialManager	{

	map<string,GLuint> textures;
	map<string,material_t*> materials;

public:
	MaterialManager()	{

	}

	~MaterialManager()	{
		unloadAllTextures();
	}


	bool bindTexture(string texturePath)	{
		map<string,GLuint>::iterator tex = textures.find(texturePath);

		if( tex != textures.end() )	{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, tex->second);
			return true;
		}

		return false;
	}

	bool enableMaterial(string matName)	{
		material_t* mat = materials[matName];

		if( !mat )	{
			cout << "No material." << endl;
			return false;
		}


		bindTexture(mat->map_Kd);
		// TODO Enable other effects

		return true;
	}

	void disableMaterial(string matName)	{
		material_t* mat = materials[matName];

		if( !mat )
			return;

		if( strcmp(mat->map_Kd, "(null)") )	{	// If "(null)" isn't the string
			glDisable(GL_TEXTURE_2D);

		}

	}


	bool loadBitmap(string str)	{
		Bitmap* bmp = new Bitmap();

		if( !bmp->loadBMP(str) )	{
			cout << "Couldn't load bitmap: " << bmp->error << endl;
			return false;
		}

		GLuint tid;
		glGenTextures(1, &tid);
		glBindTexture(GL_TEXTURE_2D, tid);

		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		gluBuild2DMipmaps( GL_TEXTURE_2D, 3, bmp->width, bmp->height,
		                       GL_RGB, GL_UNSIGNED_BYTE, bmp->data );

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

	void addMaterial(string matName, material_t* mat)	{
		materials[matName] = mat;
	}

	void removeMaterial(string matName)	{
		materials.erase(matName);
	}

};








#endif /* TEXTURE_H_ */
