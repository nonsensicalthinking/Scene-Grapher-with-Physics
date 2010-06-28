/*
 * Console.h
 *
 *  Created on: Jun 13, 2010
 *      Author: brof
 */
#include "font.h"
#include <iostream>
#include <sstream>
#include <string>
#include <list>

#ifndef CONSOLE_H_
#define CONSOLE_H_

enum CON_COMMAND {
	CLEAR,
	DUMP,
	QUIT,
	CON_HEIGHT
};

using namespace std;


class Console {

private:

public:
	string inputString;
	ostringstream* instr;
	list<string> *input;
	list<string> *output;
	Font* font;

	Console();
	virtual ~Console();

	void appendToInput(unsigned char s);
	void removeLastCharacter();
	void clearInput();
	void processConsoleCommand(const std::string conInput);
	int TokenizeString(const string str, const char delim, list<string> *tokens);
	void Draw();

};

#endif /* CONSOLE_H_ */
