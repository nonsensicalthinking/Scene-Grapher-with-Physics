/*
 * files.h
 *
 *  Created on: Jan 21, 2012
 *      Author: brof
 */
#include <iostream>
#include <string>

#ifdef _WIN32
#include <direct.h>
#endif


using namespace std;

#ifndef FILES_H_
#define FILES_H_

void changeCWD(string path)	{
#ifdef linux
		return chdir(path.c_str());
#endif

#ifdef _WIN32
		return _chdir(path.c_str());
#endif
}

#endif /* FILES_H_ */
