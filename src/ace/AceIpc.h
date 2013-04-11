#ifndef ACEIPC_H_
#define ACEIPC_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <errno.h>
#include <ctype.h>
#include "AceCommon.h"
#define ACE_FIFO_NAME "config/fifo/thefifo"

namespace ace {

void selfCall(int num);

}
#endif /* ACEIPC_H_ */
