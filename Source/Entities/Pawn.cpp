#include "BitStream.h"

#include "Pawn.h"
#include "World/World.h"
#include "World/Camera.h"
#include "World/Terrain/Terrain.h"
#include "Constants.h"
#include "Entities/Physics/DynamicEntity.h"
#include "Items/ItemData.h"
#include "Networking/Connection.h"
#include "Networking/Server.h"
#include "Animation/Animation.h"
#include "Animation/Skeleton.h"
#include "Animation/Bone.h"
#include "Gui/GameOverlay.h"
#include "Things/Thing.h"
#include "Game/Game.h"
#include "Game/GameStates/GameState.h"
#include "Game/Scene.h"

#include "Gui/Canvas.h"

Pawn::Pawn(Game *game, const EntityID id) :
	DynamicEntity(game->getWorld(), id),
	m_camera(game->getWorld()->getCamera()),
	m_terrain(game->getWorld()->getTerrain()),
	m_gameOverlay(game->getGameOverlay()),
	m_jumpTimer(1.0f),
	m_canJump(false),
	m_maxHealth(12),
	m_health(m_maxHealth),
	m_lmbPressed(false),
	m_storage(10),
	m_bag(new Bag(20, 5)),
	m_moveSpeed(5.0f)
{
	// Set body size
	Entity::setSize(24, 48);

	// Init inventory things
	m_storage.setNext(m_bag->getStorage());

	// Add to player list
	m_world->m_pawns.push_back(this);
}

Pawn::~Pawn()
{
}

void Pawn::setController(Controller * controller)
{
	m_controller = controller;
	m_controller->setPawn(this);
}

Storage::Slot *Pawn::getCurrentItem()
{
	if(!m_heldItem.isEmpty())
	{
		return &m_heldItem;
	}
	return m_storage.getSlotAt(m_gameOverlay->getHotbar()->getSelectedSlot());
}

void Pawn::activateThing(int action)
{
	if(action != GLFW_PRESS) return;
	Vector2 inputPosition = m_camera->getInputPosition();
	int blockX = (int)floor(inputPosition.x / BLOCK_PXF), blockY = (int)floor(inputPosition.y / BLOCK_PXF);
	for(Thing *thing : m_terrain->getChunkLoader()->getChunkAt((int)floor(inputPosition.x / CHUNK_PXF), (int)floor(inputPosition.y / CHUNK_PXF)).getThings())
	{
		if(thing->getX() == blockX && thing->getY() == blockY)
		{
			thing->activate(this);
		}
	}
}

void Pawn::update(const float delta)
{
	m_controller->update();

	// Jumping
	if(isContact(SOUTH))
	{
		if(m_controller->getInputState(Controller::INPUT_JUMP))
		{
			if(m_canJump)
			{
				applyImpulse(Vector2(0.0f, -12.0f));
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
			if(m_controller->getInputState(Controller::INPUT_JUMP)) // High/low jumping
			{
				applyImpulse(Vector2(0.0f, -2.5f));
			}
			m_jumpTimer += delta;
		}
		else if(isContact(WEST) || isContact(EAST)) // Wall jumping
		{
			setVelocityY(getVelocity().y * 0.5f);
			if(m_controller->getInputState(Controller::INPUT_JUMP))
			{
				if(m_canJump)
				{
					setVelocityX((isContact(WEST) - isContact(EAST)) * 14.0f);
					setVelocityY(-4.5f);
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
	applyImpulse(Vector2((m_controller->getInputState(Controller::INPUT_MOVE_RIGHT) - m_controller->getInputState(Controller::INPUT_MOVE_LEFT)) * (m_controller->getInputState(Controller::INPUT_RUN) ? 1.5f : 1.0f) * 10.0f, 0.0f));
	if(getVelocity().x < -5.0f)
	{
		setVelocityX(-m_moveSpeed);
		setAccelerationX(0.0f);
	}
	else if(getVelocity().x > 5.0f)
	{
		setVelocityX(m_moveSpeed);
		setAccelerationX(0.0f);
	}
	else
	{
		// Apply friction
		setVelocityX(getVelocity().x * 0.85f);
	}

	// Update physics
	DynamicEntity::update(delta);

	// Use current item
	if(m_controller->getInputState(Controller::INPUT_USE_ITEM))
	{
		Storage::Slot *slot = getCurrentItem();
		ItemData *item = ItemData::get(slot->getItem());
		if(item != nullptr && (!item->isSingleShot() || !m_lmbPressed))
		{
			item->use(this, delta);
		}
		m_lmbPressed = true;
	}
	else
	{
		m_humanoid.setPostAnimation(Humanoid::ANIM_NULL);
		m_lmbPressed = false;
	}
	
	// Set animations
	m_humanoid.getMainAnimationState()->setTimeScale(math::abs(getVelocity().x) * 4.0f * delta);
	if(isContact(SOUTH))
	{
		m_humanoid.getMainAnimationState()->setLooping(true);
		if(getVelocity().x >= 0.1f)
		{
			m_humanoid.setMainAnimation(Humanoid::ANIM_WALK);
			m_humanoid.getSkeleton()->setFlipX(false);
		}
		else if(getVelocity().x <= -0.1f)
		{
			m_humanoid.setMainAnimation(Humanoid::ANIM_WALK);
			m_humanoid.getSkeleton()->setFlipX(true);
		}
		else
		{
			m_humanoid.setMainAnimation(Humanoid::ANIM_IDLE);
			setVelocityX(0.0f);
			m_humanoid.getMainAnimationState()->setTimeScale(1.0f);
		}
	}
	else
	{
		/*if(isContact(WEST) >= 3) // TODO: I should check for a column of 3 rows of blocks instead of simlply one
		{
			m_humanoid.getSkeleton()->setFlipX(false);
			m_humanoid.getMainAnimationState()->setLooping(false);
			m_humanoid.getMainAnimationState()->setTimeScale(5.0f);
			m_humanoid.setMainAnimation(Humanoid::ANIM_WALL_SLIDE);
		}
		else if(isContact(EAST))
		{
			m_humanoid.getSkeleton()->setFlipX(true);
			m_humanoid.getMainAnimationState()->setLooping(false);
			m_humanoid.getMainAnimationState()->setTimeScale(5.0f);
			m_humanoid.setMainAnimation(Humanoid::ANIM_WALL_SLIDE);
		}
		else*/
		{
			m_humanoid.getMainAnimationState()->setLooping(false);
			m_humanoid.getMainAnimationState()->setTimeScale(1.0f);
			m_humanoid.setMainAnimation(Humanoid::ANIM_JUMP);
		}
	}

	// Update animations
	m_humanoid.update(delta);
}

void Pawn::draw(SpriteBatch *spriteBatch, const float alpha)
{
	m_humanoid.draw(this, spriteBatch, alpha);
	
	ItemData *item = ItemData::get(getCurrentItem()->getItem());
	if(item != nullptr)
	{
		item->draw(this, spriteBatch, alpha);
	}
}

void Pawn::pack(RakNet::BitStream *bitStream, const Connection *conn)
{
	if(conn->isServer())
	{
		bitStream->Write(getPosition().x);
		bitStream->Write(getPosition().y);
		bitStream->Write(getVelocity().x);
		bitStream->Write(getVelocity().y);
	}
}

void Pawn::unpack(RakNet::BitStream *bitStream, const Connection *conn)
{
	if(conn->isClient())
	{
		float x; bitStream->Read(x);
		float y; bitStream->Read(y);
		Entity::setPosition(x, y);
		bitStream->Read(x);
		bitStream->Read(y);
		setVelocityX(x);
		setVelocityY(y);
	}
}

void Pawn::createSaveData(FileWriter &saveData)
{
	// Save position
	saveData << (int)getPosition().x << endl;
	saveData << (int)getPosition().y << endl;

	// Save hotbar
	for(uint i = 0; i < 10; ++i)
	{
		saveData << m_storage.getSlotAt(i)->getItem() << endl;
		saveData << m_storage.getSlotAt(i)->getAmount() << endl;
	}

	// Save bag and its content
	saveData << (int)m_bag->getWidth() << endl;
	saveData << (int)m_bag->getHeight() << endl;
	for(uint y = 0; y < m_bag->getHeight(); ++y)
	{
		for(uint x = 0; x < m_bag->getWidth(); ++x)
		{
			saveData << m_bag->getStorage()->getSlotAt(x + y * m_bag->getWidth())->getItem() << endl;
			saveData << m_bag->getStorage()->getSlotAt(x + y * m_bag->getWidth())->getAmount() << endl;
		}
	}
}

void Pawn::loadSaveData(FileReader &saveData)
{
	// Restore position
	int x, y;
	saveData >> x;
	saveData >> y;
	setPosition(Vector2((float) x, (float) y));

	// Restore hotbar
	for(uint i = 0; i < 10; ++i)
	{
		int item, amount;
		saveData >> item;
		saveData >> amount;
		m_storage.getSlotAt(i)->set((ItemID)item, amount);
	}
	
	// Restore bag and its content
	int bagWidth, bagHeight;
	saveData >> bagWidth;
	saveData >> bagHeight;

	delete m_bag;
	m_bag = new Bag(bagWidth, bagHeight);
	for(int y = 0; y < bagHeight; ++y)
	{
		for(int x = 0; x < bagWidth; ++x)
		{
			int item, amount;
			saveData >> item;
			saveData >> amount;
			m_bag->getStorage()->getSlotAt(x + y * bagWidth)->set((ItemID)item, amount);
		}
	}
}

void Pawn::decHealth(int amt)
{
	m_health -= amt;
	if(m_health <= 0)
	{
		LOG("Dead");
	}
}