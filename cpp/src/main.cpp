#include <iostream>
#include <sstream>
#include <string>
using namespace std;

int main() {
	cout << "hi ww" << endl;
	
	auto ch = new char[1024];
	for(;;) {
		cin.read(ch, 1024);
		string input = string(ch);
		cout << input;
		if (cin.eof()) break;
	}
	delete[] ch; ch = nullptr;
	cout << endl;
	
	return 0;	
}
