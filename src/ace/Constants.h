#ifndef ACE_CONSTANTS_H_
#define ACE_CONSTANTS_H_

#include "Common.h"
#include "Singleton.h"

namespace ace {

#define constants (Constants::getInstance())

class Constants : public Singleton<Constants> {
public:

	static const std::string DefaultAbout, DefaultHelp;
	static const std::string DefaultMakefile, DefaultUi, DefaultConfig;

};

} // namespace ace

#endif
