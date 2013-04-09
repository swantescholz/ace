#include "AceIpc.h"
#include "AceUtil.h"

namespace ace {

void selfCall(int num) {
	std::string s = "", sall = "";
	int max = 50000;
	char *ch = new char[max];
	for(int i = 0;; ++i) {
		cin.getline(ch, max);
		s = std::string(ch);
		sall += s;
		if(cin.eof()) {
			if(s.empty()) {
				sall.erase(sall.length()-1);
			}
			break;
		}
		sall += "\n";
		cout << i << "\t: " << s << endl;
	}
	cout << sall << endl;
	FILE* fifo = fopen((ACE_FIFO_NAME + util::lex(num)).c_str(), "w");
	fwrite(sall.c_str(), 1, sall.length(), fifo);
	fclose(fifo);
	delete[] ch; ch = nullptr;
}

}
