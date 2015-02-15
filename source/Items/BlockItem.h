#ifndef BLOCK_ITEM_H
#define BLOCK_ITEM_H

#include "Config.h"
#include "Items/ItemData.h"

enum BlockID;
enum TerrainLayer;

class BlockItem : public ItemData
{
public:
	BlockItem(const TerrainLayer layer, const BlockID blockID);

	void use(Player *player);
	void draw(Player *player, SpriteBatch *spriteBatch);

private:
	const TerrainLayer m_layer;
	const BlockID m_blockID;
};

#endif // BLOCK_ITEM_H