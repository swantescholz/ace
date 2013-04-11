#ifndef ACE_CONSTANTS_H_
#define ACE_CONSTANTS_H_

#include "Common.h"
#include "Singleton.h"

namespace ace {

#define constants (Constants::getInstance())

class Constants : public Singleton<Constants> {
public:

	static const std::string defaultAbout, defaultHelp;
	static const std::string defaultMakefile, defaultUi, defaultConfig;
	static const std::string configPath;
	static const std::string lastOpenedTabsPath, lastOpenedPathPath;
	static const std::string uiPath;
	static const std::string makefileName;
	static const std::string makefileTemplatePath;
};

} // namespace ace

#endif
