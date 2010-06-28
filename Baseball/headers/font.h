/*
 * font.h
 *
 *  Created on: Jun 19, 2010
 *      Author: brof
 */


#include "texture.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>


#ifndef FONT_H_
#define FONT_H_


using namespace std;



typedef struct dimension_s	{
	float width;
	float height;
}dimention_t;



class Font	{
private:
	GLuint base;            /* base display list for the font */
//	GLuint texture;      /* storage for our two textures */
	GLuint loop;            /* generic loop variable */
	GLfloat cnt1, cnt2;     /* counters for movement and coloring */
	dimention_t *windowDimention;
	texture_t *fontTex;

public:
	Font(dimention_t *dimention)	{
		this->windowDimention = dimention;

		fontTex = new texture_t;

		string fontName = "font.bmp";

		if( loadTextureIntoGL(fontName, fontTex) )	{
//			texture = fontTex->texID;
			buildFont();
			cout << "Font Build Complete." << endl;
		}
		else
			cout << "Font build failed!" << endl;

	}

	Font(float width, float height)	{

		this->windowDimention = new dimention_t;

		this->windowDimention->height = height;
		this->windowDimention->width = width;

		fontTex = new texture_t;

		string fontName = "font.bmp";

		if( loadTextureIntoGL(fontName, fontTex) )	{
//			texture = fontTex->texID;
			buildFont();
			cout << "Font Build Complete." << endl;
		}
		else
			cout << "Font build failed!" << endl;

	}


	~Font()	{
		killFont();
	}

	void buildFont(void) {
	    GLfloat cx, cy;         /* the character coordinates in our texture */
	    base = glGenLists(256);
	    glBindTexture(GL_TEXTURE_2D, fontTex->texID);
	    for (loop = 0; loop < 256; loop++)
	    {
	        cx = (float) (loop % 16) / 16.0f;
	        cy = (float) (loop / 16) / 16.0f;
	        glNewList(base + loop, GL_COMPILE);
	            glBegin(GL_QUADS);
	                glTexCoord2f(cx, 1 - cy - 0.0625f);
	                glVertex2i(0, 0);
	                glTexCoord2f(cx + 0.0625f, 1 - cy - 0.0625f);
	                glVertex2i(16, 0);
	                glTexCoord2f(cx + 0.0625f, 1 - cy);
	                glVertex2i(16, 16);
	                glTexCoord2f(cx, 1 - cy);
	                glVertex2i(0, 16);
	            glEnd();
	            glTranslated(12, 0, 0);
	        glEndList();
	    }
	}

	void killFont(void) {
	    glDeleteLists(base, 256);
	}

	void glPrint(GLint x, GLint y, const char *string, int set) {
	    if (set > 1)
	    	set = 1;

		glEnable(GL_TEXTURE_2D);
	    glBindTexture(GL_TEXTURE_2D, fontTex->texID);
	    glDisable(GL_DEPTH_TEST);
	    glMatrixMode(GL_PROJECTION);
	    glPushMatrix();
			glLoadIdentity();
			glOrtho(0, windowDimention->width, 0, windowDimention->height, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
				glLoadIdentity();
				glColor3f(1.0, 1.0, 1.0);
				glTranslated(x, y, 0);
				glListBase(base - 32 + (128 * set));
				glCallLists(strlen(string), GL_BYTE, string);
				glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
	    glPopMatrix();
	    glEnable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
	}

};



#endif /* FONT_H_ */