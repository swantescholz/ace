#include "Common.h"

namespace ace {

#define implop(T) \
	std::string& operator+=(std::string& s, T i) {s += std::to_string(i); return s;} \
	std::string operator+(std::string s, T i) {return s + std::to_string(i);} \
	std::string operator+(T i, std::string s) {return std::to_string(i) + s;}
implop(bool)
implop(short)
implop(int)
implop(long long int)
implop(float)
implop(double)
implop(std::size_t)
#undef implop


} // namespace ace



