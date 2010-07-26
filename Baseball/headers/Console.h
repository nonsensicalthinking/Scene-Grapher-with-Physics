/*
 * Console.h
 *
 *  Created on: Jun 13, 2010
 *      Author: Derek Brooks
 */
#include "font.h"
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <stdarg.h>

#ifndef CONSOLE_H_
#define CONSOLE_H_

#define MAX_CONSOLE_LINE_LEN	128

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
	void Draw();
	void con_print(string s);
};

#endif /* CONSOLE_H_ */
