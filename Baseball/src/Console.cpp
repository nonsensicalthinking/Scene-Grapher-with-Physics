/*
 * Console.cpp
 *
 *  Created on: Jun 13, 2010
 *      Author: brof
 */

#include "Console.h"
#include "keys.h"
#include <iostream>
#include <list>

using namespace std;


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
	instr = new ostringstream;
}

Console::Console() {
	input = new list<string>;
	instr = new ostringstream;
	output = new list<string>;	// console output
	font = new Font(800,600);
}

Console::~Console() {
	// TODO Auto-generated destructor stub
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



void range_tolower ( string::iterator beg, string::iterator end ) {
	for( string::iterator iter = beg; iter != end; ++iter ) {
		*iter = std::tolower( *iter );
	}
}

void string_tolower ( std::string &str ) {
	range_tolower( str.begin(), str.end() );
}



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


int Console::TokenizeString(const string str, const char delim, list<string> *tokens)
{
    string::size_type lastPos = str.find_first_not_of(delim, 0);
    string::size_type pos = str.find_first_of(delim, lastPos);

    while (string::npos != pos || string::npos != lastPos)	{
    	tokens->push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delim, pos);
        pos = str.find_first_of(delim, lastPos);
    }

    return tokens->size();
}

#define top 600
#define left 0
#define lines		10

void Console::Draw()	{
	int lineNumber = lines;
	list<string>::iterator strItr;

	// TODO Implement a way to scroll up in the console
	// TODO Give console a background and outline the input line

	// iterate and draw the console output backward so we can see most recent output
	for(strItr=output->end(); (strItr!=output->begin() && lineNumber > 0); strItr--, lineNumber--)
		font->glPrint(left, (top-(lineNumber*16)), (*strItr).c_str(), 0);

	// draw input line
	font->glPrint(left, top-(lines*16), inputString.c_str(), 0);


}



/*
int main(void)	{

	Console *con = new Console();

	list<string> *tokens = new list<string>;

	con->processConsoleCommand("CONDUMP output.txt");
	con->processConsoleCommand("HERE");
	con->processConsoleCommand("THERE");
	con->processConsoleCommand("EVERYwhere");
	con->processConsoleCommand("boo");




	return 0;
}
*/
