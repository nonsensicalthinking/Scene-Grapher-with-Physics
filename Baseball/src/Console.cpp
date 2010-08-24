/*
 * Console.cpp
 *
 *  Created on: Jun 13, 2010
 *      Author: Derek Brooks
 */
#include "Scene.h"
#include "strtools.h"
#include "Console.h"
#include "keys.h"
#include "Game.h"
#include <iostream>
#include <list>

using namespace std;

extern Scene* getScene();
extern Game* getGame();
extern float sceneAdvRate;


Console::Console(int width, int height) {
	input = new list<string>;
	instr = new ostringstream;
	output = new list<string>;	// console output
	font = new Font(width,height);
	screenWidth = width;
	screenHeight = height;
	minusIndex = 1;
}

Console::~Console() {
	delete input;
	delete instr;
	delete output;
	delete font;
}

void Console::scrollDown()	{
	if( minusIndex > 1 )
		minusIndex--;
}

void Console::scrollUp()	{
	if( minusIndex < output->size() )
		minusIndex++;
}

void Console::appendToInput(unsigned char s)	{
	if( s == ENTER_KEY )	{
		(*instr) << s;

		string coninput = instr->str();

		clearInput();

		processConsoleCommand(coninput);
	}
	else if( s == BACKSPACE_KEY )	{
		removeLastCharacter();
		inputString = instr->str();
	}
	else	{	// just append
		(*instr) << s;
		inputString = instr->str();
	}

}

void Console::removeLastCharacter()	{
	string s = instr->str();
	clearInput();
	(*instr) << s.substr(0, s.length()-1);
}

void Console::clearInput()	{
//	delete instr;
	instr = new ostringstream;
	inputString = "";
}

void range_tolower ( string::iterator beg, string::iterator end ) {
	for( string::iterator iter = beg; iter != end; ++iter ) {
		*iter = std::tolower( *iter );
	}
}

void string_tolower ( std::string &str ) {
	range_tolower( str.begin(), str.end() );
}



// TODO make these changeable and/or dependent upon environment
#define left 		0
#define lines		20	// number of visible lines on the screen? this has to be wrong...
#define lineHeight	16	// line hight in pixels

void Console::Draw()	{
	int lineNumber = lines;
	list<string>::iterator strItr;

	// TODO The background for the console is super ghetto
	// won't change with any other variables atm, -4 modifier
	// used to get a "decent" shape.

	// Console's background
	glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
		glLoadIdentity();
		glOrtho(0, screenWidth, 0, screenHeight, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
			GLfloat emis[] = {0.0, 0.0, 0.0};	// Make it black
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emis);
			glBegin(GL_QUADS);
			glVertex2i(0,(lines-4)*lineHeight);
			glVertex2i(screenWidth, (lines-4)*lineHeight);
			glVertex2i(screenWidth, screenHeight);
			glVertex2i(0,screenHeight);
			glEnd();
			glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);

	// Seek scroll position
	strItr = output->end();
	for(int x=0; x < minusIndex; x++)
		strItr--;

	// iterate and draw the console output backward so we can see most recent output
	for(; (strItr!=output->begin() && lineNumber > 0); strItr--, lineNumber--)
		font->glPrint(left, (screenHeight-(lineNumber*16)), (*strItr).c_str(), 0);

	string s = ">" + inputString;

	// draw input line
	font->glPrint(left, screenHeight-((lines+1)*16), s.c_str(), 0);

}

void Console::con_print(string s)	{
	output->push_back(s);
}



// Commands
enum {
	QUIT=0,
	CLEAR,
	PUTCAM,
	POLYCOUNT,
	LOAD,
	SET_PITCH,
	SET_YAW,
	SET_ROLL,
	FOV,
	PSPEED,
	SAR
};

// Commands need to be entered in lowercase
string commands[] = {
		"quit",
		"clear",
		"putcam",
		"polycount",
		"load",
		"pitch",
		"yaw",
		"roll",
		"fov",
		"pspeed",
		"sar"
};

extern int pitchSpeed;

void Console::processConsoleCommand(const string conInput)	{
	// Don't proceed without real input
	if( conInput[0] == '\n' || conInput[0] == '\r' )
		return;

	ostringstream oss;

	// Save command to history.
	// and save to console output
	this->input->push_front(conInput);
	this->output->push_back(conInput);

	Scene* curScene = getScene();

	string strin = conInput;
	string_tolower(strin);
	char line[MAX_CONSOLE_LINE_LEN];
	strcpy(line, strin.c_str());
	char *token = strtok(line, WHITESPACE);
	string cmd = token;

	int intCmd = -1;
	int commandCount = sizeof(commands)/8;
	for(int x=0; x < commandCount; x++ )	{
		if( cmd == commands[x] )	{
			intCmd = x;
			break;
		}

		if( x+1 == commandCount )
			intCmd = -1;
	}


	switch(intCmd)	{
	case QUIT:
		getScene()->exit();
		break;

	case CLEAR:
		output->clear();
		break;

	case PUTCAM:
		vec3_t origin;
		if( sscanf(conInput.c_str(), "putcam %f %f %f", &origin[0], &origin[1], &origin[2] ) > 0 )	{
			curScene->cam->goTo(origin);
			oss << "Camera placed @ [" << origin[0] << ", " << origin[1] << ", " << origin[2] << "]";
			con_print(oss.str());
		}
		else	{
			oss << "Camera @ [" << curScene->cam->origin[0] << ", " << curScene->cam->origin[1] << ", " << curScene->cam->origin[2] << "]";
			con_print(oss.str());
		}
		break;

	case POLYCOUNT:
		oss << "Polygons in scene: " << getScene()->polygonCount << endl;
		con_print(oss.str());
		break;

	case LOAD:
		char mapname[128];
		if( sscanf(conInput.c_str(), "load %s", mapname) > 0 )	{
			oss << "Loading map: " << mapname;
			con_print(oss.str().c_str());
			getGame()->load(mapname);
		}
		break;

	case SET_PITCH:
		float pitch;
		if( sscanf(conInput.c_str(), "pitch %f", &pitch) > 0 )
			curScene->cam->pitch_rate = pitch;

		oss << "pitch rate = " << curScene->cam->pitch_rate << "(rad)";
		con_print(oss.str());
		break;

	case SET_YAW:
		float yaw;
		if( sscanf(conInput.c_str(), "yaw %f", &yaw) > 0 )
			curScene->cam->yaw_rate = yaw;

		oss << "yaw rate = " << curScene->cam->yaw_rate << "(rad)";
		con_print(oss.str());
		break;

	case SET_ROLL:
		float roll;
		if( sscanf(conInput.c_str(), "roll %f", &roll) > 0 )
			curScene->cam->roll_rate = roll;

		oss << "roll rate = " << curScene->cam->roll_rate << "(rad)";
		con_print(oss.str());
		break;

	case FOV:
		int val;
		if( sscanf(conInput.c_str(), "fov %d", &val) > 0 )	{
			curScene->cam->setFieldOfView(val);
		}

		oss << "FOV = " << curScene->cam->fov << endl;
		con_print(oss.str());
		break;


	case PSPEED:
		int val1;
		if( sscanf(conInput.c_str(), "pspeed %d", &val1) > 0 )	{
			pitchSpeed = val1;
		}

		oss << "Pitch speed = " << pitchSpeed << endl;
		con_print(oss.str());
		break;

	case SAR:
		int val2;
		if( sscanf(conInput.c_str(), "sar %d", &val2) > 0 )	{
			sceneAdvRate = val2;
		}

		oss << "SAR = " << sceneAdvRate << endl;
		con_print(oss.str());
		break;



	case -1:
		oss << "Unrecognized command: " << token << endl;
		con_print(oss.str());
		break;
	}

}









