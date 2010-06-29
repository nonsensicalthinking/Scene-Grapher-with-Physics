/*
 * strtools.h
 *
 *  Created on: Jun 29, 2010
 *      Author: brof
 */

#include <iostream>
#include <string>
#include <sstream>
#include <list>

using namespace std;

#ifndef STRTOOLS_H__
#define STRTOOLS_H__

// String stuff
inline int TokenizeString(const string str, const char delim, list<string> *tokens)	{
    string::size_type lastPos = str.find_first_not_of(delim, 0);
    string::size_type pos = str.find_first_of(delim, lastPos);

    while (string::npos != pos || string::npos != lastPos)	{
    	tokens->push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delim, pos);
        pos = str.find_first_of(delim, lastPos);
    }

    return tokens->size();
}

#endif /* STRTOOLS_H__ */

