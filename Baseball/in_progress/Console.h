/*
 * Console.h
 *
 *  Created on: Jun 13, 2010
 *      Author: brof
 */

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
	list<string> *input;
	list<string> *output;


public:
	Console();
	virtual ~Console();

	void processConsoleCommand(const std::string conInput);
	int TokenizeString(const string str, const char delim, list<string> *tokens);


};

#endif /* CONSOLE_H_ */
