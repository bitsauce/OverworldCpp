#include "InGameState.h"
#include "Game/Game.h"

InGameState::InGameState(OverworldGame *game) :
	GameState(GAME_STATE_IN_GAME, false),
	m_world(game->getWorld())
{
	addChildLast(m_world);
}

void InGameState::onEnter()
{
	/*RakNet::BitStream bitStream;
	bitStream.Write((RakNet::MessageID)ID_CREATE_ENTITY);
	//bitStream.Write(ENTITY_PLAYER);
	if(Connection::getInstance()->isServer())
	{
		((Server*)Connection::getInstance())->getRakPeer()->SendLoopback((const char*)bitStream.GetData(), bitStream.GetNumberOfBytesUsed());
	}
	else
	{
		((Client*)Connection::getInstance())->sendPacket(&bitStream);
	}*/

	//Input::setContext(Input::getContext("game"));
}

void InGameState::onTick(TickEvent *e)
{
	//m_world->update(delta);
	//m_scene.update(delta);
	GameState::onTick(e);
}

void InGameState::onDraw(DrawEvent *e)
{
	//m_world->draw(spriteBatch, alpha);
	//m_scene.draw(spriteBatch, alpha);
	GameState::onDraw(e);
}