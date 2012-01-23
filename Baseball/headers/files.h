/*
 * files.h
 *
 *  Created on: Jan 21, 2012
 *      Author: brof
 */
#include <iostream>
#include <string>

//#ifdef WINDOWS
#include <direct.h>
//#endif


using namespace std;

#ifndef FILES_H_
#define FILES_H_

int changeCWD(string path)	{
#ifdef LINUX
		return chdir(path.c_str());
#endif
#ifdef WINDOWS
		return _chdir(path.c_str());
#endif
}

#endif /* FILES_H_ */
