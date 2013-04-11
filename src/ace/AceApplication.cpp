#include "AceApplication.h"
#include "AceUtil.h"
#include <unistd.h>

using namespace Gtk;

namespace ace {

#define mymemfun(X) sigc::mem_fun(*this, &Application::X)

Application::Application(std::string aceExecName) {
	m_rootPath = util::getDirOfPath(aceExecName, true);
	m_confix["aceExec"] = aceExecName;
	
	m_stopFifoThread = false;
	m_chBufferSize = 50000;
	m_chBuffer = new char[m_chBufferSize];
	
	m_uiManager = UIManager::create();
	init();
	
	m_actionGroup = ActionGroup::create();
	m_actionGroup->add( Action::create("MenuFile"       , "_File"   ));
	m_actionGroup->add( Action::create("MenuEdit"       , "_Edit"   ));
	m_actionGroup->add( Action::create("MenuView"       , "_View"   ));
	m_actionGroup->add( Action::create("MenuProject"    , "_Project"));
	m_actionGroup->add( Action::create("MenuHelp"       , "_Help"   ));
	m_actionGroup->add( Action::create("FileNew"        , Stock::NEW    ), mymemfun(actNewFile));
	m_actionGroup->add( Action::create("FileOpen"       , Stock::OPEN   ), sigc::mem_fun(*this, &Application::actOpen   ));
	m_actionGroup->add( Action::create("FileSave"       , Stock::SAVE   ), sigc::mem_fun(*this, &Application::actSave   ));
	m_actionGroup->add( Action::create("FileSaveAs"     , Stock::SAVE_AS), AccelKey("<control><shift>s"), sigc::mem_fun(*this, &Application::actSaveAs ));
	m_actionGroup->add( Action::create("FileClose"      , Stock::CLOSE  ), sigc::mem_fun(*this, &Application::actClose  ));
	m_actionGroup->add( Action::create("FileQuit"       , Stock::QUIT   ), sigc::mem_fun(*this, &Application::actQuit   ));
	m_actionGroup->add( Action::create("EditUndo"       , Stock::UNDO   ), AccelKey("<control>z"), sigc::mem_fun(*this, &Application::actUndo   ));
	m_actionGroup->add( Action::create("EditRedo"       , Stock::REDO   ), AccelKey("<control>y"), sigc::mem_fun(*this, &Application::actRedo   ));
	m_actionGroup->add( Action::create("EditCut"        , Stock::CUT    ), sigc::mem_fun(*this, &Application::actCut    ));
	m_actionGroup->add( Action::create("EditCopy"       , Stock::COPY   ), sigc::mem_fun(*this, &Application::actCopy   ));
	m_actionGroup->add( Action::create("EditPaste"      , Stock::PASTE  ), sigc::mem_fun(*this, &Application::actPaste  ));
	m_actionGroup->add( Action::create("EditCloseBrace" ,"Close Brace"  ), AccelKey("braceright"), sigc::mem_fun(*this, &Application::actCloseBrace  ));
	m_actionGroup->add( Action::create("EditAutoIndent" , Stock::INDENT, "Auto Indent" ), AccelKey("<control>i"), sigc::mem_fun(*this, &Application::actAutoIndent  ));
	m_actionGroup->add( Action::create("EditFindReplace", Stock::FIND_AND_REPLACE), sigc::mem_fun(*this, &Application::actFindReplace ));
	m_actionGroup->add( Action::create("EditReloadConfigFile", Stock::PREFERENCES, "Reload Config File"), AccelKey("<control><shift>r"), sigc::mem_fun(*this, &Application::actReloadConfigFile ));
	m_actionGroup->add( ToggleAction::create("ViewMaximize", "Maximize", "Maximize Window", m_maximized ), AccelKey("F12"), sigc::mem_fun(*this, &Application::actMaximize  ));
	m_actionGroup->add( ToggleAction::create("ViewFullscreen", Stock::FULLSCREEN,"",""    , m_fullscreen), AccelKey("F11"), sigc::mem_fun(*this, &Application::actFullscreen));
	m_actionGroup->add( Action::create_with_icon_name("ProjectBuild", "build", "Build", "Build current project"), AccelKey("F5"), sigc::mem_fun(*this, &Application::actBuild  ));
	m_actionGroup->add( Action::create_with_icon_name("ProjectRun"  , "run"  , "Run"  , "Run current project"  ), AccelKey("F6"), sigc::mem_fun(*this, &Application::actRun    ));
	m_actionGroup->add( Action::create("ProjectEditMakefile", "_Edit Makefile"), AccelKey("F8"), sigc::mem_fun(*this, &Application::actOpenMakefile ));
	m_actionGroup->add( Action::create("HelpHelp"    , Stock::HELP   ), AccelKey("F1"), sigc::mem_fun(*this, &Application::actHelp   ));
	m_actionGroup->add( Action::create("HelpAbout"   , Stock::ABOUT  ), AccelKey("F9"), sigc::mem_fun(*this, &Application::actAbout  ));
	Glib::signal_timeout().connect(sigc::mem_fun(*this, &Application::timeCallback), 10 );
	
	m_uiManager->insert_action_group(m_actionGroup);
	add_accel_group(m_uiManager->get_accel_group());
	
	Widget* pMenubar = m_uiManager->get_widget("/MenuBar");
	Widget* pToolbar = m_uiManager->get_widget("/ToolBar");
	m_vbox.pack_start(*pMenubar, Gtk::PACK_SHRINK);
	m_vbox.pack_start(*pToolbar, Gtk::PACK_SHRINK);
	m_terminal.view.setText(std::string("THIS IS\nTHE TERMINAL"));
	m_terminal.window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_terminal.view.setShowLineNumbers(false);
	m_terminal.view.setHighlightCurrentLine(false);
	m_terminal.view.setEditable(false);
	m_terminal.view.addToContainer(m_terminal.window);
	m_notebook.set_scrollable(true);
	m_vpaned.add1(m_notebook);
	m_vpaned.add2(m_terminal.window);
	m_vbox.pack_start(m_vpaned);
	this->add(m_vbox);
	int width = 0, height = 0;
	get_size(width,height);
	m_vpaned.set_position(height * (1.0-util::lex<double>(m_config["terminalRelativeHeight"])));
	if(m_tabs.empty()) {addTab("NewFile", "", true);}
	show_all_children();
}

Application::~Application() {
	updateLastOpened();
	delete[] m_chBuffer; m_chBuffer = nullptr;
	m_stopFifoThread = true;
	FILE* fifo = fopen(m_fifoName.c_str(), "w");
	m_fifoThread->join();
	fclose(fifo);
	util::removeFile(m_fifoName);
	delete m_fifoThread; m_chBuffer = nullptr;
}
void Application::init() {
	openFifo();
	loadConfigFile();
	loadIcons("res/icons/", ".png");
	std::vector<std::string> stabs;
	Textfile::readFile(m_config["lastOpenedTabsPath"], stabs);
	for(auto s : stabs) {
		if(!s.empty())
			addTab(util::getFileOfPath(s),Textfile::readFile(s),false,util::getDirOfPath(s));
	}
}
void Application::loadConfigFile(const std::string& name) {
	if(!util::fileExists(name)) {
		Textfile::saveFile(s_sDefaultConfig, name);
	}
	Textfile tf;
	tf.readAt(name);
	m_config = util::mapConfigFile(tf.getText(), true, true);
	m_maximized  = (util::lex<int>(m_config["maximized" ]) != 0);
	m_fullscreen = (util::lex<int>(m_config["fullscreen"]) != 0);
	set_title(m_config["programName"]);
	set_icon_from_file(m_config["iconPath"]);
	set_size_request(util::lex<int>(m_config["width"]), util::lex<int>(m_config["height"]));
	if(m_maximized ) maximize();
	if(m_fullscreen) fullscreen();
	if(!util::fileExists(m_config["lastOpenedPathPath"])) {
		Textfile::saveFile("~", m_config["lastOpenedPathPath"]);
	}
	if(!util::fileExists(m_config["lastOpenedTabsPath"])) {
		Textfile::saveFile(std::string(""), m_config["lastOpenedTabsPath"]);
	}
	if(true || !util::fileExists(m_config["uiPath"])) {
		Textfile::saveFile(s_sDefaultUi, m_config["uiPath"]);
	}
	if(true || !util::fileExists(m_config["makefileTemplatePath"])) {
		Textfile::saveFile(s_sDefaultMakefile, m_config["makefileTemplatePath"]);
	}
	m_lastAccessedDir = Textfile::readFile(m_config["lastOpenedPathPath"]);
	m_uiManager->add_ui_from_string(Textfile::readFile(m_config["uiPath"]));
	m_makefileTemplate = Textfile::readFile(m_config["makefileTemplatePath"]);
}
void Application::loadSourceViewOptions(SourceView& view) {
	view.setHighlightMatchingBrackets(util::lex<int>(m_config["highlightMatchingBrackets"])==1);
	view.setMaxUndoLevels (util::lex<int>(m_config["maxUndoLevels"]));
	view.setSmartIndent   (util::lex<int>(m_config["smartIndent"])==1);
	view.setIndentOnTab   (util::lex<int>(m_config["indentOnTab"])==1);
	view.setIndentString  (m_config["indentString"]);
	view.setTabWidth      (util::lex<int>(m_config["tabWidth"]));
	view.setTabsAreSpaces (util::lex<int>(m_config["tabsAreSpaces"])==1);
	view.setHighlightCurrentLine(util::lex<int>(m_config["highlightCurrentLine"])==1);
	view.setShowLineNumbers     (util::lex<int>(m_config["showLineNumbers"])==1);
	view.setRightMarginPosition (util::lex<int>(m_config["rightMarginPosition"]));
	view.setShowRightMargin     (util::lex<int>(m_config["showRightMargin"])==1);
	int i = util::lex<int>(m_config["smartHomeEnd"]);
	int c = 0;
	switch (i) {
		case 1 : c = (int)GTK_SOURCE_SMART_HOME_END_DISABLED; break;
		case 2 : c = (int)GTK_SOURCE_SMART_HOME_END_BEFORE; break;
		case 3 : c = (int)GTK_SOURCE_SMART_HOME_END_AFTER; break;
		case 4 : c = (int)GTK_SOURCE_SMART_HOME_END_ALWAYS; break;
		default: c = (int)GTK_SOURCE_SMART_HOME_END_BEFORE; break;
	}
	view.setSmartHomeEnd(c);
	i = util::lex<int>(m_config["wrapMode"]);
	switch (i) {
		case 1 : c = (int)GTK_WRAP_NONE; break;
		case 2 : c = (int)GTK_WRAP_CHAR; break;
		case 3 : c = (int)GTK_WRAP_WORD; break;
		case 4 : c = (int)GTK_WRAP_WORD_CHAR; break;
		default: c = (int)GTK_WRAP_WORD_CHAR; break;
	}
	view.setWrapMode(c);
	view.setFont(m_config["font"]);
}
void Application::loadIcons(const std::string& start, const std::string& end) {
	std::vector<std::string> icons = {"run", "build", "stop", "delete"};
	for (auto i : icons) {
		IconTheme::add_builtin_icon(i, 32, Gdk::Pixbuf::create_from_file(start+i+end));
	}
}
void Application::addTab(const std::string& tabname, const std::string& text, bool isnew, const std::string& tabpath) {
	std::shared_ptr<Tab> tab(new Tab);
	tab->name = tabname;
	tab->path = tabpath;
	tab->isnew = isnew;
	tab->window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	loadSourceViewOptions(tab->view);
	tab->view.beginUndoableAction();
	tab->view.setText(text);
	tab->view.endUndoableAction();
	tab->view.isModified(true);
	updateTabLanguage(tab);
	tab->view.addToContainer(tab->window);
	m_notebook.append_page(tab->window, tab->name + ((isnew)?"*":""));
	m_notebook.set_tab_reorderable(tab->window, true);
	m_notebook.show_all();
	m_notebook.set_current_page(m_notebook.get_n_pages()-1);
	m_tabs.push_back(tab);
}
std::shared_ptr<Tab> Application::getCurrentTab() {
	int n = m_notebook.get_current_page();
	auto it = m_tabs.begin();
	for(int i = 0; i < n; ++i) {++it;}
	return *it;
}
void Application::openFifo() {
	int result = 0;
	int i = 1;
	do {
		m_fifoName = m_rootPath + ACE_FIFO_NAME + util::lex(i);
		m_fifoNum  = i;
		cout << "FIFO_NAME: " << m_fifoName << endl;
		result = mkfifo(m_fifoName.c_str(), S_IRUSR | S_IWUSR);
		++i;
	} while(result < 0);
	m_fifoThread = new std::thread([this](){this->checkFifo();});
}
void Application::checkFifo() {
	while(true) {
		m_fifo = fopen(m_fifoName.c_str(), "r");
		if(m_stopFifoThread) {
			break;
		}
		int numread = fread(m_chBuffer, 1, m_chBufferSize, m_fifo);
		fclose(m_fifo);
		if(numread <= 0) return;
		m_chBuffer[numread] = '\0';
		m_textForTerminal += std::string(m_chBuffer);
	}
}
bool Application::timeCallback() {
	if(!m_textForTerminal.empty()) {
		m_terminal.view.setText(m_terminal.view.getText() + m_textForTerminal);
		m_textForTerminal.clear();	
		auto vadj = m_terminal.window.get_vadjustment();
		auto newValue = vadj->get_upper() - vadj->get_page_size();
		vadj->set_value(newValue);	
	}
	auto tab = getCurrentTab();
	if(tab->view.isModified(true)) {
		if(std::string(m_notebook.get_tab_label_text(tab->window)).find("*") ==  std::string::npos) {
			m_notebook.set_tab_label_text(tab->window, tab->name + "*"); //with *
		}
		tab->view.update();
	}
	return true;
}
void Application::updateTabLanguage(std::shared_ptr<Tab>& tab) {
	if(tab->name.find(m_config["makefileName"]) == std::string::npos) {
		tab->view.setLanguageByFile(tab->name);
	} else {
		tab->view.setLanguageById("makefile");
	}
}
void Application::updateLastOpened() {
	Textfile::saveFile(m_lastAccessedDir, m_config["lastOpenedPathPath"]);
	std::string stabs;
	for(auto i : m_tabs) {
		if(!i->isnew) {
			stabs += i->path + i->name + "\n";
		}
	}
	if(!stabs.empty()) stabs.erase(stabs.length() - 1);
	Textfile::saveFile(stabs, m_config["lastOpenedTabsPath"]);
}
//------------------------------------------------
void Application::newFile() {
	cout << "NEW FILE" << endl;
	addTab("NewFile", "", true);
}
void Application::openFile() {
	cout << "OPEN FILE" << endl;
	FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);
	dialog.set_current_folder(m_lastAccessedDir);
	dialog.add_button(Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Stock::OPEN  , Gtk::RESPONSE_OK);
	const int result = dialog.run();
	m_lastAccessedDir = dialog.get_current_folder();
	if (result == Gtk::RESPONSE_OK) {
		auto filename = dialog.get_filename();
		auto path = util::getDirOfPath (filename);
		auto name = util::getFileOfPath(filename);
		auto text = Textfile::readFile(path + name);
		addTab(name,text,false,path);
		std::cout << "File selected: " <<  filename << std::endl;
	}
	else {
		std::cout << "Cancel clicked." << std::endl;
	}
	updateLastOpened();
}
void Application::saveFile() {
	cout << "SAVE FILE" << endl;
	auto tab = getCurrentTab();
	if(tab->isnew) {
		saveAsFile();
		return;
	}
	Textfile::saveFile(tab->view.getText(), tab->path + tab->name);
	m_notebook.set_tab_label_text(tab->window, tab->name); //without *
}
void Application::saveAsFile() {
	cout << "SAVE_AS FILE" << endl;
	auto tab = getCurrentTab();
	
	FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);
	dialog.set_create_folders(true);
	if(tab->isnew) {
		dialog.set_current_name(tab->name);
		dialog.set_current_folder(m_lastAccessedDir);
	}
	else {
		dialog.set_filename(tab->path+tab->name);
	}
	dialog.add_button(Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Stock::SAVE_AS , Gtk::RESPONSE_OK);
	const int result = dialog.run();
	m_lastAccessedDir = dialog.get_current_folder();
	if (result == Gtk::RESPONSE_OK) {
		std::string filename = dialog.get_filename();
		tab->path = util::getDirOfPath (filename);
		tab->name = util::getFileOfPath(filename);
		Textfile::saveFile(tab->view.getText(), tab->path + tab->name);
		m_notebook.set_tab_label_text(tab->window, tab->name); //without *
		tab->isnew = false;
		updateTabLanguage(tab);
		m_lastAccessedDir = tab->path;
		std::cout << "File selected: " <<  filename << std::endl;
	}
	else {
		std::cout << "Cancel clicked." << std::endl;
	}
	updateLastOpened();
}
void Application::closeFile() {
	cout << "CLOSE FILE" << endl;
	int n = m_notebook.get_current_page();
	auto it = m_tabs.begin();
	for(int i = 0; i < n; ++i) {++it;}
	m_notebook.remove_page(n);
	m_tabs.erase(it);
	if(m_tabs.empty()) {
		addTab("NewFile", "", true);
	}
	updateLastOpened();
}
void Application::clearTerminal() {
	m_terminal.view.setText("");	
}
void Application::buildProject() {
	cout << "BUILD PROJECT" << endl;
	saveFile();
	clearTerminal();
	auto tab = getCurrentTab();	
	if(tab->isnew) {
		m_textForTerminal = "Error: File '" + tab->name + "' not saved.";
		return;
	}
	auto makefileName = m_config["makefileName"];
	auto path = tab->path;
	path.erase(path.length()-1);
	while(true) {
		auto makefilePath = path+"/"+makefileName;
		if(util::fileExists(makefilePath)) {
			util::system("make -C " + path + " -f " + makefileName +" 2>&1 | "+m_config["aceExec"] +
				" --self" + util::lex(m_fifoNum) + " &");
			break;
		}
		if(path.length() <= 0) {
			makefilePath = tab->path + m_config["makefileName"];
			std::string makefileText = util::replaceAnyWith(m_makefileTemplate, "__SRCDIR__", util::getDirOfPath(makefilePath,false));
			Textfile::saveFile(makefileText, makefilePath);
			addTab(m_config["makefileName"], makefileText, false, tab->path);
			break;
		}
		path = util::getDirOfPath(path, false);
	}
}
void Application::runProject() {
	cout << "RUN PROJECT" << endl;
	clearTerminal();
	auto tab = getCurrentTab();
	if(tab->isnew) {
		m_textForTerminal = "Error: File '" + tab->name + "' not saved.";
		return;
	}
	std::string path = tab->path, makefilePath;
	path.erase(path.length()-1);
	while(true) {
		makefilePath = path+"/"+m_config["makefileName"];
		if(util::fileExists(makefilePath)) {
			std::string cmd = "make -C " + path + " run 2>&1 | " + m_config["aceExec"];
			cmd += " --self" + util::lex(m_fifoNum);
			cout << "exec: " << cmd << endl;
			util::system(cmd);
			break;
		}
		if(path.length() <= 0) {
			m_textForTerminal = "No Makefile/Executable to run found";
			break;
		}
		path = util::getDirOfPath(path, false);
	}
}
void Application::openMakefile() {
	cout << "OPEN MAKEFILE" << endl;
	auto tab = getCurrentTab();
	if(tab->isnew) {
		m_textForTerminal = "Error: File '" + tab->name + "' not saved.";
		return;
	}
	std::string path = tab->path, makefilePath;
	path.erase(path.length()-1);
	while(true) {
		makefilePath = path+"/"+m_config["makefileName"];
		if(util::fileExists(makefilePath)) {
			addTab(m_config["makefileName"], Textfile::readFile(makefilePath), false, tab->path);
			break;
		}
		if(path.length() <= 0) {
			makefilePath = tab->path + m_config["makefileName"];
			std::string makefileText = util::replaceAnyWith(m_makefileTemplate, "__SRCDIR__", util::getDirOfPath(makefilePath,false));
			Textfile::saveFile(makefileText, makefilePath);
			addTab(m_config["makefileName"], makefileText, false, tab->path);
			break;
		}
		path = util::getDirOfPath(path, false);
	}
}

void Application::quit() {
	Gtk::Main::quit();
}
void Application::undo() {
	cout << "UNDO" << endl;
	auto tab = getCurrentTab();
	if(tab->view.canUndo()) {
		tab->view.undo();
	}
}
void Application::redo() {
	cout << "REDO" << endl;
	auto tab = getCurrentTab();
	if(tab->view.canRedo()) {
		tab->view.redo();
	}
}
void Application::cut() {
	cout << "CUT" << endl;
	auto tab = getCurrentTab();
	tab->view.cutClipboard();
}
void Application::copy() {
	cout << "COPY" << endl;
	auto tab = getCurrentTab();
	tab->view.copyClipboard();
}
void Application::paste() {
	cout << "PASTE" << endl;
	auto tab = getCurrentTab();
	tab->view.pasteClipboard();
}
void Application::closeBrace() {
	cout << "CLOSE BRACE" << endl;
	auto tab = getCurrentTab();
	tab->view.closeBrace();
}
void Application::autoIndent() {
	cout << "AUTO INDENT" << endl;
	auto tab = getCurrentTab();
	tab->view.autoIndentSelection();
}
void Application::findReplace() {
	cout << "FIND/REPLACE" << endl;
}
void Application::showHelp() {
	cout << "HELP" << endl;
	Gtk::MessageDialog dialog(*this, "Ace Help", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CLOSE);
	dialog.set_secondary_text(s_sDefaultHelp);
	dialog.run();
}
void Application::showAbout() {
	cout << "ABOUT" << endl;
	Gtk::MessageDialog dialog(*this, "Ace About", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CLOSE);
	dialog.set_secondary_text(s_sDefaultAbout);
	dialog.run();
}

//---------------------------------------------------
const std::string Application::s_sDefaultAbout =
	R"(Default About)";
const std::string Application::s_sDefaultHelp =
	R"(Default Help)";
const std::string Application::s_sDefaultMakefile =
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
const std::string Application::s_sDefaultUi =
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
const std::string Application::s_sDefaultConfig =
	R"(programName=Ace Version 0.1
aceExec='/home/swante/workspace/Ace/Ace'
iconPath=res/icons/icon.png
width=600
height=600
maximized=0
fullscreen=0
vpanedSeperatorPosition=500
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

//where-am-i = $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
//SRCDIR := $(call where-am-i)

}
