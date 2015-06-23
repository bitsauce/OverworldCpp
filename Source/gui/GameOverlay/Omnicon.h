#ifndef OMNICON_H
#define OMNICON_H

#include "Config.h"
#include "Gui/UiObject.h"
//>REMOVE ME<
class GameOverlay;

class Omnicon : public UiObject
{
public:
	Omnicon(Scene *scene, GameOverlay *gameOverlay);

	void toggle();
	void setVisible(const bool visible) { m_visible = visible; }
	bool isVisible() const { return m_visible; }

	void update(const float delta);
	void draw(SpriteBatch *spriteBatch, const float alpha);

private:
	GameOverlay *m_gameOverlay;
	Sprite m_bookSprite;
	bool m_visible;
};

#endif // OMNICON_H