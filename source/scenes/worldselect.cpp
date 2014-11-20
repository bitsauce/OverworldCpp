#include "worldselect.h"
#include "worldcreate.h"
#include "gui/uiobject.h"
#include "gui/button.h"
#include "constants.h"
#include "game.h"

void WorldSelectScene::showEvent()
{
	LOG("Scene: Select World");

	xd::FileSystemIterator itr("saves:/Overworld/", "*", xd::FileSystemIterator::DIRECTORIES);
	int i = 0;
	while(itr)
	{
		string worldDir = itr.next();
		Button *button = new Button(XIniFile(worldDir + "/world.ini").getValue("world", "name"), function<void()>(bind(&WorldSelectScene::worldClicked, this)));
		button->setAnchor(Vector2(0.5f, 0.5f));
		button->setPosition(Vector2(0.0f, -0.3f + i++*0.1f));
		button->setSize(Vector2(300.0f/CANVAS_WIDTH, 70.0f/CANVAS_HEIGHT));
		button->setUserData(new string(worldDir));
		m_uiObjects.push_back(button);
	}

	Button *cwBtn = new Button("Create World", function<void()>(bind(&WorldSelectScene::showCreateWorld, this)));
	cwBtn->setAnchor(Vector2(0.5f, 1.0f));
	cwBtn->setPosition(Vector2(0.0f, -0.1f));
	cwBtn->setSize(Vector2(300.0f/CANVAS_WIDTH, 70.0f/CANVAS_HEIGHT));
	m_uiObjects.push_back(cwBtn);
}

void WorldSelectScene::hideEvent()
{
	for(uint i = 0; i < m_uiObjects.size(); ++i)
	{
		delete m_uiObjects[i];
	}
}

void WorldSelectScene::drawEvent()
{
	gameDraw();
}

void WorldSelectScene::updateEvent()
{
	gameUpdate();
}

void WorldSelectScene::worldClicked()
{

}

void WorldSelectScene::showCreateWorld()
{
	XEngine::pushScene(scene::worldCreate);
}