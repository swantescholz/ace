#include "AceSourceView.h"
#include <iostream>
#include "AceUtil.h"

namespace ace {

bool SourceView::key_press_event(GtkWidget* widget, GdkEventKey* event) {
	std::cout << "key_press_event" << std::endl;
	return true;
}

SourceView::SourceView() {
	clipboard = gtk_clipboard_get(GDK_NONE);
	buffer = gtk_source_buffer_new(0);
	editor = gtk_source_view_new_with_buffer(buffer);
	slm    = gtk_source_language_manager_get_default();
	lastNumLines = lastNumChars = 0;
	gtk_source_view_set_auto_indent(GTK_SOURCE_VIEW(editor), false); //self implemented auto/smart-indent!
	gtk_source_view_set_indent_width(GTK_SOURCE_VIEW(editor), -1);
	setHighlightMatchingBrackets(true);
	setMaxUndoLevels(30);
	setSmartIndent(true);
	setIndentOnTab(true);
	setIndentString("\t");
	setTabWidth(4);
	setTabsAreSpaces(false);
	setHighlightCurrentLine(true);
	setShowLineNumbers(true);
	setRightMarginPosition(80);
	setShowRightMargin(true);
	setSmartHomeEnd(GTK_SOURCE_SMART_HOME_END_BEFORE);
	setWrapMode(GTK_WRAP_WORD_CHAR);
	setFont("monospace");
}

SourceView::~SourceView() {
	
}

void SourceView::addToContainer(Gtk::Widget& container) {
	gtk_container_add(GTK_CONTAINER(container.gobj()), editor);
}

void SourceView::setHighlightMatchingBrackets(bool b) {
	gtk_source_buffer_set_highlight_matching_brackets(buffer, b);
}
void SourceView::setMaxUndoLevels(int num) {
	gtk_source_buffer_set_max_undo_levels(buffer, num);
}
bool SourceView::canUndo() {
	return gtk_source_buffer_can_undo(buffer);
}
bool SourceView::canRedo() {
	return gtk_source_buffer_can_redo(buffer);
}
void SourceView::undo() {
	gtk_source_buffer_undo(buffer);
}
void SourceView::redo() {
	gtk_source_buffer_redo(buffer);
}
void SourceView::copyClipboard() {
	gtk_text_buffer_copy_clipboard(GTK_TEXT_BUFFER(buffer), clipboard);
}
void SourceView::cutClipboard() {
	gtk_text_buffer_cut_clipboard(GTK_TEXT_BUFFER(buffer), clipboard, true);
}
void SourceView::pasteClipboard() {
	GtkTextIter start, end;
	gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(buffer), &start, &end);
	gtk_text_buffer_paste_clipboard(GTK_TEXT_BUFFER(buffer), clipboard, &start, true);
}
void SourceView::beginUndoableAction() {
	gtk_source_buffer_begin_not_undoable_action(buffer);
}
void SourceView::endUndoableAction() {
	gtk_source_buffer_end_not_undoable_action(buffer);
}
bool SourceView::isModified(bool setFalse) {
	bool b = gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(buffer));
	if(b && setFalse) {
		gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(buffer), false);
	}
	return b;
}
void SourceView::closeBrace() {
	GtkTextBuffer* tbuf = GTK_TEXT_BUFFER(buffer);
	GtkTextIter a,b,c;
	gtk_text_buffer_get_iter_at_mark(tbuf, &b, gtk_text_buffer_get_insert(tbuf));
	c = a = b;
	std::string sold, snew;
	for(;;) {
		if(gtk_text_iter_is_start(&a)) {
			break;
		}
		gtk_text_iter_backward_char(&a);
		char ch = gtk_text_iter_get_char(&a);
		if(ch == '\n') {
			gtk_text_iter_forward_char(&a);
			break;
		}
		sold += ch;
	}
	if(util::isStringWhitespace(sold)) { //delete
		GtkTextMark* mark = gtk_text_buffer_create_mark(tbuf, "mymark", &b, false);
		gtk_text_buffer_delete(tbuf, &a, &b);
		int numClosed = 1;
		for(;;) {
			if(gtk_text_iter_is_start(&a)) {
				snew = sold;
				break;
			}
			gtk_text_iter_backward_char(&a);
			char ch = gtk_text_iter_get_char(&a);
			if(ch == '}') {
				++numClosed;
			}
			else if(ch == '{') {
				--numClosed;
				if(numClosed == 0) {
					snew = getIndentationOfLine(a);
					break;
				}
			}
		}
		gtk_text_buffer_get_iter_at_mark(tbuf, &c, mark);
		gtk_text_buffer_delete_mark(tbuf, mark);
	}
	snew += "}";
	gtk_text_buffer_insert(tbuf, &c, snew.c_str(), snew.length());
}
std::string SourceView::getIndentationOfLine(const GtkTextIter& iter) {
	GtkTextIter it = iter;
	std::string sindent;
	for(;;) {
		if(gtk_text_iter_is_start(&it)) {
			break;
		}
		gtk_text_iter_backward_char(&it);
		char ch = gtk_text_iter_get_char(&it);
		if(ch == '\n') {
			break;
		}
	}
	for(;;) {
		gtk_text_iter_forward_char(&it);
		char ch = gtk_text_iter_get_char(&it);
		if(!util::isStringWhitespace(std::string("")+ch) ||
			ch == '\n' || gtk_text_iter_is_end(&it) ) {
			break;
		}
		sindent += ch;
	}
	return sindent;
}
char SourceView::getLastNonwhiteCharOfLine(const GtkTextIter& iter) {
	GtkTextIter it = iter;
	for(;;) {
		char ch = gtk_text_iter_get_char(&it);
		if(ch == '\n' || gtk_text_iter_is_end(&it) ) {
			break;
		}
		gtk_text_iter_forward_char(&it);
	}
	for(;;) {
		gtk_text_iter_backward_char(&it);
		char ch = gtk_text_iter_get_char(&it);
		if(!util::isStringWhitespace(std::string("")+ch)) {
			return ch;
		}
		if(gtk_text_iter_is_start(&it) || ch == '\n') {
			break;
		}
	}
	return ' ';
}
char SourceView::getFirstNonwhiteCharOfLine(const GtkTextIter& iter) {
	GtkTextIter it = iter;
	for(;!gtk_text_iter_is_start(&it);) {
		gtk_text_iter_backward_char(&it);
		char ch = gtk_text_iter_get_char(&it);
		if(ch == '\n') {
			gtk_text_iter_forward_char(&it);
			break;
		}
	}
	for(;!gtk_text_iter_is_end(&it);) {
		char ch = gtk_text_iter_get_char(&it);
		if(ch == '\n') {
			break;
		}
		if(!util::isStringWhitespace(std::string("")+ch)) {
			return ch;
		}
		gtk_text_iter_forward_char(&it);
	}
	return ' ';
}
void SourceView::deleteWhitespaceInFrontOfLine(const GtkTextIter& iter) {
	auto* tbuf = GTK_TEXT_BUFFER(buffer);
	GtkTextIter a = iter;
	int line = gtk_text_iter_get_line(&iter);
	gtk_text_buffer_get_iter_at_line(tbuf, &a, line);
	GtkTextIter b = a;
	while(!gtk_text_iter_is_end(&b)) {
		char ch = gtk_text_iter_get_char(&b);
		if(ch == '\n') {
			break;
		}
		if(!util::isStringWhitespace(std::string("")+ch)) {
			break;
		}
		gtk_text_iter_forward_char(&b);
	}
	gtk_text_buffer_delete(tbuf, &a, &b);
}
void SourceView::update() {
	std::cout << "sv::update" << std::endl;
	auto* tbuf = GTK_TEXT_BUFFER(buffer);
	auto nlines = gtk_text_buffer_get_line_count(tbuf);
	auto nchars = gtk_text_buffer_get_char_count(tbuf);
	if(nlines == lastNumLines + 1 && nchars == lastNumChars + 1) {
		GtkTextIter a;
		gtk_text_buffer_get_iter_at_mark(tbuf, &a, gtk_text_buffer_get_insert(tbuf));
		gtk_text_iter_backward_char(&a);
		if(gtk_text_iter_get_char(&a) == '\n') {
			std::string s;
			s = getIndentationOfLine(a);
			char ch = getLastNonwhiteCharOfLine(a);
			if(ch == '{') {
				s += extraIndentation;
			}
			std::cout << "ch:" << ch << std::endl;
			std::cout << "s:"  << s  << std::endl;
			gtk_text_buffer_insert_at_cursor(tbuf, s.c_str(), s.length());
		}
	}
	lastNumLines = nlines;
	lastNumChars = nchars;
}

void SourceView::getLines(const GtkTextIter& a, const GtkTextIter& b, std::vector<std::string> lines) {
	
}

std::string SourceView::getCorrectIndentationOfLine(const GtkTextIter& iter) {
	
}

void SourceView::autoIndentSelection() {
	std::cout << "sv::autoIndentSelection" << std::endl;
	auto* tbuf = GTK_TEXT_BUFFER(buffer);
	gtk_text_buffer_begin_user_action(tbuf);
	GtkTextIter a,b;
	gtk_text_buffer_get_selection_bounds(tbuf, &a, &b);
	const int line1 = gtk_text_iter_get_line(&a);
	const int line2 = gtk_text_iter_get_line(&b);
	for(int i = line1; i <= line2; ++i) {
		gtk_text_buffer_get_iter_at_line(tbuf, &a, i);
		std::string s = "";
		char firstChar = getFirstNonwhiteCharOfLine(a);
		if(firstChar == '}') {
			int numClosed = 1;
			b = a;
			for(;!gtk_text_iter_is_start(&b);) {
				gtk_text_iter_backward_char(&b);
				char ch = gtk_text_iter_get_char(&b);
				if(ch == '{') --numClosed;
				if(ch == '}') ++numClosed;
				if(numClosed == 0) {
					s = getIndentationOfLine(b);
					deleteWhitespaceInFrontOfLine(a);
					gtk_text_buffer_get_iter_at_line(tbuf, &a, i);
					break;
				}
			}
		}
		else {
			int line = gtk_text_iter_get_line(&a);
			if(line >= 1) {
				gtk_text_buffer_get_iter_at_line(tbuf, &b, line-1);
				char lastChar = getLastNonwhiteCharOfLine(b);
				if(lastChar == '{') {
					s = getIndentationOfLine(b) + extraIndentation;
					deleteWhitespaceInFrontOfLine(a);
					gtk_text_buffer_get_iter_at_line(tbuf, &a, i);
				}
				else if (lastChar != ';') {
					s = "";
				}
				else {
					s = getIndentationOfLine(b);
					deleteWhitespaceInFrontOfLine(a);
					gtk_text_buffer_get_iter_at_line(tbuf, &a, i);
				}
			}
		}
		gtk_text_buffer_insert(tbuf, &a, s.c_str(), s.length());
	}
	gtk_text_buffer_end_user_action(tbuf);
}

void SourceView::setSmartIndent(bool b) {
	doSmartIndent = b;
}
void SourceView::setIndentOnTab(bool b) {
	gtk_source_view_set_indent_on_tab(GTK_SOURCE_VIEW(editor), b);
}
void SourceView::setIndentString(std::string str) {
	if(util::isStringWhitespace(str)) {
		extraIndentation = str;
	}
	else {
		extraIndentation = "/t";
		std::cout << "WARNING: wrong indent string (using tab instead)" << std::endl;
	}
}
void SourceView::setTabWidth(int w) {
	gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(editor), w);
}
void SourceView::setTabsAreSpaces(bool b) {
	gtk_source_view_set_insert_spaces_instead_of_tabs(GTK_SOURCE_VIEW(editor), b);
}
void SourceView::setHighlightCurrentLine(bool b) {
	gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(editor), b);
}
void SourceView::setShowLineNumbers(bool b) {
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(editor), b);
}
void SourceView::setRightMarginPosition(int r) {
	gtk_source_view_set_right_margin_position(GTK_SOURCE_VIEW(editor), r);
}
void SourceView::setShowRightMargin(bool b) {
	gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(editor), b);
}
void SourceView::setSmartHomeEnd(int mode) {
	gtk_source_view_set_smart_home_end(GTK_SOURCE_VIEW(editor), (GtkSourceSmartHomeEndType)mode);
}
void SourceView::setWrapMode(int mode) {
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(editor), (GtkWrapMode)mode);
}
void SourceView::setEditable(bool b) {
	gtk_text_view_set_editable(GTK_TEXT_VIEW(editor), b);
}
void SourceView::setLanguageByFile(std::string filename) {
	auto* lang = gtk_source_language_manager_guess_language(slm, filename.c_str(), 0);
	gtk_source_buffer_set_language(buffer, lang);
}
void SourceView::setLanguageById(std::string id) {
	auto* lang = gtk_source_language_manager_get_language(slm, id.c_str());
	gtk_source_buffer_set_language(buffer, lang);
}
void SourceView::setFont(std::string name) {
	PangoFontDescription *font_desc = pango_font_description_new();
	pango_font_description_set_family (font_desc, name.c_str());
	gtk_widget_modify_font (editor, font_desc);
}
void SourceView::setText(const std::string& text) {
	auto* tbuf = GTK_TEXT_BUFFER(buffer);
	gtk_text_buffer_set_text (tbuf, text.c_str(), text.length());
	lastNumLines = gtk_text_buffer_get_line_count(tbuf);
	lastNumChars = gtk_text_buffer_get_char_count(tbuf);
}
std::string SourceView::getText() {
	GtkTextIter start, end;
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
	gtk_text_buffer_get_end_iter  (GTK_TEXT_BUFFER(buffer), &end);
	return std::string(gtk_text_buffer_get_text (GTK_TEXT_BUFFER(buffer), &start, &end,true));
}


} /* namespace ace */
