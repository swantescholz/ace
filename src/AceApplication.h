#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <gtkmm.h>
#include <iostream>
#include <thread>
#include <memory>
#include "AceSourceView.h"
#include "AceTextfile.h"
#include "AceIpc.h"
using namespace Gtk;
using namespace std;

namespace ace {

struct Tab {
	std::string name, path;
	bool        isnew;
	Gtk::ScrolledWindow             window;
	SourceView                      view;
	//Glib::RefPtr<Gtk::TextBuffer>   buffer;
};

struct Terminal {
	Gtk::ScrolledWindow             window;
	SourceView                      view;
	//Glib::RefPtr<Gtk::TextBuffer>   buffer;
};

class Application : public Gtk::Window {
public:
	Application();
	~Application();
public:
	void actNewFile     () {newFile    ();}
	void actOpen        () {openFile   ();}
	void actSave        () {saveFile   ();}
	void actSaveAs      () {saveAsFile ();}
	void actClose       () {closeFile  ();}
	void actQuit        () {quit       ();}
	void actUndo        () {undo       ();}
	void actRedo        () {redo       ();}
	void actCut         () {cut        ();}
	void actCopy        () {copy       ();}
	void actPaste       () {paste      ();}
	void actCloseBrace  () {closeBrace ();}
	void actNewline     () {addNewline ();}
	void actAutoIndent  () {autoIndent ();}
	void actFindReplace () {findReplace();}
	void actFullscreen  () {if(m_fullscreen) unfullscreen(); else fullscreen(); m_fullscreen = !m_fullscreen;}
	void actMaximize    () {if(m_maximized ) unmaximize  (); else maximize  (); m_maximized  = !m_maximized;}
	void actHelp        () {showHelp  ();}
	void actAbout       () {showAbout ();}
	void actBuild       () {buildProject();}
	void actRun         () {runProject();}
	void actOpenMakefile() {openMakefile();}
	void actReloadConfigFile() {std::cout << "RELOAD CONFIG FILE" << std::endl; loadConfigFile();}
private:
	void newFile();
	void openFile();
	void saveFile();
	void saveAsFile();
	void closeFile();
	void quit();
	void undo();
	void redo();
	void cut();
	void copy();
	void paste();
	void closeBrace();
	void addNewline();
	void autoIndent();
	void findReplace();
	void showHelp();
	void showAbout();
	void buildProject();
	void runProject();
	void openMakefile();
private:
	Glib::RefPtr<Gtk::TextBuffer> createTextBuffer();
	void init();
	void loadConfigFile(const std::string& name = "config/config.txt");
	void loadSourceViewOptions(SourceView& view);
	void loadIcons(const std::string& start, const std::string& end);
	void addTab(const std::string& tabname, const std::string& text, bool isnew = true, const std::string& tabpath = "");
	void openFifo();
	void checkFifo();
	std::shared_ptr<Tab> getCurrentTab();
	bool timeCallback();
	void updateTabLanguage(std::shared_ptr<Tab>& tab);
	void updateLastOpened(); //saves the last opened tabs and accessed paths
	
	Glib::RefPtr<Gtk::ActionGroup>  m_actionGroup;
	Glib::RefPtr<Gtk::UIManager>    m_uiManager;
	Gtk::VBox                       m_vbox;
	Gtk::Notebook                   m_notebook;
	Terminal                        m_terminal;
	Gtk::VPaned                     m_vpaned;
	std::list<std::shared_ptr<Tab>> m_tabs;
	Glib::RefPtr<Glib::TimeoutSource> m_timeoutSource;
	
	std::string m_fifoName;
	std::string m_textForTerminal;
	
	int m_fifoNum;
	FILE* m_fifo;
	char* m_chBuffer;
	int m_chBufferSize;
	bool m_stopFifoThread;
	std::thread *m_fifoThread;
	std::map<std::string, std::string> m_config;
	bool m_maximized, m_fullscreen;
	std::string m_lastAccessedDir;
	std::string m_makefileTemplate;
	
	static const std::string s_sDefaultAbout, s_sDefaultHelp, s_sDefaultMakefile, s_sDefaultUi, s_sDefaultConfig;
};

}

#endif /* APPLICATION_H_ */
