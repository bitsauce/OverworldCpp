#ifndef ITEM_DROP_H
#define ITEM_DROP_H

#include "Config.h"
#include "Entities/Entity.h"
#include "Physics/PhysicsBody.h"

enum ItemID;

class ItemDrop : public Entity
{
public:
	ItemDrop(World &world, const Vector2 position, const ItemID item);

	Vector2 getPosition() const { return m_body.getPosition(); }
	Vector2 getSize() const { return m_body.getSize(); }

	void update(const float dt);
	void draw(SpriteBatch *spriteBatch, const float alpha);

private:
	const ItemID m_itemID;
	PhysicsBody m_body;
	Vector2 m_prevPosition;
	float m_dragDistance;
	float m_pickupDistance;
};

#endif // ITEM_DROP_H