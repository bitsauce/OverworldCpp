#ifndef ITEM_DATA_H
#define ITEM_DATA_H

#include "Config.h"

class Player;
enum ItemID;

class ItemData
{
	friend class GameManager;
public:
	ItemData();
	virtual void use(Player *player) = 0;
	virtual void draw(Player *player, SpriteBatch *spriteBatch) = 0;
	static ItemData *get(const ItemID id) { return s_itemData[id]; }

	ItemID getID() const { return m_id; }
	Texture2DPtr getIconTexture() const { return m_iconTexture; }
	uint getMaxStack() const { return m_maxStack; }

protected:
	virtual void set(const ItemID id, const string &name, const string &desc, const string &texture, const uint maxStack, const bool singleShot);

private:
	ItemID m_id;
	string m_name;
	string m_desc;
	uint m_maxStack;
	Texture2DPtr m_iconTexture;
	bool m_singleShot;

	static void init();
	static vector<ItemData*> s_itemData;
};

#endif // ITEM_DATA_H