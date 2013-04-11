#include "Util.h"

#include <ctime>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <chrono>

#include "Timer.h"
#include "Exception.h"

namespace ace {

bool Util::almostEqual(double a, double b, double epsilon) {
	return std::abs(a - b) <= ( (std::abs(a) < std::abs(b) ? std::abs(b) : std::abs(a)) * epsilon);
}

void Util::system(const std::string& str) {
	if(std::system(0)) {
		std::system(str.c_str());
	}
	else {
		ace_except("no system call shell available", "system");
	}
}




std::string Util::getDirOfPath(std::string ps, bool includeLastSlash) {
	size_t pos = ps.rfind("/");
	if (pos == std::string::npos) return std::string("");
	return ps.substr(0,pos+(includeLastSlash?1:0));
}
std::string Util::getFileOfPath(std::string ps) {
	size_t pos = ps.rfind("/");
	if (pos == std::string::npos) return ps;
	return ps.substr(pos+1);
}
std::map<std::string, std::string> Util::mapConfigFile(std::string str, bool ignoreWhitespaceBefore, bool ignoreWhitespaceAfter) {
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
std::string Util::addLineNumbersToString(std::string str, const std::string& indent) {
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
std::string Util::deleteComments(std::string str, const std::string& com_line,
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

bool Util::fileExists(const std::string& sPath) {
	std::ifstream fin;
	fin.open(sPath.c_str(), std::ios_base::in);
	if (!fin) return false;
	fin.close();
	return true;
}
int Util::fileSize(const std::string& sPath) {
	std::ifstream fin;
	fin.open(sPath.c_str(), std::ios_base::in);
	if (!fin) return -1;
	fin.seekg(0, std::ios_base::end);
	int pos = fin.tellg();
	fin.close();
	return pos;
}
bool Util::removeFile(const std::string& sPath) {return remove(sPath.c_str()) == 0;}
bool Util::renameFile(const std::string& sPathOld, const std::string& sPathNew) {
	return rename(sPathOld.c_str(), sPathNew.c_str()) == 0;}

}


