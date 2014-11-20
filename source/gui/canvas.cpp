#include "canvas.h"
#include "constants.h"

Canvas::Canvas() :
	UiObject(0)
{
	m_anchor.set(0.5f, 0.5f);
}

void Canvas::update()
{
	Vector2 size;
	if(XWindow::getSize().x < XWindow::getSize().y) {
		// Fit width and use inverse aspect ratio
		size.x = (float)min(CANVAS_WIDTH, XWindow::getSize().x);
		size.y = size.x*CANVAS_HEIGHT/CANVAS_WIDTH; 
	}
	else {
		// Fit height and use aspect ratio
		size.y = (float)min(CANVAS_HEIGHT, XWindow::getSize().y);
		size.x = size.y*CANVAS_WIDTH/CANVAS_HEIGHT; 
	}
	m_rect.size = size/Vector2(XWindow::getSize());
	//Debug::setVaraible("Canvas", "("+util::floatToStr(size.x)+", "+util::floatToStr(size.y)+")");
	UiObject::update();
}