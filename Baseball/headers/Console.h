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

using namespace std;


class Console {
public:
	string inputString;
	ostringstream* instr;
	list<string> *input;
	list<string> *output;
	int minusIndex;
	Font* font;

	int screenWidth;
	int screenHeight;

	Console(int width, int height);
	virtual ~Console();

	void scrollDown();
	void scrollUp();
	void appendToInput(unsigned char s);
	void removeLastCharacter();
	void clearInput();
	void processConsoleCommand(const std::string conInput);
	int TokenizeString(const string str, const char delim, list<string> *tokens);
	void Draw();

};

#endif /* CONSOLE_H_ */
