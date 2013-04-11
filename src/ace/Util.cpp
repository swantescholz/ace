#include "Util.h"

#include <ctime>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <fstream>

#include "Timer.h"
#include "Assert.h"
#include "String.h"

using namespace std;

namespace ace {

void Util::sleep(double sec) {
	this_thread::sleep_for(chrono::microseconds((long long int)(sec * 1000000.0)));
}

bool Util::almostEqual(double a, double b, double epsilon) {
	return std::abs(a - b) <= ( (std::abs(a) < std::abs(b) ? std::abs(b) : std::abs(a)) * epsilon);
}

void Util::system(const std::string& str) {
	if(std::system(0)) {
		std::system(str.c_str());
	}
	else {
		assertFail("no system call shell available");
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
void Util::mapConfigFile(std::map<std::string,std::string>& m, std::string str, bool ignoreWhitespaceBefore, bool ignoreWhitespaceAfter) {
	auto lines = String(str).toLines();
	std::string a,b;
	std::string::size_type pos;
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


