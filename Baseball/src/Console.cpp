/*
 * Console.cpp
 *
 *  Created on: Jun 13, 2010
 *      Author: brof
 */

#include "strtools.h"
#include "Console.h"
#include "keys.h"
#include <iostream>
#include <list>

using namespace std;

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






#define CMDCOUNT	3

// Commands
enum {
	HERE=0,
	THERE,
	EVERYWHERE
};


// Commands need to be entered in lowercase
string commands[] = {
		"here",
		"there",
		"everywhere"
};


void Console::processConsoleCommand(const string conInput)	{

	string cmd;

	// Save command to history.
	// and save to console output
	this->input->push_front(conInput);
	this->output->push_back(conInput);

	list<string> *tokens = new list<string>;

	if( TokenizeString(conInput, ' ', tokens) )	{
		cmd = tokens->front();
	}
	else	{
		if( conInput.size() )
			cmd = conInput;
		else
			return;	// Can't do anything without input
	}

	string_tolower(cmd);
//	cout << "ConInput CMD = " << cmd << endl;

	int x;
	int intCmd = -1;

	for(x=0; x < CMDCOUNT; x++ )	{

		cout << "Command: " << commands[x] << endl;
		if( cmd == commands[x] )	{
			intCmd = x;
			break;
		}

		if( x+1 == CMDCOUNT )
			intCmd = -1;
	}

	switch(intCmd)	{
	case HERE:
		cout << "Here" << endl;
		break;
	case THERE:
		cout << "There" << endl;
		break;
	case EVERYWHERE:
		cout << "EVERYWHERE" << endl;
		break;
	case -1:
		cout << "NONE OF THEM" << endl;
		break;
	}

}






