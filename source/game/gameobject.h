#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <x2d/x2d.h>

enum DrawOrder;

class GameObject
{
	friend class Game;
public:
	GameObject(DrawOrder depth);
	virtual ~GameObject();

	virtual void update() { }
	virtual void draw(XBatch*) { }

private:
	const DrawOrder m_depth;
};

class Game
{
	friend class GameObject;
public:
	static void update();
	static void draw();

private:
	static void addGameObject(GameObject *object);
	static void removeGameObject(GameObject *object);

	static list<GameObject*> s_gameObjects;
};

#endif // GAME_OBJECT_H