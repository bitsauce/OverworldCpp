#include "button.h"
#include "constants.h"

Button::Button(string text, function<void()> onClick, UiObject *parent) :
	UiObject(parent),
	m_font(xd::ResourceManager::get<xd::Font>(UI_MENU_BUTTON_FONT)),
	m_btnTexture(xd::ResourceManager::get<xd::Texture2D>(UI_MENU_BUTTON_TEXTURE)),
	m_textTexture(0),
	m_animTime(0.0f),
	m_userData(0)
{
	// Set callback
	m_callback = onClick;

	// Set sprite
	m_buttonSprite = new xd::Sprite(m_btnTexture);
		
	// Set text
	setText(text);
}
	
/*Button::Button(string text, function<void(Button*)> onClick, UiObject *parent) :
	UiObject(parent),
	m_font(ResourceManager::getFont("font_large")),
	m_animTime(0.0f),
	m_userData(0)
{
	// Set callback
	m_callbackThis = onClick;

	// Set sprite
	m_buttonSprite = new xd::Sprite(xd::TextureRegion());
		
	// Set text
	setText(text);
}*/

Button::~Button()
{
	delete m_buttonSprite;
}
	
void Button::setText(string text)
{
	// Set text
	m_text = text;
		
	// Store text render texture
	m_font->setColor(xd::Color(255, 255, 255, 255));
	m_textTexture = nullptr; // Flag for re-draw
}

string Button::getText() const
{
	return m_text;
}

void Button::setUserData(void *userData)
{
	m_userData = userData;
}

void *Button::getUserData() const
{
	return m_userData;
}

void Button::update()
{
	if(isHovered())
	{
		m_animTime = min(m_animTime + xd::Graphics::getTimeStep()*4.0f, 1.0f);
	}
	else
	{
		m_animTime = max(m_animTime - xd::Graphics::getTimeStep()*4.0f, 0.0f);
	}
	UiObject::update();
}
	
void Button::draw(xd::SpriteBatch *spriteBatch)
{
	//if(!m_textTexture) m_textTexture = m_font->renderToTexture(spriteBatch->getGraphicsContext(), text, 128);

	// Get size and position
	Vector2 position = getPosition();
	Vector2 size = getSize();

	// Draw button sprite
	m_buttonSprite->setSize(size);
	m_buttonSprite->setPosition(position);

	// Draw hovered sprite
	if(m_animTime > 0.0f)
	{
		m_buttonSprite->setColor(xd::Color(255, 255, 255, m_animTime));
		m_buttonSprite->setRegion(xd::TextureRegion(0.0f, 0.0f, 1.0f, 0.5f));
		spriteBatch->drawSprite(*m_buttonSprite);
	}

	// Draw default sprite
	m_buttonSprite->setColor(xd::Color(255, 255, 255, 255));
	m_buttonSprite->setRegion(xd::TextureRegion(0.0f, 0.5f, 1.0f, 1.0f));
	spriteBatch->drawSprite(*m_buttonSprite);
		
	// Draw text
	m_font->setColor(xd::Color(0, 0, 0, 255));
	spriteBatch->drawText(position - (Vector2(m_font->getStringWidth(m_text), m_font->getStringHeight(m_text))-size)*0.5f, m_text, m_font);
}
	
void Button::clickEvent()
{
	if(m_callback) {
		m_callback();
	}

	if(m_callbackThis) {
		m_callbackThis(this);
	}
}
