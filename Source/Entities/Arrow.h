#pragma once

#include "Entity.h"
#include "Game/Game.h"
#include "Pawn.h"

class Arrow : public Entity
{
public:
	Arrow(const Json::Value &attributes);
	void onDraw(DrawEvent *e);
	bool plotTest(int x, int y);
	void onTick(TickEvent *e);

	static Arrow *Factory(const Json::Value &attributes)
	{
		return new Arrow(attributes);
	}

private:
	//Pawn *m_owner;
	Sprite m_sprite;
	Vector2F m_prevVelocity;
	uint m_hitState;
	float m_deleteTime;
	float m_moveToAlpha;
};