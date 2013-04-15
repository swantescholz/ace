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
#include <chrono>
#include <thread>

using namespace std;

namespace ace {

void Ipc::runThread(string fifoName) {
	do {
		util.sleep(10_ms);
		mtx.lock();
		if (!input.empty()) {
			FILE* fifo = fopen(fifoName.c_str(), "w");
			cout << "input: " << input.c_str() << endl;
			fwrite(input.c_str(), 1, input.length(), fifo);
			input.clear();
			fclose(fifo);
		}
		mtx.unlock();
	} while (!stopThread);
}

void Ipc::selfCall(int num) {
	string fifoName = ACE_FIFO_NAME + util.lex(num);
	thread t([&]{runThread(fifoName);});
	for(int i = 0;; ++i) {
		char c;
		cin.get(c);
		if(cin.eof()) {
			break;
		}
		if (mtx.try_lock()) {
			input += c;
			mtx.unlock();
		}
	}
	stopThread = true;
	t.join();
}

}
