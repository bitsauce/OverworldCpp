#ifndef LINE_EDIT_H
#define LINE_EDIT_H

#include "game.h"
#include "uiobject.h"

class UiObject;

class LineEdit : public UiObject//, public KeyboardListener
{
public:
	LineEdit(UiObject *parent = nullptr);

	void setAcceptFunc(function<void()> func) { m_acceptFunc = func ; }

	void setText(const string &text);
	string getText() const;

	void update();
	void draw(XBatch *batch);

private:
	void insertAt(const uint at, const string &str);
	void removeAt(const uint at);
	void charEvent(uint code);
	void keyPressed(XVirtualKey key);
	void keyReleased(XVirtualKey key);

	xd::Resource<XFont> m_font;
	string m_text;
	int m_cursorPos;
	float m_cursorTime;
	function<void()> m_acceptFunc;
};

#endif // LINE_EDIT_H