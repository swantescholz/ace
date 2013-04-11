#ifndef ACE_IPC_H_
#define ACE_IPC_H_

#define ACE_FIFO_NAME "config/fifo/thefifo"

#include "Singleton.h"
#include <mutex>


namespace ace {

#define ipc (Ipc::getInstance())

class Ipc : public Singleton<Ipc> {
public:

	void selfCall(int num);
private:
	void runThread(std::string fifoName);
	std::string input;
	std::mutex mtx;
	bool stopThread = false;
	
};

}
#endif /* ACEIPC_H_ */
