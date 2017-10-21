#include "ItemDrop.h"
#include "Constants.h"
#include "Player.h"
#include "World/World.h"
#include "Items/ItemData.h"
#include "Entities/EntityData.h"

ItemDrop::ItemDrop(World *world, const Json::Value &attributes) :
	Entity(world, attributes),
	m_noPickupTime(1.0f), // No pickup for 1 s
	m_dragDistance(16.0f * BLOCK_PXF),
	m_pickupDistance(16.0f),
	m_hoverTime(0.0f),
	m_prevHoverTime(0.0f)
{
	m_itemID = attributes["item"].get("id", ITEM_NONE).asInt();
	m_amount = attributes["item"].get("amount", 1).asInt();

	setGravityScale(0.1f);
	setSize(Vector2F(16.0f));
}

ItemDrop::~ItemDrop()
{
}

void ItemDrop::onTick(TickEvent *e)
{
	if(m_noPickupTime <= 0.0f)
	{
		list<Pawn*> pawns = m_world->getPawns();
		for(Pawn *pawn : pawns)
		{
			Vector2F deltaPosition = pawn->getCenter() - getCenter();
			if(pawn->getRect().contains(getCenter()))
			{
				m_amount -= m_amount - pawn->getStorage()->addItem(m_itemID, m_amount);
				if(m_amount <= 0)
				{
					m_connection->removeEntity(this);
					return;
				}
			}
			else if(deltaPosition.length() <= m_dragDistance)
			{
				applyImpulse(deltaPosition.normalized());
			}
		}
	}
	m_noPickupTime -= e->getDelta();

	m_prevHoverTime = m_hoverTime;
	m_hoverTime += 5.0f * e->getDelta();

	Entity::onTick(e);
}

void ItemDrop::onDraw(DrawEvent *e)
{
	SpriteBatch *spriteBatch = (SpriteBatch*) e->getUserData();
	spriteBatch->drawSprite(Sprite(ItemData::Get(m_itemID)->getIconTexture(), RectF(
		getDrawPosition(e->getAlpha()) - Vector2F(0.0f, ((sin(math::lerp(m_prevHoverTime, m_hoverTime, e->getAlpha())) * 0.5f + 0.5f) * 8.0f)) - Vector2F(4.0f, 4.0f),
		Vector2F(24.0f)),
		Vector2F(), 0.0f, TextureRegion())
		);
}

void ItemDrop::packData(RakNet::BitStream *bitStream)
{
	bitStream->Write(getPosition().x);
	bitStream->Write(getPosition().y);
	bitStream->Write(getVelocity().x);
	bitStream->Write(getVelocity().y);
	bitStream->Write(m_itemID);
	bitStream->Write(m_amount);
}

extern Timer t;

bool ItemDrop::unpackData(RakNet::BitStream *bitStream, const bool force)
{
	double time = t.getElapsedTime();
	t.stop();

	// Get position and velocity
	Vector2F position, velocity;
	bitStream->Read(position.x);
	bitStream->Read(position.y);
	bitStream->Read(velocity.x);
	bitStream->Read(velocity.y);
	bitStream->Read(m_itemID);
	bitStream->Read(m_amount);

	// TODO: Verify using some time detla between this and previous packet
	float radius = getVelocity().length() * time * 100.0 + 20.0 /*gravity*/;
	float moved = (getPosition() - position).length();
	if(!force && radius < moved)
	{
		// Invalid position, lets not accept the values we we're sent
		// Send back a packet containing the server-side object state
		// to all clients
		return false;
	}
	else
	{
		// Lets move the player to their new (verified) position
		setPosition(getLastPosition()); // Doing this ensures that players are interpolated correctly from their last position to their new position
		moveTo(position); // TODO: Not really a pretty solution
		setVelocity(velocity);

	}
	t.start();
	return true;
}