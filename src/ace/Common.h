#ifndef ACE_COMMON_H_
#define ACE_COMMON_H_

#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include <functional>

namespace ace {

using std::cout;
using std::cerr;
using std::cin;
using std::endl;

template<typename T> void print(T v) {std::cout << v;}
template<typename T> void println(T v) {std::cout << v << std::endl;}
template<typename T> void printnl(T v) {std::cout << std::endl;}

#ifdef _WIN32
	const std::string newline("\r\n");
#else
	const std::string newline("\n"); // Mac OS X uses \n, Linux too
#endif



#define declstrop(T) \
	std::string& operator+=(std::string& s, T i); \
	std::string operator+(std::string s, T i); \
	std::string operator+(T i, std::string s);
declstrop(bool) declstrop(short) declstrop(int) declstrop(long long int)
declstrop(float) declstrop(double) declstrop(std::size_t)
#undef declstrop

}

#endif /* ACECOMMON_H_ */
