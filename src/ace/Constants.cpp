#include "Constants.h"

namespace ace {

const std::string Constants::DefaultAbout =
	R"(Default About)";
const std::string Constants::DefaultHelp =
	R"(Default Help)";
const std::string Constants::DefaultMakefile =
	R"(#Ace standard makefile
#WIN or LIN
OS   = LIN
NAME = test
SRCDIR = __SRCDIR__

C_LIN   = g++
ARG_LIN = 
INC_LIN = 
LIB_LIN = 
END_LIN =

C_WIN   = i586-mingw32msvc-g++
ARG_WIN = 
INC_WIN = 
LIB_WIN = 
END_WIN = .exe

SRC=$(wildcard $(SRCDIR)/*.cpp)
#print:
#	@echo SRC = $(SRC)
OBJ=$(SRC:.cpp=.o)

CC  = $(C_$(OS))
ARG = -std=c++0x $(ARG_$(OS))
INC = $(INC_$(OS))
LIB = $(LIB_$(OS))
END = $(END_$(OS))

all: $(SRCDIR)/$(NAME)$(END)

$(SRCDIR)/$(NAME)$(END): $(OBJ)
	$(CC) -o $@ $(ARG) $(INC) $(OBJ) $(LIB) 

%.o: %.cpp
	$(CC) -o $@ $(ARG) $(INC) -c $<

clean:
	rm -f $(SRCDIR)/*.o)";
	
	
const std::string Constants::DefaultUi =
	R"(<ui>
	<menubar name='MenuBar'>
		<menu action='MenuFile'>
			<menuitem action='FileNew'/>
			<menuitem action='FileOpen'/>
			<menuitem action='FileSave'/>
			<menuitem action='FileSaveAs'/>
			<menuitem action='FileClose'/>
			<separator/>
			<menuitem action='FileQuit'/>
		</menu>
		<menu action='MenuEdit'>
			<menuitem action='EditUndo'/>
			<menuitem action='EditRedo'/>
			<menuitem action='EditCut'/>
			<menuitem action='EditCopy'/>
			<menuitem action='EditPaste'/>
			<menuitem action='EditCloseBrace'/>
			<menuitem action='EditAutoIndent'/>
			<menuitem action='EditFindReplace'/>
			<menuitem action='EditReloadConfigFile'/>
		</menu>
		<menu action='MenuProject'>
			<menuitem action='ProjectBuild'/>
			<menuitem action='ProjectRun'/>
			<menuitem action='ProjectEditMakefile'/>
		</menu>
		<menu action='MenuView'>
			<menuitem action='ViewMaximize'/>
			<menuitem action='ViewFullscreen'/>
		</menu>
		<menu action='MenuHelp'>
			<menuitem action='HelpHelp'/>
			<separator/>
			<menuitem action='HelpAbout'/>
		</menu>
	</menubar>
	<toolbar name='ToolBar'>
		<toolitem action='FileQuit'/>
		<toolitem action='FileNew'/>
		<toolitem action='FileOpen'/>
		<toolitem action='FileSave'/>
		<toolitem action='EditAutoIndent'/>
		<toolitem action='EditFindReplace'/>
		<toolitem action='EditReloadConfigFile'/>
		<toolitem action='ProjectBuild'/>
		<toolitem action='ProjectRun'/>
		<toolitem action='HelpAbout'/>
	</toolbar>
</ui>)";


const std::string Constants::DefaultConfig =
	R"(programName=Ace Version 0.1
iconPath=res/icons/icon.png
width=600
height=600
maximized=0
fullscreen=0
terminalRelativeHeight=0.3
lastOpenedPathPath=config/lastOpenedPath.txt
lastOpenedTabsPath=config/lastOpenedTabs.txt
uiPath=config/ui.xml
makefileName=Makefile
makefileTemplatePath=config/MakefileTemplate
highlightMatchingBrackets=1
maxUndoLevels=30
smartIndent=1
indentOnTab=1
indentString=	
tabWidth=4
tabsAreSpaces=0
highlightCurrentLine=1
showLineNumbers=1
rightMarginPosition=90
showRightMargin=1
smartHomeEnd=2
wrapMode=4
font=monospace
)";

} // namespace ace

//where-am-i = $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
//SRCDIR := $(call where-am-i)



