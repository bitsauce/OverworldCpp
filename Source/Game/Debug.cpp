#include "BitStream.h"

#include "Debug.h"
#include "Constants.h"
#include "World/World.h"
#include "Entities/Player.h"
#include "Blocks/BlockData.h"
#include "BlockEntities/BlockEntityData.h"
#include "Networking/Server.h"
#include "Entities/Mobs/Zombie.h"
#include "Game/States/GameState.h"
#include "Game/Game.h"
#include "Gui/GameOverlay/GameOverlay.h"

#define DEBUG_FUNCTIONS_STRING \
	"F1: Toggle debug\n" \
	"F2: Toggle lighting\n" \
	"F3: Toggle overlays\n" \
	"F4: Toggle block painter\n" \
	"F5: Show chunk loader info\n" \
	"F6: Show lighting info\n" \
	"F7: Set time\n" \
	"F8: Spawn menu\n" \
	"F9: Multiplayer menu\n" \
	"F10: Detach/attach camera\n" \
	"F11: [No func]\n" \
	"F12: [No func]"

Debug::Debug(OverworldGame *game) :
	m_game(game),
	m_world(game->getWorld()),
	m_block(BLOCK_GRASS),
	m_enabled(false),
	m_debugChunkLoader(false),
	m_debugLighting(false),
	m_blockPainterEnabled(false),
	m_font(Game::GetInstance()->getResourceManager()->get<Font>("Fonts/Debug")),
	m_blockPainterTexture(new Texture2D())
{
	// Set font color
	m_font->setColor(Color(255, 255, 255, 255));
}

void Debug::debugFunction(KeyEvent *e)
{
	// Debug functions only called when debug enabled
	if(e->getType() != KeyEvent::DOWN || (!m_enabled && e->getKeycode() != CGF_KEY_F1))
	{
		return;
	}

	switch(e->getKeycode())
	{
		// Toggle debug
		case CGF_KEY_F1: toggle(); break;

		// Toggle lighting
		case CGF_KEY_F2:
		{
			m_world->getLighting()->m_enabled = !m_world->getLighting()->m_enabled;
		}
		break;

		// Toggle overlay
		case CGF_KEY_F3:
		{
			m_game->getGameOverlay()->m_hidden = !m_game->getGameOverlay()->m_hidden;
		}
		break;

		// Toggle block painter
		case CGF_KEY_F4:
		{
			m_blockPainterEnabled = !m_blockPainterEnabled;
		}
		break;

		// Toggle chunk loader info
		case CGF_KEY_F5:
		{
			m_debugChunkLoader = !m_debugChunkLoader;
			m_world->getTerrain()->getChunkManager()->m_applyZoom = !m_world->getTerrain()->getChunkManager()->m_applyZoom;
		}
		break;

		// Toggle lighting info
		case CGF_KEY_F6:
		{
			m_debugLighting = !m_debugLighting;
		}
		break;

		// Set time
		case CGF_KEY_F7:
		{
			m_world->getTimeOfDay()->setTime(m_world->getTimeOfDay()->getTime() + (m_game->getInputManager()->getKeyState(CGF_KEY_LSHIFT) ? -100 : 100));
		}
		break;

		// Show spawn menu
		case CGF_KEY_F8:
		{
			// Spawn light
			new Pointlight(m_world->getLighting(), math::floor(Vector2F(m_world->getCamera()->getInputPosition()) / BLOCK_PXF) + Vector2F(0.5f, 0.5f), 20, Color((uchar) m_random.nextInt(255), (uchar) m_random.nextInt(255), (uchar) m_random.nextInt(255), 255));
			// Spawn zombie

			// Create zombie
			/*RakNet::BitStream bitStream;
			bitStream.Write((RakNet::MessageID)ID_CREATE_ENTITY);
			bitStream.Write(ENTITY_ZOMBIE);
			((Server*)Connection::getInstance())->getRakPeer()->SendLoopback((const char*) bitStream.GetData(), bitStream.GetNumberOfBytesUsed());*/
		}
		break;

		// Multiplayer menu
		case CGF_KEY_F9:
		{
			if(m_game->peekState()->getID() == GAME_STATE_MULTIPLAYER)
			{
				m_game->popState();
			}
			else
			{
				//m_game.pushState(new MultiplayerScene(m_world));
			}
		}
		break;

		// Attach/detach camera
		case CGF_KEY_F10:
		{
			Camera *camera = m_world->getCamera();
			if(camera->getTargetEntity())
			{
				// Detach camera
				camera->setTargetEntity(nullptr);
			}
			else
			{
				// Attach camera to local player
				camera->setTargetEntity(m_world->getLocalPlayer());
			}
		}
		break;

		case CGF_KEY_F11:
		{
			ChunkManager::ChunkArea area = m_world->getTerrain()->getChunkManager()->getActiveArea();
			for(int y = area.y0 * CHUNK_BLOCKS; y <= area.y1 * CHUNK_BLOCKS; ++y)
			{
				for(int x = area.x0 * CHUNK_BLOCKS; x <= area.x1 * CHUNK_BLOCKS; ++x)
				{
					BlockEntityData::get(BLOCK_ENTITY_TORCH)->create(m_world, x, y);
				}
			}
		}
		break;

		case CGF_KEY_F12:
		{
		}
		break;
	}
}

void Debug::onTick(TickEvent *e)
{
	if(!m_enabled) return;

	if(m_blockPainterEnabled)
	{
		// Block painting
		WorldLayer layer = WORLD_LAYER_MIDDLE;
		if(m_game->getInputManager()->getKeyState(CGF_KEY_LSHIFT)) layer = WORLD_LAYER_FRONT;
		if(m_game->getInputManager()->getKeyState(CGF_KEY_LCTRL)) layer = WORLD_LAYER_BACK;
		if(m_game->getInputManager()->getKeyState(CGF_MOUSE_BUTTON_LEFT) || m_game->getInputManager()->getKeyState(CGF_MOUSE_BUTTON_RIGHT))
		{
			m_world->getTerrain()->setBlockAt(
				(int) floor(m_world->getCamera()->getInputPosition().x / BLOCK_PXF),
				(int) floor(m_world->getCamera()->getInputPosition().y / BLOCK_PXF),
				layer,
				m_game->getInputManager()->getKeyState(CGF_MOUSE_BUTTON_LEFT) ? m_block : BLOCK_EMPTY,
				true);
		}
	}
}

void Debug::onDraw(DrawEvent *e)
{
	if(!m_enabled) return;

	GraphicsContext *context = e->getGraphicsContext();
	SpriteBatch *spriteBatch = (SpriteBatch*) e->getUserData();
	spriteBatch->end();
	spriteBatch->begin();

	// Set debug variables
	setVariable("Chunks", util::intToStr(m_world->getTerrain()->getChunkManager()->m_chunks.size()) + " / " + util::intToStr(m_world->getTerrain()->getChunkManager()->m_optimalChunkCount));
	Vector2I center = m_world->getCamera()->getCenter(e->getAlpha());
	Vector2F inputPosition = m_world->getCamera()->getInputPosition();
	setVariable("Camera", util::floatToStr(center.x) + ", " + util::floatToStr(center.y));
	setVariable("Zoom", util::intToStr(int(m_world->getCamera()->getZoomLevel() * 100)) + "%");
	setVariable("Block Under Cursor", util::intToStr(m_world->getTerrain()->getBlockAt((int) floor(inputPosition.x / BLOCK_PXF), (int) floor(inputPosition.y / BLOCK_PXF), WORLD_LAYER_MIDDLE)) + " at " + util::intToStr((int) floor(inputPosition.x / BLOCK_PXF)) + ", " + util::intToStr((int) floor(inputPosition.y / BLOCK_PXF)));
	string hourStr, minStr;
	{
		int hour = m_world->getTimeOfDay()->getHour();
		hourStr = hour < 10 ? ("0" + util::intToStr(hour)) : util::intToStr(hour);
		int min = m_world->getTimeOfDay()->getMinute();
		minStr = min < 10 ? ("0" + util::intToStr(min)) : util::intToStr(min);
	}
	setVariable("Time", hourStr + ":" + minStr);

	if(m_game->getWorld()->getLocalPlayer())
	{
		Player *player = m_game->getWorld()->getLocalPlayer();
		Controller *controller = player->getController();
		setVariable("Velocity", player->getVelocity().toString());
		setVariable("Move dir", util::intToStr(controller->getInputState(Controller::INPUT_MOVE_RIGHT) - controller->getInputState(Controller::INPUT_MOVE_LEFT)));
		setVariable("Running", util::intToStr(controller->getInputState(Controller::INPUT_RUN)));
	}

	// Draw top-left debug info
	string drawString;
	for(map<string, string>::iterator itr = m_variables.begin(); itr != m_variables.end(); ++itr)
	{
		drawString += itr->first + ": " + itr->second + "\n";
	}
	m_font->draw(spriteBatch, Vector2F(0.0f), drawString);

	m_font->draw(spriteBatch, Vector2F((float) context->getWidth(), 0.0f), DEBUG_FUNCTIONS_STRING, FONT_ALIGN_RIGHT);


	// Block painter
	if(m_blockPainterEnabled)
	{
		m_blockPainterTexture->updatePixmap(BlockData::get(m_block)->getPixmap());
		spriteBatch->drawText(Vector2F(5.0f, context->getHeight() - 48.0f), "Current block:   (" + util::intToStr(m_block) + ")\n" + "Current layer: " + (m_game->getInputManager()->getKeyState(CGF_KEY_LCTRL) ? "BACK" : (m_game->getInputManager()->getKeyState(CGF_KEY_LSHIFT) ? "FRONT" : "SCENE")), m_font.get());
		Sprite blockSprite(m_blockPainterTexture, RectF(m_font->getStringWidth("Current block:"), context->getHeight() - 60.0f, 32.0f, 32.0f), Vector2F(0.0f, 0.0f), 0.0f, TextureRegion(0.0f, 1.0f / 3.0f, 1.0f, 1.0f));
		spriteBatch->drawSprite(blockSprite);
	}

	// Draw block grid
	context->setTransformationMatrix(m_world->getCamera()->getTransformationMatrix(e->getAlpha()));
	context->setTexture(nullptr);
	Vector2F position = m_world->getCamera()->getPosition();
	Vector2F size = m_world->getCamera()->getSize();
	if(m_game->getInputManager()->getKeyState(CGF_KEY_K))
	{
		int x0 = (int) floor(position.x / BLOCK_PXF);
		int y0 = (int) floor(position.y / BLOCK_PXF);
		int x1 = (int) floor((position.x + size.x) / BLOCK_PXF);
		int y1 = (int) floor((position.y + size.y) / BLOCK_PXF);

		for(int y = y0; y <= y1; ++y)
		{
			context->drawRectangle(x0 * BLOCK_PXF, y * BLOCK_PXF, (x1 - x0 + 1) * BLOCK_PXF, 1.0f / m_world->getCamera()->getZoomLevel(), Color(80, 80, 80, 80));
		}

		for(int x = x0; x <= x1; ++x)
		{
			context->drawRectangle(x * BLOCK_PXF, y0 * BLOCK_PXF, 1.0f / m_world->getCamera()->getZoomLevel(), (y1 - y0 + 1) * BLOCK_PXF, Color(80, 80, 80, 80));
		}
	}

	// Draw chunk grid
	int x0 = (int) floor(position.x / CHUNK_PXF);
	int y0 = (int) floor(position.y / CHUNK_PXF);
	int x1 = (int) floor((position.x + size.x) / CHUNK_PXF);
	int y1 = (int) floor((position.y + size.y) / CHUNK_PXF);

	for(int y = y0; y <= y1; ++y)
	{
		context->drawRectangle(x0 * CHUNK_PXF, y * CHUNK_PXF, (x1 - x0 + 1) * CHUNK_PXF, 1.0f / m_world->getCamera()->getZoomLevel(), Color(0, 0, 0, 80));
	}

	for(int x = x0; x <= x1; ++x)
	{
		context->drawRectangle(x * CHUNK_PXF, y0 * CHUNK_PXF, 1.0f / m_world->getCamera()->getZoomLevel(), (y1 - y0 + 1) * CHUNK_PXF, Color(0, 0, 0, 80));
	}

	// Draw debug chunks
	if(m_debugChunkLoader)
	{
		// Draw current view
		Vector2F position = m_world->getCamera()->getCenter(e->getAlpha()) - context->getSize() * 0.5f;
		Vector2F size = context->getSize();
		context->drawRectangle(position.x, position.y, 1.0f / m_world->getCamera()->getZoomLevel(), size.y, Color(127, 127, 255, 255));
		context->drawRectangle((position.x + size.x), position.y, 1.0f / m_world->getCamera()->getZoomLevel(), size.y, Color(127, 127, 255, 255));
		context->drawRectangle(position.x, position.y, size.x, 1.0f / m_world->getCamera()->getZoomLevel(), Color(127, 127, 255, 255));
		context->drawRectangle(position.x, (position.y + size.y), size.x, 1.0f / m_world->getCamera()->getZoomLevel(), Color(127, 127, 255, 255));

		// Draw load area
		ChunkManager::ChunkArea loadArea = m_world->getTerrain()->getChunkManager()->getLoadingArea();
		context->drawRectangle(loadArea.x0 * CHUNK_PXF, loadArea.y0 * CHUNK_PXF, 1.0f / m_world->getCamera()->getZoomLevel(), (loadArea.y1 - loadArea.y0 + 1) * CHUNK_PXF, Color(255, 255, 255, 255));
		context->drawRectangle((loadArea.x1 + 1) * CHUNK_PXF, loadArea.y0 * CHUNK_PXF, 1.0f / m_world->getCamera()->getZoomLevel(), (loadArea.y1 - loadArea.y0 + 1) * CHUNK_PXF, Color(255, 255, 255, 255));
		context->drawRectangle(loadArea.x0 * CHUNK_PXF, loadArea.y0 * CHUNK_PXF, (loadArea.x1 - loadArea.x0 + 1) * CHUNK_PXF, 1.0f / m_world->getCamera()->getZoomLevel(), Color(255, 255, 255, 255));
		context->drawRectangle(loadArea.x0 * CHUNK_PXF, (loadArea.y1 + 1) * CHUNK_PXF, (loadArea.x1 - loadArea.x0 + 1) * CHUNK_PXF, 1.0f / m_world->getCamera()->getZoomLevel(), Color(255, 255, 255, 255));

		// Draw chunk status
		for(int y = y0; y <= y1; ++y)
		{
			for(int x = x0; x <= x1; ++x)
			{
				// DARK BLUE: No blocks loaded
				// LIGHT BLUE: Blocks loaded in memory, but not rendered to the global render target
				// RED: Blocks rendered to the global render target, but not sorted
				// BLANK: Blocks rendered and sorted.
				if(!m_world->getTerrain()->getChunkManager()->isChunkLoadedAt(x, y))
				{
					context->drawRectangle(x * CHUNK_PXF, y * CHUNK_PXF, CHUNK_PXF, CHUNK_PXF, Color(0, 100, 170, 127));
				}
				else if(!m_world->getTerrain()->getChunkManager()->getChunkAt(x, y).isAttached())
				{
					context->drawRectangle(x * CHUNK_PXF, y * CHUNK_PXF, CHUNK_PXF, CHUNK_PXF, Color(0, 160, 230, 127));
				}
				else if(!m_world->getTerrain()->getChunkManager()->getChunkAt(x, y).isSorted())
				{
					context->drawRectangle(x * CHUNK_PXF, y * CHUNK_PXF, CHUNK_PXF, CHUNK_PXF, Color(230, 30, 30, 127));
				}
			}
		}
	}

	// Debug lighting
	if(m_debugLighting)
	{
		// Show light sources
		for(list<LightSource*>::iterator itr = m_world->getLighting()->m_lightSources.begin(); itr != m_world->getLighting()->m_lightSources.end(); ++itr)
		{
			context->drawCircle((*itr)->getPosition() * BLOCK_PXF, BLOCK_PX / 2 - 1, 12, math::lerp(Color(243, 230, 188, 255), (*itr)->getColor(), 0.75f));
		}

		// Show lighting passes
		context->setTransformationMatrix(Matrix4());
		context->disable(GraphicsContext::BLEND);
		//context->setTexture(m_world->getTerrain()->getChunkManager()->m_lightRenderTarget->getTexture()); context->drawRectangle(0.0f, 128.0f * 1, 256.0f, 128.0f);
		context->setTexture(m_world->getTerrain()->getChunkManager()->m_lightingPass0->getTexture()); context->drawRectangle(0.0f, 128.0f * 2, 256.0f, 128.0f);
		context->setTexture(m_world->getTerrain()->getChunkManager()->m_lightingPass1->getTexture()); context->drawRectangle(0.0f, 128.0f * 3, 256.0f, 128.0f);
		context->setTexture(m_world->getTerrain()->getChunkManager()->m_lightingPass2->getTexture()); context->drawRectangle(0.0f, 128.0f * 4, 256.0f, 128.0f);
		context->enable(GraphicsContext::BLEND);
	}
}

void Debug::toggle()
{
	m_enabled = !m_enabled;
}

void Debug::nextBlock(KeyEvent *e)
{
	if(m_enabled && e->getType() == KeyEvent::DOWN)
	{
		if((m_block = BlockID(m_block + 1)) >= BLOCK_COUNT)
		{
			m_block = BlockID(BLOCK_EMPTY + 1);
		}
	}
}

void Debug::prevBlock(KeyEvent *e)
{
	if(m_enabled && e->getType() == KeyEvent::DOWN)
	{
		if((m_block = BlockID(m_block - 1)) <= BLOCK_EMPTY)
		{
			m_block = BlockID(BLOCK_COUNT - 1);
		}
	}
}

void Debug::setVariable(const string &name, const string &value)
{
	m_variables[name] = value;
}