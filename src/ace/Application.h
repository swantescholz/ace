#ifndef ACE_APPLICATION_H_
#define ACE_APPLICATION_H_

#include <gtkmm.h>
#include <iostream>
#include <thread>
#include <memory>
#include "SourceView.h"
#include "Textfile.h"
#include "Ipc.h"
using namespace Gtk;
using namespace std;

namespace ace {

struct Tab {
	std::string name, path;
	bool        isnew;
	Gtk::ScrolledWindow             window;
	SourceView                      view;
};

struct Terminal {
	Gtk::ScrolledWindow             window;
	SourceView                      view;
};

class Application : public Gtk::Window {
public:
	Application(std::string aceExecName);
	~Application();
public:

	void actFullscreen  () {if(m_fullscreen) unfullscreen(); else fullscreen(); m_fullscreen = !m_fullscreen;}
	void actMaximize    () {if(m_maximized ) unmaximize  (); else maximize  (); m_maximized  = !m_maximized;}
	void actSwitchPage(Widget* page, guint page_num);
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
	void initGui();
	void loadConfigFile();
	void loadSourceViewOptions(SourceView& view);
	void loadIcons(const std::string& start, const std::string& end);
	void addTab(const std::string& tabname, const std::string& text, bool isnew = true, const std::string& tabpath = "");
	void openFifo();
	void checkFifo();
	std::shared_ptr<Tab> getCurrentTab();
	bool timeCallback();
	void updateTabLanguage(std::shared_ptr<Tab>& tab);
	void updateLastOpened(); //saves the last opened tabs and accessed paths
	void clearTerminal();	
	void stopFifoThread();
	
	Glib::RefPtr<Gtk::ActionGroup>  m_actionGroup;
	Glib::RefPtr<Gtk::UIManager>    m_uiManager;
	Gtk::VBox                       m_vbox;
	Gtk::Notebook                   m_notebook;
	Terminal                        m_terminal;
	Gtk::VPaned                     m_vpaned;
	std::list<std::shared_ptr<Tab>> m_tabs;
	Glib::RefPtr<Glib::TimeoutSource> m_timeoutSource;
	
	std::string m_rootPath;
	std::string m_fifoName;
	std::string m_textForTerminal;
	
	int m_fifoNum;
	bool m_stopFifoThread = false;
	std::thread *m_fifoThread = nullptr;
	std::map<std::string, std::string> m_config;
	bool m_maximized, m_fullscreen;
	std::string m_lastAccessedDir;
	std::string m_makefileTemplate;
	
};

}

#endif /* APPLICATION_H_ */
