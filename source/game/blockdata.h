#ifndef BLOCK_DATA_H
#define BLOCK_DATA_H

#include <x2d/x2d.h>

enum BlockID;
enum ItemID;
class b2Fixture;

class BlockData
{
public:
	BlockData();
	BlockData(BlockID id, const shared_ptr<XTexture> &texture, const ItemID item, const float opacity);
	
	void setupFixture(b2Fixture *fixture) {}

	shared_ptr<XTexture> getTexture() const { return m_texture; }
	float getOpacity() const { return m_opacity; }

	static void init();
	static BlockData &get(const BlockID id);
	static XTextureAtlas *getBlockAtlas() { return s_blockAtlas; }
	
private:
	BlockID m_id;
	shared_ptr<XTexture> m_texture;
	ItemID m_item;
	float m_opacity;

	static vector<BlockData> s_blockData;
	static XTextureAtlas *s_blockAtlas;
};

#endif // BLOCK_DATA_H