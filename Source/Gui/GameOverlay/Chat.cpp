#include "Constants.h"
#include "Chat.h"
#include "GameOverlay.h"
#include "Game/Game.h"

Chat::Chat(OverworldGame *game, GraphicsContext *context, GameOverlay *gameOverlay) :
	UiObject(gameOverlay),
	m_game(game),
	m_active(false),
	m_chatInput(0),
	m_fadeTime(0.0f),
	m_font(Game::GetInstance()->getResourceManager()->get<Font>("Fonts/Chat")),
	m_redrawText(true),
	m_textSpriteBatch(context),
	m_messages(100),
	m_chatLinePos(0)
{
	setAnchor(Vector2F(0.0f, 0.4f));
	setSize(Vector2F(CHAT_WIDTHF, CHAT_HEIGHTF) / gameOverlay->getSize());
	setPosition(Vector2F(0.01f, 0.0f));

	/*m_chatInput.setAnchor(Vector2F(0.0f, 1.0f));
	m_chatInput.setSize(Vector2F(CHAT_WIDTHF, CHAT_TEXT_INPUT_HEIGHTF) / getSize());
	m_chatInput.setPosition(Vector2F(0.0f, 0.0f));
	*/
	m_chatRenderTarget = new RenderTarget2D(CHAT_WIDTH, CHAT_HEIGHT);
	m_font->setColor(Color(255, 255, 255, 255));
}

void Chat::toggle(int action)
{
	//if(action != GLFW_PRESS) return;
	m_active = !m_active;

	if(m_active)
	{
		//Input::setContext(Input::getContext("chat"));
		m_fadeTime = 0.0f;
	}
	else
	{
		//Input::setContext(Input::getContext("game"));
		m_fadeTime = CHAT_FADE_TIME;
	}
}

void Chat::sendMessage(int action)
{
	if(!m_active /*|| action != GLFW_PRESS*/) return;

	string chatStr;// = m_chatInput.getText();
	if(!chatStr.empty())
	{
		if(chatStr[0] == '/')
		{
			m_game->getCommander()->execute(chatStr.substr(1));
		}
		else
		{
			// TODO: Get player name and insert
			insertMessage("Bitsauce: " + chatStr);
		}
		m_redrawText = true;
	}
	//m_chatInput.setText("");
	toggle(action);
}

void Chat::insertMessage(const string & message)
{
	m_messages[m_chatLinePos++ % 100] = message;
}

void Chat::onTick(TickEvent *e)
{
	//setSize(Vector2F(CHAT_WIDTHF, CHAT_HEIGHTF) / m_parent->getSize());
	//m_chatInput.setSize(Vector2F(CHAT_WIDTHF, CHAT_TEXT_INPUT_HEIGHTF) / getSize());

	//m_chatInput.setActive(m_active);
	//m_chatInput.onTick(e);
	if(!m_active)
	{
		m_fadeTime -= e->getDelta();
	}
}

void Chat::onDraw(DrawEvent *e)
{
	if(!m_active && m_fadeTime <= 0.0f) return;

	Vector2I position = getPosition();
	Vector2I size = Vector2F(CHAT_WIDTHF, CHAT_HEIGHTF);

	SpriteBatch *spriteBatch = (SpriteBatch*) e->getUserData();
	GraphicsContext *context = e->getGraphicsContext();

	if(m_redrawText)
	{
		// Set render target
		context->setRenderTarget(m_chatRenderTarget);

		// Clear render target
		context->setBlendState(BlendState(BlendState::BLEND_ZERO, BlendState::BLEND_ZERO));
		context->drawRectangle(Vector2F(0.0f, 0.0f), size);

		// Create chat string
		string chatStr;
		int lineCount = (int) ceil(CHAT_HEIGHTF / m_font->getStringHeight(" "));
		for(int i = 0; i < lineCount; ++i)
		{
			chatStr += m_messages[i] + "\n";
		}

		// Draw text to render target
		m_textSpriteBatch.begin(SpriteBatch::State(SpriteBatch::DEFERRED, BlendState(BlendState::BLEND_ONE, BlendState::BLEND_ONE)));
		m_font->drawBox(&m_textSpriteBatch, Vector2F(5.0f), size.x - 10.0f, chatStr, 0, FONT_ALIGN_JUSTIFY);
		m_textSpriteBatch.end();

		// Reset render target
		context->setRenderTarget(0);

		// Set redraw flag
		m_redrawText = false;
	}

	// Get fade opacity
	float opacity = 1.0f;
	if(m_fadeTime > 0.0f)
	{
		opacity = min((m_fadeTime * 2.0f) / CHAT_FADE_TIME, 1.0f);
	}

	// Draw chat rectangle and text
	context->drawRectangle(position, size - Vector2F(0.0f, 30.0f), Color(0, 0, 0, uchar(127 * opacity)));
	context->setTexture(m_chatRenderTarget->getTexture());
	context->drawRectangle(position, size, Color(255, 255, 255, uchar(255 * opacity)));
	context->setTexture(0);

	// If fade time == 0.0f we draw the chat text input
	if(m_fadeTime <= 0.0f)
	{
		//m_chatInput.draw(spriteBatch);
	}
}