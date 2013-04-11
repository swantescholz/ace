#include "AceUtil.h"

#include <ctime>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <chrono>

#include "AceTimer.h"
#include "AceException.h"

namespace ace {

std::string operator+= (      std::string &s, bool   b) {if (b) return s += "true"; s += "false"; return s;}
std::string operator+  (const std::string &s, bool   b) {if (b) return s +  "true";   return (s + "false");}
//std::string operator+= (      std::string &s, char   c) {char a[2]; a[0] = c; a[1] = '\0'; return std::string(a);}
//std::string operator+  (const std::string &s, char   c) {char a[2]; a[0] = c; a[1] = '\0'; return std::string(a);}
std::string operator+= (      std::string &s, int    i) {char c[40]; sprintf(c, "%i", i); s+=c; return s;}
std::string operator+  (const std::string &s, int    i) {char c[40]; sprintf(c, "%i", i); return (s + c);}
std::string operator+= (      std::string &s, float  f) {char c[40]; sprintf(c, "%f", f); s+=c; return s;} //<- here possibly specify precision
std::string operator+  (const std::string &s, float  f) {char c[40]; sprintf(c, "%f", f); return (s + c);} //<- here possibly specify precision
std::string operator+= (      std::string &s, double d) {char c[40]; sprintf(c, "%g", d); s+=c; return s;} //<- here possibly specify precision
std::string operator+  (const std::string &s, double d) {char c[40]; sprintf(c, "%g", d); return (s + c);} //<- here possibly specify precision

std::string operator+  (bool   b, const std::string &s) {if (b) return "true" +  s; return "false" +  s;}
std::string operator+  (char   c, const std::string &s) {char a[2]; a[0] = c; a[1] = '\0'; return std::string(a);}
std::string operator+  (int    i, const std::string &s) {char c[40]; sprintf(c, "%i", i); return c +  s;}
std::string operator+  (float  f, const std::string &s) {char c[40]; sprintf(c, "%f", f); return c +  s;} //<- here possibly specify precision
std::string operator+  (double d, const std::string &s) {char c[40]; sprintf(c, "%g", d); return c +  s;} //<- here possibly specify precision

namespace util {

void system(const std::string& str) {
	if(std::system(0)) {
		std::system(str.c_str());
	}
	else {
		ace_except("no system call shell available", "system");
	}
}
void delay(double timeInSec) {
	auto t1 = Timer::tclock::now();
	while (std::chrono::duration<double>(Timer::tclock::now()-t1).count() < timeInSec) {}
}
double timeDiff() {
	static Timer::tclock::time_point last = Timer::tclock::now();
	double d = std::chrono::duration<double>(Timer::tclock::now() - last).count();
	last = Timer::tclock::now();
	return d;
}
int getNextNumber(int pNum) {
	static int sCounter = 0;
	if (pNum >= 0) sCounter = pNum;
	sCounter++;
	return sCounter-1;
}
bool  onceTrue() {static bool b=true; if(b){b=false;return true;}return false;}
float  floor(float  f) {return (int)f;}
double floor(double d) {return (int)d;}
float  ceil (float  f) {return (int)f+1;}
double ceil (double d) {return (int)d+1;}
int    round         (double d)               {
	if (d - static_cast<int>(d) < static_cast<int>(d)+1 - d)
		return static_cast<int>(d);
	return static_cast<int>(d)+1;
}

std::string replaceAnyWith(std::string str, const std::string& any, const std::string& with) {
	size_t pos = 0;
	size_t len1 = any.length(), len2 = with.length();
	for (;;) {
		pos = str.find(any, pos);
		if (pos == std::string::npos) return str;
		str.replace(pos, len1, with);
		pos += len2;
	}
}
std::string deleteLetters (std::string ps, std::string delims, bool atFront, bool inMiddle,  bool atEnd) {
	std::string::size_type notDelimPos;
	if (atFront == true) { // trim leading whitespace
		notDelimPos = ps.find_first_not_of(delims.c_str());
		ps.erase(0, notDelimPos);
	}
	if (atEnd == true) {   // trim trailing whitespace
		notDelimPos = ps.find_last_not_of (delims.c_str());
		ps.erase(notDelimPos+1);
	}
	if (inMiddle == true) {
		std::string tmp = ps;
		ps = "";
		for (unsigned int i = 0; i < tmp.length(); i++) {
			if (delims.find( tmp.at(i)) == std::string::npos) {
				ps += tmp.at(i);
			}
		}
	}
	return ps;
}
std::string deleteOtherLettersThan (std::string ps, std::string delims, bool atFront, bool inMiddle,  bool atEnd) {
	std::string::size_type delimPos;
	if (atFront == true) { // trim leading whitespace
		delimPos = ps.find_first_of(delims.c_str());
		ps.erase(0, delimPos);
	}
	if (atEnd == true) {   // trim trailing whitespace
		delimPos = ps.find_last_of (delims.c_str());
		ps.erase(delimPos+1);
	}
	if (inMiddle == true) {
		std::string tmp = ps;
		ps = "";
		for (unsigned int i = 0; i < tmp.length(); i++) {
			if (delims.find( tmp.at(i)) != std::string::npos) {
				ps += tmp.at(i);
			}
		}
	}
	return ps;
}
std::string reduceWhitespaces (std::string ps) {
	if (ps.length() <= 1) return ps; 
	std::string tmp;
	for (unsigned int i = 0; i < ps.length(); i++) {
		if (i != 0) {
			if (ps[i] == ' ' && ps[i-1] == ' ') {
				continue;
			}
		}
		tmp += ps[i];
	}
	return tmp;
}
std::string getDirOfPath(std::string ps, bool includeLastSlash) {
	size_t pos = ps.rfind("/");
	if (pos == std::string::npos) return std::string("");
	return ps.substr(0,pos+(includeLastSlash?1:0));
}
std::string getFileOfPath(std::string ps) {
	size_t pos = ps.rfind("/");
	if (pos == std::string::npos) return ps;
	return ps.substr(pos+1);
}
bool stringBeginsWith(const std::string& s1, const std::string& s2) {
	if(s2.length() > s1.length()) return false;
	if(s2 == s1) return true;
	for(unsigned int i = 0; i< s2.length(); i++) {
		if (s1[i] != s2[i]) return false;
	}
	return true;
}
bool stringEndsWith(const std::string& s1, const std::string& s2) {
	if(s2.length() > s1.length()) return false;
	if(s2 == s1) return true;
	for(unsigned int i = 0; i< s2.length(); i++) {
		if (s1[s1.length()-1-i] != s2[s2.length()-1-i]) return false;
	}
	return true;
}
bool isStringWhitespace(const std::string& str) {
	const unsigned long length = str.length();
	for (unsigned long i = 0; i < length; ++i) {
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\v' && str[i] != '\f' && str[i] != '\r')
			return false;
	}
	return true;
}
void stringToLines(const std::string& strWithNewlines, std::vector<std::string> &out) {
	std::string::size_type pos1 = 0, pos2 = 0;
	for(;;) {
		pos2 = strWithNewlines.find("\n", pos1);
		if(pos2 == std::string::npos) {
			out.push_back(strWithNewlines.substr(pos1));
			break;
		}
		out.push_back(strWithNewlines.substr(pos1, pos2-pos1));
		pos1 = pos2+1;
	}
}
std::map<std::string, std::string> mapConfigFile(std::string str, bool ignoreWhitespaceBefore, bool ignoreWhitespaceAfter) {
	std::vector<std::string> lines;
	stringToLines(str, lines);
	std::string a,b;
	std::string::size_type pos;
	std::map<std::string, std::string> m;
	for(auto i : lines) {
		pos = i.find("=");
		if(pos != std::string::npos) {
			a = i.substr(0,pos);
			b = i.substr(pos+1);
			if(ignoreWhitespaceBefore && a.length() > 0) {if(a[a.length()-1] == ' ') {a.erase(a.length()-1);}}
			if(ignoreWhitespaceAfter  && b.length() > 0) {if(b[0] == ' ') {b.erase(0,1);}}
			m.insert(std::make_pair(a,b));
		}
	}
	return std::move(m);
}
std::string addLineNumbersToString(std::string str, const std::string& indent) {
	long numLines = 1;
	size_t pos = 0;
	for(;;) {
		pos = str.find("\n", pos);
		if (pos == std::string::npos) break;
		++pos;
		++numLines;
	}
	const int numLength = util::lex(numLines).length();
	str = util::addZeros("1", numLength) + indent + str;
	numLines = 2;
	pos = 0;
	for(;;) {
		pos = str.find("\n", pos);
		if (pos == std::string::npos) break;
		str.replace(pos, 1, "\n"+util::addZeros(util::lex(numLines), numLength) + indent);
		++pos;
		++numLines;
	}
	return str;
}
std::string deleteComments(std::string str, const std::string& com_line,
	const std::string& com_block1,
	const std::string& com_block2, bool deleteWhiteLines) {
	size_t pos1 = 0, pos2, tmp1, tmp2;
	
	for(;;) {
		pos1 = str.find(com_line, pos1);
		if (pos1 == std::string::npos) break;
		pos2 = str.find("\n", pos1);
		if (pos2 == std::string::npos)
			str.erase(pos1);
		else
			str.erase(pos1, pos2-pos1);
		if (deleteWhiteLines) {
			//whitespaces?
			tmp1 = str.rfind("\n", pos1-1);
			if (tmp1 == std::string::npos) tmp1 = 0;
			tmp2 = str.find ("\n", pos1);
			if (tmp2 == std::string::npos) tmp2 = str.length()-1;
			if (util::isStringWhitespace(str.substr(tmp1, tmp2-tmp1))) {
				str.erase(tmp1, tmp2-tmp1);
			}
		}
	}
	pos1 = 0;
	for(;;) {
		pos1 = str.find(com_block1, pos1);
		if (pos1 == std::string::npos) break;
		pos2 = str.find(com_block2, pos1);
		if (pos2 == std::string::npos) { //end reached
			str.erase(pos1);
			break;
		}
		else
			str.erase(pos1, pos2-pos1+com_block2.length());
	}
	return str;
}
std::string addZeros(std::string  ps, int newSize) {
	for (int i = ps.length(); i < newSize; i++) {
		ps = "0" + ps;
	}
	return ps;
}
void toStringArray(int argc, char **argv, std::string *buffer) {
	if(buffer == nullptr || argc == 0 || argv == nullptr) return;
	for (int i = 0; i < argc; i++) {
		buffer[i] = argv[i];
	}
}

bool fileExists(const std::string& sPath) {
	std::ifstream fin;
	fin.open(sPath.c_str(), std::ios_base::in);
	if (!fin) return false;
	fin.close();
	return true;
}
int fileSize(const std::string& sPath) {
	std::ifstream fin;
	fin.open(sPath.c_str(), std::ios_base::in);
	if (!fin) return -1;
	fin.seekg(0, std::ios_base::end);
	int pos = fin.tellg();
	fin.close();
	return pos;
}
bool removeFile(const std::string& sPath) {return remove(sPath.c_str()) == 0;}
bool renameFile(const std::string& sPathOld, const std::string& sPathNew) {return rename(sPathOld.c_str(), sPathNew.c_str()) == 0;}

}

}


