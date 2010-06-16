/*
 * Console.h
 *
 *  Created on: Jun 13, 2010
 *      Author: brof
 */

#include <string>

#ifndef CONSOLE_H_
#define CONSOLE_H_

enum CON_COMMAND {
	CLEAR,
	DUMP,
	QUIT,
	CON_HEIGHT
};



class Console {

public:
	Console();
	virtual ~Console();

	void processConsoleCommand(const std::string conInput);


};

#endif /* CONSOLE_H_ */
