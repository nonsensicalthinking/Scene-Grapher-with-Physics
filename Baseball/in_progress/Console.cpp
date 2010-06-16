/*
 * Console.cpp
 *
 *  Created on: Jun 13, 2010
 *      Author: brof
 */

#include "Console.h"
#include <iostream>

using namespace std;



Console::Console() {
	// TODO Auto-generated constructor stub

}

Console::~Console() {
	// TODO Auto-generated destructor stub
}

void Console::processConsoleCommand(const string conInput)	{

	string input = conInput;


}



#define MAX_STRING_SPLITS	10

int splitString(const char delimiter, const string toDice, string result[])	{

	// Do this C style, while(strstr()) blah blah, it'll work faster?



	cout << "Found " << count << " instances of '" << delimiter << "' " << endl;

	return count;
}



int main(void)	{

	Console *con = new Console();

	string a = "ABC";
	string b = "DEF";

	string e = a.substr(1, a.size()-1);

	cout << "e: " << e << endl;

	string split[MAX_STRING_SPLITS];
	int found = splitString(' ', "Hello there how are you?", split);

	int x;

	for(x=0; x < found; x++)
		cout << x << ": " << split[x] << endl;


	return 0;
}
