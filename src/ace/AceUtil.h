#ifndef ACEUTIL_H_
#define ACEUTIL_H_

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include "AceCommon.h"

namespace ace {

//useful string operands
std::string operator+= (      std::string &s, bool   b);
std::string operator+  (const std::string &s, bool   b);
//std::string operator+= (      std::string &s, char   c);
//std::string operator+  (const std::string &s, char   c);
std::string operator+= (      std::string &s, int    i);
std::string operator+  (const std::string &s, int    i);
std::string operator+= (      std::string &s, float  f);
std::string operator+  (const std::string &s, float  f);
std::string operator+= (      std::string &s, double d);
std::string operator+  (const std::string &s, double d);

std::string operator+  (bool   b, const std::string &s);
std::string operator+  (char   c, const std::string &s);
std::string operator+  (int    i, const std::string &s);
std::string operator+  (float  f, const std::string &s);
std::string operator+  (double d, const std::string &s);

namespace util {

void        system        (const std::string& str);              // a system call
void        delay         (double timeInSec);                    // a simple delay in seconds
double      timeDiff      ();                                    // returns the time since the last call
int         getNextNumber (int pNum = -1) ;                       // returns a number higher than last
bool        onceTrue      ();                                    // returns true on first call, otherwise false
float       floor         (float  f);                            //returns the floor
double      floor         (double d);                            //returns the floor
float       ceil          (float  f);                            //returns the ceiling
double      ceil          (double d);                            //returns the ceiling
bool        isNearlyZero  (float  f);                            //returns wheather f is nearly 0
bool        isNearlyZero  (double d);                            //returns wheather d is nearly 0
int         round         (double d);
std::string replaceAnyWith(std::string str, const std::string& any, const std::string& with); //replaces all anys with with
std::string deleteLetters          (std::string ps, std::string delims = " \t\n\v\f\r", bool atFront = true, bool inMiddle = true,  bool atEnd = true); //deletes letters where you want to
std::string deleteOtherLettersThan (std::string ps, std::string delims = "1234567890.", bool atFront = true, bool inMiddle = true,  bool atEnd = true); //deletes letters which are not part of delims where you want to
std::string reduceWhitespaces      (std::string ps);                                //deletes a space after a space
std::string getDirOfPath (std::string  ps, bool includeLastSlash = true);
std::string getFileOfPath(std::string  ps);
bool        stringBeginsWith(const std::string& s1, const std::string& s2);         //does string s1 begin with s2?
bool        stringEndsWith  (const std::string& s1, const std::string& s2);         //does string s1 end   with s2?
bool        isStringWhitespace(const std::string& str);                             //is the whole string whitespace?
void        stringToLines   (const std::string& strWithNewlines, std::vector<std::string> &out); //divides string into lines placed in 'out'
std::map<std::string, std::string> mapConfigFile(std::string str, bool ignoreWhitespaceBefore = true, bool ignoreWhitespaceAfter = true); //fills a map with X=Y
std::string addLineNumbersToString(std::string str, const std::string& indent = "\t");
std::string deleteComments  (std::string str, const std::string& com_line = "//",
	const std::string& com_block1 = "/*",
	const std::string& com_block2 = "*/", bool deleteWhiteLines = false); //deletes all line and block-comments from str
std::string addZeros     (std::string  ps, int newSize);  //returns a string with added '0' in front
void        toStringArray(int argc, char **argv, std::string *buffer); //converts command line arguments into a string array

//files
bool  fileExists  (const std::string& sPath); //does this file exist?
int   fileSize    (const std::string& sPath); //returns the size of this file in bytes
bool  removeFile  (const std::string& sPath); //removes the file if existent
bool  renameFile  (const std::string& sPathOld, const std::string& sPathNew); //renames the file if existent

// TEMPLATE-FUNCIONS
template<typename T> std::string lex(const T          & obj) {std::stringstream ss            ; ss << obj; return ss.str();}
template<typename T> T           lex(const std::string& str) {std::stringstream ss(str); T obj; ss >> obj; return obj;}
template<typename T> constexpr int variadic_size(const T& obj) {return 1;}
template<typename T, typename... Args>
constexpr int variadic_size(const T& head, Args... tail) {return 1+variadic_size(tail...);}

// string-formatting
template<typename T>
void strformat_i(std::stringstream& ss, const T& t ) {
	ss << t << static_cast<char>(1);
}
template< typename T, typename... Args>
void strformat_i(std::stringstream& ss, const T& head, const Args&... tail) {
	strformat_i(ss, head );
	strformat_i(ss, tail... );
}
template< typename T, typename... Args>
std::string
strformatd(std::string format, char delim, const T& head, const Args&... tail)
{
	std::vector<std::string> parts;
	std::string s;
	for(unsigned int i = 0; i < format.length(); ++i) {
		if(format[i] == delim) {
			parts.push_back(s);
			s.clear();
		}
		else s += format[i];
	}
	parts.push_back(s);
	std::stringstream ss;
	strformat_i(ss, head, tail...);
	s = ss.str();
	ss.clear(); ss.str("");
	s = parts[0] + s;
	const char delim2 = static_cast<char>(1);
	int index = 1;
	for(unsigned int i = 0; i < s.length(); ++i) {
		if(s[i] == delim2) {
			if(index < static_cast<int>(parts.size()))
				s.replace(i,1,parts[index++]);
			else
				s.replace(i,1,"");
		}
	}
	while(index < static_cast<int>(parts.size())) {
		s += parts[index];
		++index;
	}
	parts.clear();
	return s;
}
template< typename T, typename... Args>
std::string strformat(std::string format, const T& head, const Args&... tail)
{return strformatd(format, '%', head, tail...);}
//PARSING
template<typename T> int _strparse(std::vector<std::string>& tokens, int index, T& head) {
	if(index >= static_cast<int>(tokens.size())) return 1;
	std::stringstream ss;
	ss << tokens[index];
	ss >> head;
	if(ss.fail()) return 1;
	return 0;
}
template<typename T, typename... Args> int _strparse(std::vector<std::string>& tokens, int index, T& head, Args&... tail) {
	return _strparse(tokens, index, head) + _strparse(tokens, index+1, tail...);
}
template<typename... Args> int strparse(std::string format, char delim, std::string str, Args&... tail) {
	if(format.length() <= 0 || format.find(delim) == std::string::npos) return 0;
	int returnValue = 0;
	str    = static_cast<char>(1) + str    + static_cast<char>(2);
	format = static_cast<char>(1) + format + static_cast<char>(2);
	std::vector<std::string> delims;
	std::string s = "";
	for(char c : format) {
		if(c == delim) {
			delims.push_back(s);
			s.clear();
		}
		else
			s += c;
	}
	delims.push_back(s);
	const int nTokens = static_cast<int>(delims.size()) - 1;
	const int nArgs   = variadic_size(tail...);
	returnValue = nArgs-nTokens; //more/less tokens there, than read
	std::vector<std::string> tokens;
	size_t p1 = 0, p2 = 0;
	for(int i = 0; i < nTokens; ++i) {
		p1 = str.find(delims[i  ], p2);
		p2 = str.find(delims[i+1], p1+delims[i].length());
		if(p1 == std::string::npos || p2 == std::string::npos) {
			returnValue = nArgs-i; //that many arguments could not be filled
			break;
		}
		p1 += delims[i].length();
		tokens.push_back(str.substr(p1, p2-p1));
	}
	int tmp = _strparse(tokens, 0, tail...);
	if(tmp != 0) return tmp;
	return returnValue;
}
template<typename... Args> int strparse(std::string format, std::string str, Args&... tail) {
	return strparse(format, '%', str, tail...);
}


}
}


#endif /*ACEUTIL_H_*/
