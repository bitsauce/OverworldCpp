#ifndef BLOCK_ENTITY_DATA_H
#define BLOCK_ENTITY_DATA_H

#include "Config.h"

class Terrain;
class Thing;
class Game;
enum ThingID;

class ThingData
{
	enum PlacementRule
	{
		NEED_FLOOR		= 1 << 0,
		NEED_WALLS		= 1 << 1,
		NEED_ROOF		= 1 << 2,
		NEED_BACKGROUND	= 1 << 3
		//NEED_FOREGROUND = 1 << 0,
	};

public:
	ThingData(const int width, const int height, const int rule, const string &texture, const TextureRegion &textureRegion, function<Thing*(Game*, int, int)> factory);

	bool canPlace(const int x, const int y);
	Thing *tryPlace(const int x, const int y);

	Sprite getSprite() { return m_sprite; }

	static void init(Game *world);
	static ThingData &get(const ThingID id);

private:
	const int m_width, m_height;
	const int m_placementRule;
	const function<Thing*(Game*, int, int)> m_factory;
	Sprite m_sprite;

	static Game *s_game;
	static vector<ThingData*> s_blockEntityData;
};

#endif // BLOCK_ENTITY_DATA_H