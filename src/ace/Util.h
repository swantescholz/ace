#ifndef ACE_UTIL_H_
#define ACE_UTIL_H_

#include <map>
#include <algorithm>
#include "Common.h"
#include "Singleton.h"

#define util (Util::getInstance())

namespace ace { 

double operator"" _m(unsigned long long int v);

class Util : public Singleton<Util> {
public:

	void sleep(double sec);
	
	static constexpr double defaultEpsilon = 0.0001;	
	bool almostEqual(double a, double b, double epsilon = defaultEpsilon);
	
	void        system        (const std::string& str);              // a system call
	std::string getDirOfPath (std::string  ps, bool includeLastSlash = true);
	std::string getFileOfPath(std::string  ps);

	void mapConfigFile(std::map<std::string,std::string>& m, std::string str, bool ignoreWhitespaceBefore = true, bool ignoreWhitespaceAfter = true); //fills a map with X=Y

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
	std::string strformatd(std::string format, char delim, const T& head, const Args&... tail) {
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
};

}


#endif /*ACEUTIL_H_*/
