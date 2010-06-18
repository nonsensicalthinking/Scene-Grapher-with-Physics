/*
 * Console.cpp
 *
 *  Created on: Jun 13, 2010
 *      Author: brof
 */

#include "Console.h"
#include <iostream>
#include <list>

using namespace std;



Console::Console() {
	input = new list<string>;
	output = new list<string>;	// console output

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
	this->output->push_front(conInput);

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

		cout << "Command pos: " << commands[x] << endl;
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

int main(void)	{

	Console *con = new Console();

	list<string> *tokens = new list<string>;

	con->processConsoleCommand("CONDUMP output.txt");
	con->processConsoleCommand("HERE");
	con->processConsoleCommand("THERE");
	con->processConsoleCommand("EVERYwhere");




	return 0;
}
