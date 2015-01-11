#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CANVAS_WIDTH 800
#define CANVAS_HEIGHT 600

#define UI_DEBUG_FONT "Consolas 18"
#define UI_MENU_BUTTON_FONT "Arial Bold 27"
#define UI_MENU_BUTTON_TEXTURE ":/sprites/gui/menu_button.png"

// TERRAIN CONSTANTS
#define BLOCK_PX 16
#define BLOCK_PXF float(BLOCK_PX)

#define BORDER_PX 4
#define BORDER_PXF float(BORDER_PX)

#define FULL_BLOCK_PX (BLOCK_PX + BORDER_PX*2)
#define FULL_BLOCK_PXF float(FULL_BLOCK_PX)

#define CHUNK_BLOCKS 32
#define CHUNK_BLOCKSF float(CHUNK_BLOCKS)
#define CHUNK_PX (CHUNK_BLOCKS*BLOCK_PX)
#define CHUNK_PXF float(CHUNK_PX)

#define SUPER_CHUNK_CHUNKS 512
#define SUPER_CHUNK_CHUNKSF float(SUPER_CHUNK_CHUNKS)
#define SUPER_CHUNK_BLOCKS (SUPER_CHUNK_CHUNKS*CHUNK_BLOCKS)
#define SUPER_CHUNK_BLOCKSF float(SUPER_CHUNK_BLOCKS)
#define SUPER_CHUNK_PX (SUPER_CHUNK_CHUNKS*CHUNK_BLOCKS*BLOCK_PX)
#define SUPER_CHUNK_PXF float(SUPER_CHUNK_PX)

enum DrawOrder
{
	DRAW_ORDER_CAMERA,
	DRAW_ORDER_BACKGROUND,
	DRAW_ORDER_TIME_OF_DAY,
DRAW_ORDER_SCENE_START,
	DRAW_ORDER_BLOCK_ENTITY,
	DRAW_ORDER_TERRAIN,
	DRAW_ORDER_TERRAIN_LIGHTING,
	DRAW_ORDER_LIGHTING,
	DRAW_ORDER_PLAYER,
	DRAW_ORDER_TERRAIN_SHADOWS,
DRAW_ORDER_SCENE_END,
	DRAW_ORDER_UI,
	DRAW_ORDER_DEBUG
};

// BLOCK IDS
enum BlockID
{
	BLOCK_EMPTY,
	BLOCK_OCCUPIED,
	//BLOCK_BLOCKER, // TODO: Gonna need this some time for doors
	
	BLOCK_WOOD,
	//BLOCK_STONE,
	
	BLOCK_GRASS,
	BLOCK_STONE,
	
	BLOCK_LEAF,
	
	BLOCK_COUNT
};

// ITEM IDS
enum ItemID
{
	ITEM_NONE,

	ITEM_OAK_BLOCK,
	ITEM_,
	ITEM_GRASS_BLOCK,
	ITEM_STONE_BLOCK,
	ITEM_LEAF_BLOCK,

	ITEM_COUNT
};

// TERRAIN LAYERS
enum TerrainLayer
{
	TERRAIN_LAYER_BACK,
	TERRAIN_LAYER_MIDDLE,
	TERRAIN_LAYER_FRONT,
	TERRAIN_LAYER_COUNT
};

// DIRECTION
enum Direction
{
	NORTH		= 1 << 0,
	NORTH_EAST 	= 1 << 1,
	EAST		= 1 << 2,
	SOUTH_EAST 	= 1 << 3,
	SOUTH		= 1 << 4,
	SOUTH_WEST 	= 1 << 5,
	WEST		= 1 << 6,
	NORTH_WEST 	= 1 << 7,
	NESW		= NORTH | EAST | SOUTH | WEST
};

// SCENE IDS
enum SceneID
{
	SCENE_MAIN_MENU,
	SCENE_GAME,
	SCENE_WORLD_SELECT,
	SCENE_WORLD_CREATE,
	//SCENE_CHARACTER_CREATE,
	SCENE_COUNT
};

extern class UiObject *canvas;

#endif // CONSTANTS_H