#include "BitStream.h"

#include "Player.h"
#include "World/World.h"
#include "Entities/Camera.h"
#include "Terrain/Terrain.h"
#include "Constants.h"
#include "Physics/PhysicsBody.h"
#include "Items/ItemData.h"
#include "Networking/Connection.h"
#include "Networking/Server.h"
#include "Animation/Animation.h"
#include "Animation/Skeleton.h"
#include "Animation/Bone.h"
#include "Gui/GameOverlay.h"

Player::Player(RakNet::RakNetGUID guid) :
	Entity(DRAW_ORDER_PLAYER),
	m_camera(World::getCamera()),
	m_terrain(World::getTerrain()),
	m_jumpTimer(1.0f),
	m_canJump(false),
	m_selectedItemSlot(0),
	m_itemContainer(10),
	m_guid(guid),
	m_maxHealth(12),
	m_health(m_maxHealth),
	m_lmbPressed(false)
{
	// Set body size
	m_body.setSize(24, 48);
	
	// If player is local, do extra stuff
	if(Connection::getInstance()->getGUID() == guid)
	{
		new GameOverlay(this, canvas);

		// Bind keys to item slots
		Input::bind(XD_KEY_1, bind(&Player::setSelectedItemSlot, this, 0));
		Input::bind(XD_KEY_2, bind(&Player::setSelectedItemSlot, this, 1));
		Input::bind(XD_KEY_3, bind(&Player::setSelectedItemSlot, this, 2));
		Input::bind(XD_KEY_4, bind(&Player::setSelectedItemSlot, this, 3));
		Input::bind(XD_KEY_5, bind(&Player::setSelectedItemSlot, this, 4));
		Input::bind(XD_KEY_6, bind(&Player::setSelectedItemSlot, this, 5));
		Input::bind(XD_KEY_7, bind(&Player::setSelectedItemSlot, this, 6));
		Input::bind(XD_KEY_8, bind(&Player::setSelectedItemSlot, this, 7));
		Input::bind(XD_KEY_9, bind(&Player::setSelectedItemSlot, this, 8));
		Input::bind(XD_KEY_0, bind(&Player::setSelectedItemSlot, this, 9));
	}

	// Add to player list
	World::s_players.push_back(this);
}

void Player::update()
{
	// Jumping
	if(m_body.isContact(SOUTH))
	{
		if(m_inputState[INPUT_JUMP])
		{
			if(m_canJump)
			{
				m_body.applyImpulse(Vector2(0.0f, -4.5f));
				m_jumpTimer = 0.0f;
				m_canJump = false;
			}
		}
		else
		{
			m_canJump = true;
		}
	}
	else
	{
		if(m_jumpTimer < 0.1f)
		{
			if(m_inputState[INPUT_JUMP]) // High/low jumping
			{
				m_body.applyImpulse(Vector2(0.0f, -0.75f));
			}
			m_jumpTimer += Graphics::getTimeStep();
		}
		else if(m_body.isContact(WEST) || m_body.isContact(EAST)) // Wall jumping
		{
			m_body.setVelocityY(m_body.getVelocity().y*0.5f);
			if(m_inputState[INPUT_JUMP])
			{
				if(m_canJump)
				{
					m_body.setVelocityX((m_body.isContact(WEST) - m_body.isContact(EAST)) * 14.0f);
					m_body.setVelocityY(-4.5f);
					m_jumpTimer = 0.0f;
					m_canJump = false;
				}
			}
			else
			{
				m_canJump = true;
			}
		}
	}

	// Walking
	if(abs(m_body.getVelocity().x) < 10.0f)
	{
		m_body.applyImpulse(Vector2((m_inputState[INPUT_MOVE_RIGHT] - m_inputState[INPUT_MOVE_LEFT]) * (Input::getKeyState(XD_KEY_SHIFT) ? 1.0f : 0.5f), 0.0f));
	}

	// Apply friction
	m_body.setVelocityX(m_body.getVelocity().x * 0.85f);

	// Update physics
	m_body.update();

	// Use current item
	if(Input::getKeyState(XD_LMB))
	{
		ItemData *item = ItemData::get(m_itemContainer.getItemAt(m_selectedItemSlot));
		if(item != nullptr && (!item->isSingleShot() || !m_lmbPressed))
		{
			item->use(this);
		}
		m_lmbPressed = true;
	}
	else
	{
		m_humanoid.setPostAnimation(Humanoid::ANIM_NULL);
		m_lmbPressed = false;
	}
	
	// Set animations
	m_humanoid.getMainAnimationState()->setTimeScale(math::abs(m_body.getVelocity().x) * 0.5f);
	if(m_body.isContact(SOUTH))
	{
		m_humanoid.getMainAnimationState()->setLooping(true);
		if(m_body.getVelocity().x >= 0.01f)
		{
			m_humanoid.setMainAnimation(Humanoid::ANIM_WALK);
			m_humanoid.getSkeleton()->setFlipX(false);
		}
		else if(m_body.getVelocity().x <= -0.01f)
		{
			m_humanoid.setMainAnimation(Humanoid::ANIM_WALK);
			m_humanoid.getSkeleton()->setFlipX(true);
		}
		else
		{
			m_humanoid.setMainAnimation(Humanoid::ANIM_IDLE);
			m_body.setVelocityX(0.0f);
			m_humanoid.getMainAnimationState()->setTimeScale(1.0f);
		}
	}
	else
	{
		if(m_body.isContact(WEST)/* >= 3*/) // TODO: I should check for a column of 3 rows of blocks instead of simlply one
		{
			m_humanoid.getSkeleton()->setFlipX(false);
			m_humanoid.getMainAnimationState()->setLooping(false);
			m_humanoid.getMainAnimationState()->setTimeScale(5.0f);
			m_humanoid.setMainAnimation(Humanoid::ANIM_WALL_SLIDE);
		}
		else if(m_body.isContact(EAST))
		{
			m_humanoid.getSkeleton()->setFlipX(true);
			m_humanoid.getMainAnimationState()->setLooping(false);
			m_humanoid.getMainAnimationState()->setTimeScale(5.0f);
			m_humanoid.setMainAnimation(Humanoid::ANIM_WALL_SLIDE);
		}
		else
		{
			m_humanoid.getMainAnimationState()->setLooping(false);
			m_humanoid.getMainAnimationState()->setTimeScale(1.0f);
			m_humanoid.setMainAnimation(Humanoid::ANIM_JUMP);
		}
	}

	// Update animations
	m_humanoid.update();

	// Move camera
	if(Connection::getInstance()->getGUID() == m_guid && !Input::getKeyState(XD_KEY_L))
	{
		m_camera->lookAt(m_body.getPosition());
	}
}

void Player::pack(RakNet::BitStream *bitStream, const Connection *conn)
{
	if(conn->isServer())
	{
		bitStream->Write(m_body.getPosition().x);
		bitStream->Write(m_body.getPosition().y);
		bitStream->Write(m_body.getVelocity().x);
		bitStream->Write(m_body.getVelocity().y);
	}
	else if(conn->getGUID() == m_guid)
	{
		bitStream->Write(Input::getKeyState(XD_KEY_A));
		bitStream->Write(Input::getKeyState(XD_KEY_D));
		bitStream->Write(Input::getKeyState(XD_KEY_SPACE));
	}
}

void Player::unpack(RakNet::BitStream *bitStream, const Connection *conn)
{
	if(conn->isServer())
	{
		bitStream->Read(m_inputState[INPUT_MOVE_LEFT]);
		bitStream->Read(m_inputState[INPUT_MOVE_RIGHT]);
		bitStream->Read(m_inputState[INPUT_JUMP]);
	}
	else
	{
		float x; bitStream->Read(x);
		float y; bitStream->Read(y);
		m_body.setPosition(x, y);
		bitStream->Read(x);
		bitStream->Read(y);
		m_body.setVelocityX(x);
		m_body.setVelocityY(y);
	}
}

void Player::draw(SpriteBatch *spriteBatch)
{
	m_humanoid.draw(&m_body, spriteBatch);
	ItemData *item = ItemData::get(m_itemContainer.getItemAt(m_selectedItemSlot));
	if(item != nullptr)
	{
		item->draw(this, spriteBatch);
	}
}