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
#include <iostream>
#include <list>

using namespace std;

extern Scene* getScene();


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
		processConsoleCommand(instr->str());
		clearInput();
	}
	else if( s == BACKSPACE_KEY )	{
		removeLastCharacter();
	}
	else	{	// just append
		(*instr) << s;
	}

	inputString = instr->str();
}

void Console::removeLastCharacter()	{
	string s = instr->str();
	clearInput();
	(*instr) << s.substr(0, s.length()-1);
}

void Console::clearInput()	{
	delete instr;
	instr = new ostringstream;
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
//#define screenHeight 600
#define left 0
#define lines		20

void Console::Draw()	{
	int lineNumber = lines;
	list<string>::iterator strItr;

	// TODO Give console a background and outline the input line
	// Draw it before drawing any console text.

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

void Console::print(string s)	{
	output->push_back(s);
}






// Commands
enum {
	QUIT=0,
	CLEAR,
	PUTCAM
};

// Commands need to be entered in lowercase
string commands[] = {
		"quit",
		"clear",
		"putcam"
};

void Console::processConsoleCommand(const string conInput)	{


	// Save command to history.
	// and save to console output
	this->input->push_front(conInput);
	this->output->push_back(conInput);

	string strin = conInput;
	string_tolower(strin);
	char line[MAX_CONSOLE_LINE_LEN];
	strcpy(line, strin.c_str());
	char *token = strtok(line, WHITESPACE);
	string cmd = token;

	int intCmd = -1;
	int commandCount = sizeof(commands)/8;
	for(int x=0; x < commandCount; x++ )	{
		cout << "Command SIZE: " << sizeof(commands) << endl;
		cout << "Command: " << commands[x] << endl;
		if( cmd == commands[x] )	{
			intCmd = x;
			break;
		}

		if( x+1 == commandCount )
			intCmd = -1;
	}

	stringstream s;

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
		s << "Putting camera: " << origin[0] << ", " << origin[1] << ", " << origin[2];
		print(s.str());
		break;
	case -1:
		cout << "NONE OF THEM" << endl;
		break;
	}

}









