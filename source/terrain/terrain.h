#ifndef TERRAIN_H
#define TERRAIN_H

#include "Config.h"

#include "Game/GameObject.h"
#include "Terrain/ChunkLoader.h"
#include "Terrain/TerrainChunk.h"
#include "Terrain/Generator/TerrainGen.h"

enum TerrainLayer;
enum BlockID;

class Terrain : public GameObject, public xd::WindowListener
{
public:
	Terrain();
	~Terrain();
	
	// Move?
	void saveChunks();
	void load(const IniFile &file);
	
	// BLOCKS
	bool setBlockAt(const int x, const int y, BlockID block, const TerrainLayer layer);
	BlockID getBlockAt(const int x, const int y, const TerrainLayer layer);
	bool isBlockAt(const int x, const int y, TerrainLayer layer);
	bool removeBlockAt(const int x, const int y, TerrainLayer layer);
	
	// BLOCK ENTITIES
	bool setBlockEntityAt(const int x, const int y, BlockEntityID blockEntity);
	
	// UPDATING
	void update();
	
	// DRAWING
	void draw(SpriteBatch *spriteBatch);

	// WINDOW
	void resizeEvent(uint width, uint height);

	ChunkLoader *getChunkLoader() { return &m_chunkLoader; }

private:

	// CHUNKS
	ChunkLoader m_chunkLoader;
	
	// GENERATOR
	TerrainGen generator;
};

#endif // TERRAIN_H