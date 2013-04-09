#ifndef ACESOURCEVIEW_H_
#define ACESOURCEVIEW_H_

#include <gtkmm.h>
//#include <gdk/gdkkeysyms.h> // includes GDK_q
#include <gtk/gtk.h>
#include <string>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourceview.h>

namespace ace {

class SourceView {
public:
	SourceView();
	~SourceView();
	
	GtkWidget* gobj() {return editor;}
	void addToContainer(GtkWidget* container);
	void addToContainer(Gtk::Widget& container);
	
	void setLanguageByFile(std::string filename);
	void setLanguageById  (std::string id);
	void setFont(std::string name);
	void setText(const std::string& text);
	std::string getText();
	
	bool canUndo();
	bool canRedo();
	void undo();
	void redo();
	void copyClipboard();
	void cutClipboard();
	void pasteClipboard();
	void beginUndoableAction();
	void endUndoableAction();
	bool isModified(bool setFalse = true);
	void closeBrace(); //unindent when }
	void update(); //check for autoindent after return/enter pressed
	void autoIndentSelection();
	
	void setHighlightMatchingBrackets(bool b = true);
	void setMaxUndoLevels(int num);
	void setSmartIndent (bool b = true);
	void setIndentOnTab(bool b = true);
	void setIndentString(std::string str);
	void setTabWidth   (int  w);
	void setTabsAreSpaces       (bool b = true);
	void setHighlightCurrentLine(bool b = true);
	void setShowLineNumbers     (bool b = true);
	void setRightMarginPosition (int  r);
	void setShowRightMargin     (bool b = true);
	void setSmartHomeEnd        (int mode);
	void setWrapMode            (int mode);
	void setEditable            (bool b = true);
	
	bool key_press_event(GtkWidget* widget, GdkEventKey* event);
private:
	std::string getIndentationOfLine(const GtkTextIter& iter);
	char        getLastNonwhiteCharOfLine(const GtkTextIter& iter);
	char        getFirstNonwhiteCharOfLine(const GtkTextIter& iter);
	void        deleteWhitespaceInFrontOfLine(const GtkTextIter& iter);
	void        getLines(const GtkTextIter& a, const GtkTextIter& b, std::vector<std::string> lines);
	std::string getCorrectIndentationOfLine(const GtkTextIter& iter);
	
	GtkWidget* editor;
	GtkSourceBuffer* buffer;
	GtkSourceLanguageManager* slm;
	GtkClipboard* clipboard;
	bool doSmartIndent;
	int lastNumLines, lastNumChars;
	std::string extraIndentation;
};

}
#endif /* ACESOURCEVIEW_H_ */

