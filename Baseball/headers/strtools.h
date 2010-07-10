/*
 * strtools.h
 *
 *  Created on: Jun 29, 2010
 *      Author: Derek Brooks
 */

#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <string.h>

using namespace std;

#define WHITESPACE " \t\n\r"

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

inline int countCharOccur(char str[], const char has)	{
	int occur = 0;
	int len = strlen(str);

	for(int x=0;x<len; x++)
		if( str[x] == has )
			occur++;

	return occur;
}

#endif /* STRTOOLS_H__ */

