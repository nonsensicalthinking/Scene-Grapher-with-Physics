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
			glColor3f(0.0, 0.0, 0.0);
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
	PICKING,
	POLYCOUNT,
	LOAD,
	VID_RESTART,
	PITCH,
	YAW,
	ROLL
};

// Commands need to be entered in lowercase
string commands[] = {
		"quit",
		"clear",
		"putcam",
		"picking",
		"polycount",
		"load",
		"vid_restart",
		"pitch",
		"yaw",
		"roll"
};

extern void vid_restart();

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
		sscanf(conInput.c_str(), "putcam %f %f %f", &origin[0], &origin[1], &origin[2] );
		oss << "Putting camera: " << origin[0] << ", " << origin[1] << ", " << origin[2];
		con_print(oss.str());
		break;
	case PICKING:
		int p;
		sscanf(conInput.c_str(), "picking %i", &p);
		oss << "Polygon picking: ";
		if( p )	{
			oss << "Enabled." << endl;
			getScene()->isPicking = true;
		}
		else	{
			oss << "Disabled." << endl;
			getScene()->isPicking = false;
		}
		con_print(oss.str());
		break;
	case POLYCOUNT:
		oss << "Polygons in scene: " << getScene()->polygonCount << endl;
		con_print(oss.str());
		break;
	case LOAD:
		char mapname[128];
		sscanf(conInput.c_str(), "load %s", mapname);
		oss << mapname;
		getGame()->load(oss.str().c_str());
		break;
	case VID_RESTART:
		vid_restart();
		break;
	case PITCH:
		sscanf(conInput.c_str(), "pitch %f", &curScene->cam->pitch_rate);
		break;
	case YAW:
		sscanf(conInput.c_str(), "yaw %f", &curScene->cam->yaw_rate);
		break;
	case ROLL:
		sscanf(conInput.c_str(), "roll %f", &curScene->cam->roll_rate);
		break;
	case -1:
		oss << "Unrecognized command: " << token << endl;
		con_print(oss.str());
		break;
	}

}









