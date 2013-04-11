#include "Ipc.h"
#include "Util.h"
#include "Common.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <errno.h>
#include <ctype.h>

using namespace std;

namespace ace {

void selfCall(int num) {
	auto fifoName = (ACE_FIFO_NAME + util.lex(num)).c_str();
		
	int max = 5000;
	char *ch = new char[max];
	for(int i = 0;; ++i) {
		cin.getline(ch, max);
		if(cin.eof()) {
			break;
		}
		auto line = std::string(ch) + "\n";
		cout << i << "\t: " << line << endl;
		FILE* fifo = fopen((ACE_FIFO_NAME + util.lex(num)).c_str(), "w");
		fwrite(line.c_str(), 1, line.length(), fifo);
		fclose(fifo);
	}
	delete[] ch; ch = nullptr;
}

}
