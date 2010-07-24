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

#define MAX_MAT_NAME_LEN	128

using namespace std;

typedef struct material_s	{
	char materialName[MAX_MAT_NAME_LEN];

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
			cout << "Material not found: " << matName << endl;
			return false;
		}


		bindTexture(mat->map_Kd);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat->Ka);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->Kd);
		// TODO use mat->illum to define how specular is used
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat->Ks);
		// TODO implement light emission materials
//		glMaterialfv(GL_FRONT, GL_EMISSION, mat-> );
		glMaterialf(GL_FRONT, GL_SHININESS, mat->Ns);

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

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // GL_LINEAR_MIPMAP_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

	bool hasMaterial(string matName)	{
		if( !materials[matName] )
			return false;

		return true;
	}
};








#endif /* TEXTURE_H_ */
