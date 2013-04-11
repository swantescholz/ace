#include "Application.h"
#include "Util.h"
#include "String.h"
#include "Assert.h"
#include "Constants.h"
#include <unistd.h>

using namespace Gtk;

namespace ace {

#define mymemfun(X) sigc::mem_fun(*this, &Application::X)


template<class... T>
Glib::RefPtr<Action> actCreate(T... args) {
	return Action::create(args...);
}

Application::Application(std::string aceExecName) {
	m_rootPath = util.getDirOfPath(aceExecName, true);
	m_config["aceExec"] = aceExecName;
	
	init();
	initGui();
}

Application::~Application() {
	updateLastOpened();
	stopFifoThread();
}

void Application::stopFifoThread() {
	m_stopFifoThread = true;
	FILE* fifo = fopen(m_fifoName.c_str(), "w");
	m_fifoThread->join();
	fclose(fifo);
	util.removeFile(m_fifoName);
	delete m_fifoThread; m_fifoThread = nullptr;
}

void Application::initGui() {
	m_actionGroup = ActionGroup::create();
	auto adda = [&](Glib::RefPtr<Action> action) {
		m_actionGroup->add( action);
	};
	auto addb = [&](Glib::RefPtr<Action> action, decltype(mymemfun(newFile)) fun) {
		m_actionGroup->add( action, fun);
	};
	auto addc = [&](Glib::RefPtr<Action> action, decltype(mymemfun(newFile)) fun, string key) {
		m_actionGroup->add( action, AccelKey(key), fun);
	};
	adda(actCreate("MenuFile"       , "_File"   ));
	adda(actCreate("MenuEdit"       , "_Edit"   ));
	adda(actCreate("MenuView"       , "_View"   ));
	adda(actCreate("MenuProject"    , "_Project"));
	adda(actCreate("MenuHelp"       , "_Help"   ));
	addb(actCreate("FileNew"        , Stock::NEW    ), mymemfun(newFile));
	addb(actCreate("FileOpen"       , Stock::OPEN   ), mymemfun(openFile   ));
	addb(actCreate("FileSave"       , Stock::SAVE   ), mymemfun(saveFile   ));
	addc(actCreate("FileSaveAs"     , Stock::SAVE_AS), mymemfun(saveAsFile ), "<control><shift>s");
	addb(actCreate("FileClose"      , Stock::CLOSE  ), mymemfun(closeFile  ));
	addb(actCreate("FileQuit"       , Stock::QUIT   ), mymemfun(quit   ));
	addc(actCreate("EditUndo"       , Stock::UNDO   ), mymemfun(undo   ), "<control>z");
	addc(actCreate("EditRedo"       , Stock::REDO   ), mymemfun(redo   ), "<control>y");
	addb(actCreate("EditCut"        , Stock::CUT    ), mymemfun(cut    ));
	addb(actCreate("EditCopy"       , Stock::COPY   ), mymemfun(copy   ));
	addb(actCreate("EditPaste"      , Stock::PASTE  ), mymemfun(paste  ));
	addc(actCreate("EditCloseBrace" ,"Close Brace"  ), mymemfun(closeBrace), "braceright");
	addc(actCreate("EditAutoIndent" , Stock::INDENT, "Auto Indent" ), mymemfun(autoIndent), "<control>i");
	addb(actCreate("EditFindReplace", Stock::FIND_AND_REPLACE), mymemfun(findReplace ));
	addc(actCreate("EditReloadConfigFile", Stock::PREFERENCES, "Reload Config File"), mymemfun(loadConfigFile), "<control><shift>r");
	auto maximizeAction = ToggleAction::create("ViewMaximize", "Maximize", "Maximize Window", m_maximized ); 
	auto fullscreenAction = ToggleAction::create("ViewFullscreen", Stock::FULLSCREEN,"",""    , m_fullscreen );
	auto buildAction = Action::create_with_icon_name("ProjectBuild", "build", "Build", "Build current project" );
	auto runAction = Action::create_with_icon_name("ProjectRun"  , "run"  , "Run"  , "Run current project"  );
	addc( maximizeAction  , mymemfun(actMaximize  ), "F12");
	addc( fullscreenAction, mymemfun(actFullscreen), "F11");
	addc( buildAction     , mymemfun(buildProject  ), "F5");
	addc( runAction       , mymemfun(runProject    ), "F6");
	addc(actCreate("ProjectEditMakefile", "_Edit Makefile"), mymemfun(openMakefile ), "F8");
	addc(actCreate("HelpHelp"    , Stock::HELP   ), mymemfun(showHelp) , "F1");
	addc(actCreate("HelpAbout"   , Stock::ABOUT  ), mymemfun(showAbout), "F9");
	Glib::signal_timeout().connect(mymemfun(timeCallback), 10 );
	
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
	m_notebook.signal_switch_page().connect(mymemfun(actSwitchPage));
	m_vpaned.add1(m_notebook);
	m_vpaned.add2(m_terminal.window);
	m_vbox.pack_start(m_vpaned);
	this->add(m_vbox);
	int width = 0, height = 0;
	get_size(width,height);
	m_vpaned.set_position(height * (1.0-util.lex<double>(m_config["terminalRelativeHeight"])));
	if(m_tabs.empty()) {addTab("NewFile", "", true);}
	show_all_children();
}

void Application::init() {
	m_uiManager = UIManager::create();
	openFifo();
	loadConfigFile();
	loadIcons("res/icons/", ".png");
	std::vector<std::string> stabs;
	Textfile::readFile(constants.lastOpenedTabsPath, stabs);
	for(auto s : stabs) {
		if(!s.empty()) {
			try {
				auto text = Textfile::readFile(s);
				addTab(util.getFileOfPath(s), text, false, util.getDirOfPath(s));
			} catch (shared_ptr<AssertionException> ex) {
				cout << "unable to open previously opened tab " << s << endl;
				cout << ex->toString() << endl;
			}
		}
	}
}

void validateConfigFiles() {
	auto checkFile = [](string filename, string defaultText) {
		if(!util.fileExists(filename)) {
			Textfile::saveFile(defaultText, filename);
		}
	};
	checkFile(constants.configPath, constants.defaultConfig);
	checkFile(constants.lastOpenedPathPath, "~");
	checkFile(constants.lastOpenedTabsPath, "");
	checkFile(constants.uiPath, constants.defaultUi);
	checkFile(constants.makefileTemplatePath, constants.defaultMakefile);
}

void Application::loadConfigFile() {
	cout << "(RE)LOAD CONFIG FILE" << endl;
	validateConfigFiles();
	
	auto text = Textfile::readFile(constants.configPath);
	util.mapConfigFile(m_config, text, true, true);
	m_maximized  = (util.lex<int>(m_config["maximized" ]) != 0);
	m_fullscreen = (util.lex<int>(m_config["fullscreen"]) != 0);
	set_title(m_config["programName"]);
	set_icon_from_file(m_config["iconPath"]);
	set_size_request(util.lex<int>(m_config["width"]), util.lex<int>(m_config["height"]));
	if(m_maximized ) maximize();
	if(m_fullscreen) fullscreen();

	m_lastAccessedDir = Textfile::readFile(constants.lastOpenedPathPath);
	m_uiManager->add_ui_from_string(Textfile::readFile(constants.uiPath));
	m_makefileTemplate = Textfile::readFile(constants.makefileTemplatePath);
}

int getSmartHomeEndModeForIndex(int index) {
	switch (index) {
		case 1: return (int)GTK_SOURCE_SMART_HOME_END_DISABLED;
		case 2: return (int)GTK_SOURCE_SMART_HOME_END_BEFORE;
		case 3: return (int)GTK_SOURCE_SMART_HOME_END_AFTER;
		case 4: return (int)GTK_SOURCE_SMART_HOME_END_ALWAYS;
	}
	return GTK_SOURCE_SMART_HOME_END_BEFORE;
}
int getWrapModeForIndex(int index) {
	switch (index) {
		case 1: return (int)GTK_WRAP_NONE;
		case 2: return (int)GTK_WRAP_CHAR;
		case 3: return (int)GTK_WRAP_WORD;
		case 4: return (int)GTK_WRAP_WORD_CHAR;
	}
	return (int)GTK_WRAP_WORD_CHAR;
}
void Application::loadSourceViewOptions(SourceView& view) {
	view.setHighlightMatchingBrackets(util.lex<int>(m_config["highlightMatchingBrackets"])==1);
	view.setMaxUndoLevels (util.lex<int>(m_config["maxUndoLevels"]));
	view.setSmartIndent   (util.lex<int>(m_config["smartIndent"])==1);
	view.setIndentOnTab   (util.lex<int>(m_config["indentOnTab"])==1);
	view.setIndentString  (m_config["indentString"]);
	view.setTabWidth      (util.lex<int>(m_config["tabWidth"]));
	view.setTabsAreSpaces (util.lex<int>(m_config["tabsAreSpaces"])==1);
	view.setHighlightCurrentLine(util.lex<int>(m_config["highlightCurrentLine"])==1);
	view.setShowLineNumbers     (util.lex<int>(m_config["showLineNumbers"])==1);
	view.setRightMarginPosition (util.lex<int>(m_config["rightMarginPosition"]));
	view.setShowRightMargin     (util.lex<int>(m_config["showRightMargin"])==1);
	int smartHomeEndIndex = util.lex<int>(m_config["smartHomeEnd"]);
	int wrapIndex = util.lex<int>(m_config["wrapMode"]);
	view.setSmartHomeEnd(getSmartHomeEndModeForIndex(smartHomeEndIndex));
	view.setWrapMode(getWrapModeForIndex(wrapIndex));
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
	tab->view.setModified(false);
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
	int index = 1;
	bool mkfifoFailed;
	do {
		m_fifoName = m_rootPath + ACE_FIFO_NAME + util.lex(index);
		m_fifoNum  = index;
		cout << "FIFO_NAME: " << m_fifoName << endl;
		int result = mkfifo(m_fifoName.c_str(), S_IRUSR | S_IWUSR);
		mkfifoFailed = result < 0;
		++index;
	} while(mkfifoFailed);
	m_fifoThread = new std::thread([this](){this->checkFifo();});
}
void Application::checkFifo() {
	const int chBufferSize = 5000;
	auto chBuffer = new char[chBufferSize];
	while(true) {
		FILE* fifo = fopen(m_fifoName.c_str(), "r");
		if(m_stopFifoThread) {
			break;
		}
		int numread = fread(chBuffer, 1, chBufferSize, fifo);
		fclose(fifo);
		if(numread <= 0) return;
		chBuffer[numread] = '\0';
		m_textForTerminal += std::string(chBuffer);
	}
	delete[] chBuffer; chBuffer = nullptr;
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
	if(tab->view.isModified()) {
		if(std::string(m_notebook.get_tab_label_text(tab->window)).find("*") ==  std::string::npos) {
			m_notebook.set_tab_label_text(tab->window, tab->name + "*"); //with *
		}
		tab->view.update();
	}
	return true;
}
void Application::updateTabLanguage(std::shared_ptr<Tab>& tab) {
	if(tab->name.find(constants.makefileName) == std::string::npos) {
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
//================================================
void Application::actSwitchPage(Widget* page, guint page_num) {
	auto tab = getCurrentTab();
	auto title = m_notebook.get_tab_label_text(tab->window);
	title += " (" + tab->path + ") - Ace";
	set_title(title);
}
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
		auto path = util.getDirOfPath (filename);
		auto name = util.getFileOfPath(filename);
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
	if (tab->view.isModified()) {
		Textfile::saveFile(tab->view.getText(), tab->path + tab->name);
		tab->view.setModified(false);
	}
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
		tab->path = util.getDirOfPath (filename);
		tab->name = util.getFileOfPath(filename);
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
	auto makefileName = constants.makefileName;
	auto path = tab->path;
	path.erase(path.length()-1);
	while(true) {
		auto makefilePath = path+"/"+makefileName;
		if(util.fileExists(makefilePath)) {
			util.system("make -C " + path + " -f " + makefileName + " 2>&1 | " + m_config["aceExec"] +
				" --self" + util.lex(m_fifoNum) + " &");
			break;
		}
		if(path.length() <= 0) {
			makefilePath = tab->path + makefileName;
			std::string makefileText = String(m_makefileTemplate).replaceAll("__SRCDIR__", util.getDirOfPath(makefilePath,false));
			Textfile::saveFile(makefileText, makefilePath);
			addTab(makefileName, makefileText, false, tab->path);
			break;
		}
		path = util.getDirOfPath(path, false);
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
		makefilePath = path + "/" + constants.makefileName;
		if(util.fileExists(makefilePath)) {
			std::string cmd = "make -C " + path + " -f " + constants.makefileName +
				" run 2>&1 | " + m_config["aceExec"];
			cmd += " --self" + util.lex(m_fifoNum) + " &";
			cout << "exec: " << cmd << endl;
			util.system(cmd);
			break;
		}
		if(path.length() <= 0) {
			m_textForTerminal = "No Makefile/Executable to run found";
			break;
		}
		path = util.getDirOfPath(path, false);
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
		makefilePath = path + "/" + constants.makefileName;
		if(util.fileExists(makefilePath)) {
			addTab(constants.makefileName, Textfile::readFile(makefilePath), false, tab->path);
			break;
		}
		if(path.length() <= 0) {
			makefilePath = tab->path + constants.makefileName;
			std::string makefileText = String(m_makefileTemplate).replaceAll("__SRCDIR__", util.getDirOfPath(makefilePath,false));
			Textfile::saveFile(makefileText, makefilePath);
			addTab(constants.makefileName, makefileText, false, tab->path);
			break;
		}
		path = util.getDirOfPath(path, false);
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
	dialog.set_secondary_text(constants.defaultHelp);
	dialog.run();
}
void Application::showAbout() {
	cout << "ABOUT" << endl;
	Gtk::MessageDialog dialog(*this, "Ace About", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CLOSE);
	dialog.set_secondary_text(constants.defaultAbout);
	dialog.run();
}


}
