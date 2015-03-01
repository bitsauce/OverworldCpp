#include "BitStream.h"

#include "Player.h"
#include "World/World.h"
#include "Entities/Camera.h"
#include "Terrain/Terrain.h"
#include "Constants.h"
#include "Physics/PhysicsBody.h"
#include "Animation/Animation.h"
#include "Animation/Skeleton.h"
#include "Animation/Bone.h"
#include "Items/ItemData.h"
#include "Networking/Connection.h"

#include "Gui/GameOverlay.h"

Player::Player(RakNet::RakNetGUID guid) :
	GameObject(DRAW_ORDER_PLAYER),
	m_camera(World::getCamera()),
	m_terrain(World::getTerrain()),
	m_jumpTimer(1.0f),
	m_canJump(false),
	m_currentAnim(nullptr),
	m_selectedItemSlot(0),
	m_itemContainer(10),
	m_guid(guid),
	m_maxHealth(12),
	m_health(m_maxHealth),
	m_lmbPressed(false)
{
	// Load physics
	m_body = new PhysicsBody();
	m_body->setSize(24, 48);

	// Load skeleton data
	m_skeleton = new Skeleton(":/sprites/characters/anim/skeleton.json", ":/sprites/characters/anim/skeleton.atlas", 1.0f);
	m_skeleton->getTexture()->setFiltering(Texture2D::LINEAR);
	m_skeleton->setFlipY(true); // Not sure why this is needed, oh well

	// Setup spine animations
	m_animationStateData = new AnimationStateData(m_skeleton);
	m_animationStateData->setMix("idle", "walk", 0.2f);
	m_animationStateData->setMix("walk", "idle", 0.5f);
	m_animationStateData->setMix("jump", "idle", 0.1f);
	m_animationStateData->setMix("walk", "jump", 0.1f);
	m_animationStateData->setMix("jump", "idle", 0.1f);
	m_animationStateData->setMix("idle", "jump", 0.2f);
	
	// Create spine animation states
	m_mainAnimationState = new AnimationState(m_animationStateData);
	//m_mainAnimationState->setEventCallback(animationEvent);
	m_mainAnimationState->setLooping(true);
	
	m_itemAnimationState = new AnimationState(m_animationStateData);
	m_itemAnimationState->setLooping(true);

	setMainAnimation("idle");
	
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

	// Test code for replacing body parts/clothing
	/*TextureRegion region = m_skeleton->getTextureRegion("head");
	Texture2DPtr texture = m_skeleton->getTexture();
	uint x0 = region.uv0.x * texture->getWidth(), y0 = region.uv0.y * texture->getHeight(),
		x1 = region.uv1.x * texture->getWidth(), y1 = region.uv1.y * texture->getHeight();
	Pixmap pixmap = ResourceManager::get<Texture2D>(":/Sprites/Characters/head2.png")->getPixmap();
	m_skeleton->getTexture()->updatePixmap(x0, y0, pixmap);*/

	World::s_players.push_back(this);
}

// ANIMATIONS
void Player::setMainAnimation(const string &name)
{
	// Get animation by name
	Animation *anim = m_skeleton->findAnimation(name);
	if(anim == nullptr)
	{
		LOG("Humanoid::setMainAnimation() - Animation '%s' does not exists.", name);
		return;
	}
		
	// Make sure this animation isn't current
	if(m_currentAnim != anim)
	{
		m_mainAnimationState->setAnimation(anim);
		m_currentAnim = anim;
	}
}

void Player::setItemAnimation(Animation *anim)
{
	// Make sure this animation isn't current
	if(m_itemAnimation != anim)
	{
		if(anim != nullptr)
		{
			m_itemAnimationState->setAnimation(anim);
		}
		m_itemAnimation = anim;
	}
}

#include "Networking/Server.h"

void Player::update()
{
	//if(Server::getInstance())
	{
		// Jumping
		if(m_body->isContact(SOUTH))
		{
			if(m_inputState[INPUT_JUMP])
			{
				if(m_canJump)
				{
					m_body->applyImpulse(Vector2(0.0f, -4.5f));
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
					m_body->applyImpulse(Vector2(0.0f, -0.75f));
				}
				m_jumpTimer += Graphics::getTimeStep();
			}
			else if(m_body->isContact(WEST) || m_body->isContact(EAST)) // Wall jumping
			{
				m_body->setVelocityY(m_body->getVelocity().y*0.5f);
				if(m_inputState[INPUT_JUMP])
				{
					if(m_canJump)
					{
						m_body->setVelocityX((m_body->isContact(WEST) - m_body->isContact(EAST)) * 14.0f);
						m_body->setVelocityY(-4.5f);
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
		if(abs(m_body->getVelocity().x) < 10.0f)
		{
			m_body->applyImpulse(Vector2((m_inputState[INPUT_MOVE_RIGHT] - m_inputState[INPUT_MOVE_LEFT]) * (Input::getKeyState(XD_KEY_SHIFT) ? 1.0f : 0.5f), 0.0f));
		}

		// Apply friction
		m_body->setVelocityX(m_body->getVelocity().x * 0.85f);
	
		// Set animations
		m_mainAnimationState->setTimeScale(math::abs(m_body->getVelocity().x) * 0.5f);
		if(m_body->isContact(SOUTH))
		{
			m_mainAnimationState->setLooping(true);
			if(m_body->getVelocity().x >= 0.01f)
			{
				setMainAnimation("walk");
				m_skeleton->setFlipX(false);
			}
			else if(m_body->getVelocity().x <= -0.01f)
			{
				setMainAnimation("walk");
				m_skeleton->setFlipX(true);
			}
			else
			{
				setMainAnimation("idle");
				m_body->setVelocityX(0.0f);
				m_mainAnimationState->setTimeScale(1.0f);
			}
		}
		else
		{
			if(m_body->isContact(WEST)/* >= 3*/) // TODO: I should check for a column of 3 rows of blocks instead of simlply one
			{
				m_skeleton->setFlipX(false);
				m_mainAnimationState->setLooping(false);
				m_mainAnimationState->setTimeScale(5.0f);
				setMainAnimation("wall-slide");
			}
			else if(m_body->isContact(EAST))
			{
				m_skeleton->setFlipX(true);
				m_mainAnimationState->setLooping(false);
				m_mainAnimationState->setTimeScale(5.0f);
				setMainAnimation("wall-slide");
			}
			else
			{
				m_mainAnimationState->setLooping(false);
				m_mainAnimationState->setTimeScale(1.0f);
				setMainAnimation("jump");
			}
		}

		// Update physics
		m_body->update();

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
			m_itemAnimation = nullptr;
			m_lmbPressed = false;
		}

		// Move camera
		if(Connection::getInstance()->getGUID() == m_guid)
		m_camera->lookAt(m_body->getPosition());

		// Update animations
		m_skeleton->setPosition(m_body->getPosition() + Vector2(m_body->getSize().x*0.5f, 48.0f));
		m_mainAnimationState->update(Graphics::getTimeStep());
		if(m_itemAnimation) m_itemAnimationState->update(Graphics::getTimeStep());
	}
}

void Player::pack(RakNet::BitStream *bitStream, const Connection *conn)
{
	if(conn->isServer())
	{
		bitStream->Write(m_body->getPosition().x);
		bitStream->Write(m_body->getPosition().y);
		bitStream->Write(m_body->getVelocity().x);
		bitStream->Write(m_body->getVelocity().y);
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
		m_body->setPosition(x, y);
		bitStream->Read(x);
		bitStream->Read(y);
		m_body->setVelocityX(x);
		m_body->setVelocityY(y);
	}
}

void Player::draw(SpriteBatch *spriteBatch)
{
	m_skeleton->draw(spriteBatch->getGraphicsContext());
	
	ItemData *item = ItemData::get(m_itemContainer.getItemAt(m_selectedItemSlot));
	if(item != nullptr)
	{
		item->draw(this, spriteBatch);
	}
}

void Player::drawSpriteInHand(Sprite &sprite, const Vector2 &origin, SpriteBatch *spriteBatch)
{
	float angle = m_skeleton->findBone("rarm")->getWorldRotation();
	Vector2 size = sprite.getSize();
	if(m_skeleton->getFlipX())
	{
		sprite.setSize(sprite.getSize() * Vector2(-1.0f, 1.0f));
	}
	else
	{
		angle *= -1;
	}
	sprite.setPosition(m_skeleton->getPosition() + m_skeleton->findBone("rarm")->getWorldPosition() - origin + Vector2(cos(angle * 0.0174532925f), sin(angle * 0.0174532925f)) * 10);
	sprite.setOrigin(origin);
	sprite.setRotation(angle);
	spriteBatch->drawSprite(sprite);
	sprite.setSize(size);
}