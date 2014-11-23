#ifndef TILE_DATA_H
#define TILE_DATA_H

#include <x2d/x2d.h>

enum BlockID;
class b2Fixture;

class BlockData
{
public:
	BlockData(BlockID id, const shared_ptr<XTexture> &texture, const /*ItemID*/ uint i, const float opacity);
	
	void setupFixture(b2Fixture *fixture) {}
	vector<XVertex> getVertices(const int x, const int y, const uint state);
	vector<uint> getIndices();

	shared_ptr<XTexture> getTexture() const { return m_texture; }

	static void init();
	static BlockData &get(const BlockID id);

private:
	BlockID m_id;
	shared_ptr<XTexture> m_texture;
	uint m_itemID;
	float m_opacity;

	static vector<BlockData> s_blockData;
};

#endif // TILE_DATA_H