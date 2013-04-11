#include "String.h"
#include "Hacks.h"
#include <algorithm>



namespace ace {

const String String::whitespaceCharacters(" \t\n\r");

String::String(const std::vector<String>& strs, const String& delim) {
	self = "";
	for(auto str : strs) {
		self += str + delim;
	}
	if (!self.empty()) {
		self.erase(self.lengthInBytes()-delim.lengthInBytes());
	}
}
bool String::startsWith(const String& s) const {return find(s) == 0;}
bool String::endsWith(const String& s) const {return static_cast<int>(find_last_of(s)) == self.lengthInBytes()-1;}
bool String::contains(const String& s) const {return find(s) != string::npos;}

String String::replaceAll(const String& oldString, const String& newString) const {
	String str(*this);
	int pos = 0, la = oldString.lengthInBytes(), lb = newString.lengthInBytes();
	for (;;) {
		pos = str.find(oldString, pos);
		if (pos == npos) break;
		str.replace(pos, la, newString);
		pos += lb;
	}
	return str;
}

String String::extractTail(const String& after) const {
	int pos = self.find_last_of(after);
	return self.substr(pos + 1);
}

String String::repeat(int times, const String& delim) const {
	String repeated;
	forito(times) {
		repeated += self;
		if (i+1 < times) {
			repeated += delim;
		}
	}
	return repeated;
}

std::vector<String> String::toLines() const {
	return self.split(newline,-1);
}
std::vector<String> String::split(const String& where, int maxTimes) const {
	std::vector<String> parts;
	const int lenWhere = where.lengthInBytes();
	int pos = 0;
	int times = 0;
	for(;;) {
		int oldPos = pos;
		pos = self.find(where, pos);
		String item;
		if (pos == npos) {
			item = self.substr(oldPos);
		} else {
			item = self.substr(oldPos, pos-oldPos);
		}
		parts.push_back(item);
		times++;
		if (times == maxTimes || pos == npos) break;
		pos += lenWhere;
	}
	return parts;
}
int String::lengthInBytes() const {
	return static_cast<int>(self.size());
}
int String::lengthInCharacters() const {
	//counting utf8 correctly now:
	int count = 0;
	for (char c : self) {
		if ((c & 0xc0) != 0x80) count++;
	}
	return count;
}
bool String::isWhitespace() const {
	return self.find_first_not_of(whitespaceCharacters) == string::npos;
}


String String::trimWhitespace(bool front, bool middle, bool back) const {
	String s = self;
	if (front) {
		int firstNonwhitePos = s.find_first_not_of(whitespaceCharacters);
		s = s.substr(firstNonwhitePos);
	}
	if (back) {
		int lastNonwhitePos = s.find_last_not_of(whitespaceCharacters);
		s = s.substr(0,lastNonwhitePos + 1);
	}
	if (middle) {
		for (int i = 0; i < s.lengthInBytes(); ++i) {
			if (whitespaceCharacters.contains(s[i])) {
				s[i] = ' ';
			}
		}
		int lenFront = s.find_first_not_of(whitespaceCharacters);
		int lenBack = s.lengthInBytes() - s.find_last_not_of(whitespaceCharacters) - 1;
		if (lenFront == npos) lenFront = 0;
		if (lenBack == npos) lenBack = 0;
		for(;;) {
			int index = s.find("  ", lenFront);
			if (index >= s.lengthInBytes() - lenBack or index == npos) break;
			s.erase(index,1);
		}
	}
	return s;
}

String String::removeEnding(const String& ending) const {
	if (self.endsWith(ending)) {
		return self.substr(0,self.lengthInBytes() - ending.lengthInBytes());
	}
	return self;
}

String String::breakLines(int maxLength) const {
	vector<String> lines;
	String line = "";
	for (int i = 0; i < self.lengthInBytes(); ++i) {
		String ch = String(self.at(i));
		line += ch;
		if (line.isWhitespace()) line.clear();
		bool isLineTooLong = line.lengthInCharacters() > maxLength and ch.isWhitespace();
		bool wasNewline = ch == "\n";
		if (isLineTooLong or wasNewline) {
			line = line.trimWhitespace(true,false,true);
			lines.push_back(line);
			line.clear();
		}
	}
	if (!line.empty()) {
		lines.push_back(line);
	}
	return String(lines);
}

String String::toLower() const {
	String s(self);
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

String String::toUpper() const {
	String s(self);
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}


} // namespace ace








