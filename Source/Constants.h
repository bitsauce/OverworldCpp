#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "MessageIdentifiers.h"

enum PacketID
{
	ID_SET_BLOCK = ID_USER_PACKET_ENUM,
	ID_PLAYER_JOIN,
	ID_PLAYER_LEAVE,
	ID_NETWORK_OBJECT_UPDATE,
	ID_CREATE_ENTITY
};

#define CANVAS_WIDTH 800
#define CANVAS_HEIGHT 600

#define DEFAULT_PORT 45557

#define UI_INVENTORY_FONT ":/Fonts/Consolas11.fnt"
#define UI_DEBUG_FONT ":/Fonts/DebugFont.fnt"
#define UI_MENU_BUTTON_FONT ":/Fonts/ArialBold27.fnt"
#define UI_CHAT_FONT ":/Fonts/DebugFont.fnt"

#define UI_MENU_BUTTON_TEXTURE ":/sprites/gui/menu_button.png"

#define CHAT_FADE_TIME 10.0f
#define CHAT_WIDTH 400
#define CHAT_HEIGHT 250
#define CHAT_TEXT_INPUT_HEIGHT 27
#define CHAT_WIDTHF float(CHAT_WIDTH)
#define CHAT_HEIGHTF float(CHAT_HEIGHT)
#define CHAT_TEXT_INPUT_HEIGHTF float(CHAT_TEXT_INPUT_HEIGHT)

// TERRAIN CONSTANTS
#define BLOCK_PX 16
#define BLOCK_PXF float(BLOCK_PX)

#define CHUNK_BLOCKS 32
#define CHUNK_BLOCKSF float(CHUNK_BLOCKS)
#define CHUNK_PX (CHUNK_BLOCKS * BLOCK_PX)
#define CHUNK_PXF float(CHUNK_PX)

#define SUPER_CHUNK_CHUNKS 64
#define SUPER_CHUNK_CHUNKSF float(SUPER_CHUNK_CHUNKS)
#define SUPER_CHUNK_BLOCKS (SUPER_CHUNK_CHUNKS*CHUNK_BLOCKS)
#define SUPER_CHUNK_BLOCKSF float(SUPER_CHUNK_BLOCKS)
#define SUPER_CHUNK_PX (SUPER_CHUNK_CHUNKS*CHUNK_BLOCKS*BLOCK_PX)
#define SUPER_CHUNK_PXF float(SUPER_CHUNK_PX)

#define BLOCK_INDEX(x, y, z) (x) + (CHUNK_BLOCKS) * ((y) + (CHUNK_BLOCKS) * (z))

#define CHUNK_KEY(X, Y) (((X) & 0x0000FFFF) | (((Y) << 16) & 0xFFFF0000))

/**
 * \enum	GameStateID
 *
 * \brief	Game state IDs.
 */

enum GameStateID
{
	GAME_STATE_IN_GAME,
	GAME_STATE_MULTIPLAYER
};

/**
 * \enum	EntityID
 *
 * \brief	Entity IDs.
 */

enum EntityID
{
	ENTITY_NULL,

	ENTITY_TIME_OF_DAY,
	ENTITY_BACKGROUND,
	ENTITY_CAMERA,
	ENTITY_TERRAIN,
	ENTITY_LIGHTING,

	ENTITY_ARROW,
	ENTITY_PLAYER,
	ENTITY_ITEM_DROP,
	ENTITY_ZOMBIE,
	ENTITY_TORCH,

	ENTITY_COUNT
};

/**
 * \enum	BlockID
 *
 * \brief	Block IDs.
 */

enum BlockID
{
	// Invisible blocks
	BLOCK_EMPTY,
	BLOCK_ENTITY,

	BLOCK_GRASS,
	BLOCK_DIRT,
	BLOCK_DIRT_BACK,
	
	BLOCK_OAK_WOOD,
	BLOCK_OAK_LEAVES,
	
	BLOCK_STONE,

	BLOCK_ANIM_TEST,
	BLOCK_TORCH_TEST,

	//BLOCK_SAND,
	//BLOCK_MUD,
	//BLOCK_,
	
	
	BLOCK_COUNT
};

/**
 * \enum	BlockEntityID
 *
 * \brief	BlockEntity IDs.
 */

enum BlockEntityID
{
	BLOCK_ENTITY_NULL,
	BLOCK_ENTITY_TORCH,
	BLOCK_ENTITY_COUNT
};

/**
 * \enum	ItemID
 *
 * \brief	Item IDs.
 */

enum ItemID
{
	ITEM_NONE,
	
	ITEM_BLOCK_GRASS,
	ITEM_BLOCK_DIRT,
	ITEM_BLOCK_DIRT_BACK,
	ITEM_BLOCK_STONE,
	ITEM_BLOCK_OAK_WOOD,
	ITEM_BLOCK_OAK_LEAVES,

	ITEM_PICKAXE_IRON,

	ITEM_AXE_IRON,

	ITEM_TORCH,
	ITEM_CRAFTING_BENCH,
	ITEM_RED_CURRANT_BUSH,

	ITEM_BOW_WOODEN,

	ITEM_ARROW,

	ITEM_COUNT
};

/**
 * \enum	WorldLayer
 *
 * \brief	World Layer.
 */

enum WorldLayer
{
	WORLD_LAYER_BACK,
	WORLD_LAYER_MIDDLE,
	WORLD_LAYER_FRONT,
	WORLD_LAYER_COUNT
};

/**
 * \enum	Direction
 *
 * \brief	Direction.
 */

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

#endif // CONSTANTS_H