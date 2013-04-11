#include "Assert.h"
#include "Common.h"

using namespace std;

namespace ace {
const std::string AssertionException::unknown = "<unknown>";

string AssertionException::constructMsg(string assertion, AssertionPoint point) {
	string str = "";
	if (!(point.func.find("__")==0)) str += point.func + " ";
	str += string("(") + point.file + ":" + point.line + ")" + " assertion '" + assertion + "' failed";
	return str;
}

std::string AssertionCloseException::constructMsg(double xv, std::string xstr, double yv, std::string ystr) {
	std::string str = xstr + " not-close-to " + ystr + newline;
	str += std::string("since [") + xv + "] not-close-to [" + yv + "]" + newline;
	str += std::string("( with default comparison-precision = ") + defaultPrecision + " )";
	return str;
}

std::string AssertionCloserThanException::constructMsg(double xv, std::string xstr, double yv, std::string ystr, double prec, std::string precstr) {
	std::string str = xstr + " not-close-to " + ystr + newline;
	str += std::string("since [") + xv + "] not-close-to [" + yv + "]" + newline;
	str += std::string("( with comparison-precision '") + precstr + "' = " + prec + " )";
	return str;
}

} // namespace ace






