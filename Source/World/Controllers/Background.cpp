#include "Background.h"
#include "TimeOfDay.h"

#include "Constants.h"

#include "World/World.h"

Color mixColors(Color c1, Color c2, const float a)
{
	return c1 * a + c2 * (1.0f - a);
}

Background::Background(World *world, Window *window) :
	Entity(world, ENTITY_BACKGROUND),
	m_camera(world->getCamera()),
	m_window(window),
	m_timeOfDay(world->getTimeOfDay()),
	m_topColor(255, 255, 255, 255),
	m_bottomColor(90, 170, 255, 255),
	m_wind(20.0f),
	m_sun(Game::GetInstance()->getResourceManager()->get<Texture2D>("Sprites/Backgrounds/Sun")),
	m_moon(Game::GetInstance()->getResourceManager()->get<Texture2D>("Sprites/Backgrounds/Moon"))
{
	m_sun.setSize(m_sun.getTexture()->getSize());
	m_moon.setSize(m_moon.getTexture()->getSize());

	m_sun.setOrigin(m_sun.getCenter());
	m_moon.setOrigin(m_moon.getCenter());

	m_sun.setDepth(-2.0f);
	m_moon.setDepth(-2.0f);

	Random rand;
	for(uint i = 0; i < 10; ++i)
	{
		m_clouds.push_back(new Cloud(Sprite(Game::GetInstance()->getResourceManager()->get<Texture2D>("Sprites/Backgrounds/Clouds/0")), 0.2f + rand.nextDouble() * 0.8f, (float) rand.nextInt(0, m_window->getHeight() / 2), (float) rand.nextInt(0, m_window->getWidth())));
	}

	m_layers.push_back(new Layer(Sprite(Game::GetInstance()->getResourceManager()->get<Texture2D>("Sprites/Backgrounds/Layer/0")), 0.5f, -1080.0f));
	m_layers.push_back(new Layer(Sprite(Game::GetInstance()->getResourceManager()->get<Texture2D>("Sprites/Backgrounds/Layer/1")), 0.25f));
	m_layers.push_back(new Layer(Sprite(Game::GetInstance()->getResourceManager()->get<Texture2D>("Sprites/Backgrounds/Layer/2")), 0.125f, -100.0f));
}

Background::~Background()
{
	for(Layer *layer : m_layers)
	{
		delete layer;
	}
	m_layers.clear();
}

void Background::onTick(TickEvent *e)
{
	// Get hour and mintue
	int hour = m_timeOfDay->getHour();
	//int minute = m_timeOfDay->getMinute();
	float time = m_timeOfDay->getTime();

	// Change background depending on time
	if(m_timeOfDay->isDay())
	{
		// Apply sunrise from 6:00 to 9:00
		if(hour >= 6 && hour < 9)
		{
			// Percentage of sunrise
			float minscale = 1.0f - (540 - time) / 180.0f; // Aka. (9 * 60 - time) / (6 * 60 - 9 * 60)
			m_topColor = mixColors(Color(255, 255, 255), Color(0, 0, 0), minscale);
			m_bottomColor = mixColors(Color(90, 170, 255), Color(10, 60, 110), minscale);
		}
		else
		{
			// Set day gradient
			m_topColor = Color(255, 255, 255);
			m_bottomColor = Color(90, 170, 255);
		}

		// Place sun
		float ang = (1140 - time) / 720.0f;
		Vector2F windowSize = m_window->getSize();
		Vector2F sunSize = m_sun.getSize();
		m_sun.setPosition(windowSize.x / 2.0f - sunSize.x / 2.0f + cos(PI*ang) * (windowSize.x / 2.0f + sunSize.x / 4.0f), windowSize.y / 2.0f - sin(PI*ang) * (windowSize.y / 2.0f + 64));
		m_sun.setRotation(180 * (1.0f - ang));
	}
	else
	{
		// Apply sunset from 18:00 to 21:00
		if(hour >= 18 && hour < 21)
		{
			// Percentage of sunset
			float minscale = 1.0f - (1260 - time) / 180.0f; // Aka. (21 * 60 - time) / (18 * 60 - 21 * 60)
			m_topColor = mixColors(Color(0, 0, 0, 255), Color(255, 255, 255), minscale);
			m_bottomColor = mixColors(Color(10, 60, 110, 255), Color(90, 170, 255), minscale);
		}
		else
		{
			// Set night gradient
			m_topColor = Color(0, 0, 0);
			m_bottomColor = Color(10, 60, 110);
		}

		// Place moon
		float ang = (1860 - (time >= 1140 ? time : time + 1440)) / 720.0f;
		Vector2F windowSize = m_window->getSize();
		Vector2F moonSize = m_moon.getSize();
		m_moon.setPosition(windowSize.x / 2.0f - moonSize.x / 2.0f + cos(PI * ang) * (windowSize.x / 2.0f + moonSize.x / 2.0f), windowSize.y / 2.0f - sin(PI * ang) * windowSize.y / 2.0f);
		m_moon.setRotation(180 * (1.0f - ang));
	}

	// Apply wind
	for(Cloud *cloud : m_clouds)
	{
		float x = cloud->sprite.getX() + m_wind * cloud->depth * e->getDelta();
		if(x > m_window->getWidth())
		{
			x = -cloud->sprite.getWidth();
			cloud->sprite.setY(Random().nextInt(0, m_window->getHeight() / 2));
		}
		cloud->sprite.setX(x);
	}
}

void Background::onDraw(DrawEvent *e)
{
	GraphicsContext *graphicsContext = e->getGraphicsContext();
	SpriteBatch *spriteBatch = (SpriteBatch*) e->getUserData();

	// Draw sky gradient
	m_vertices[0].set4f(VERTEX_POSITION, 0.0f, 0.0f);
	m_vertices[1].set4f(VERTEX_POSITION, 0.0f, (float) graphicsContext->getHeight());
	m_vertices[2].set4f(VERTEX_POSITION, (float) graphicsContext->getWidth(), 0.0f);
	m_vertices[3].set4f(VERTEX_POSITION, (float) graphicsContext->getWidth(), (float) graphicsContext->getHeight());
	m_vertices[0].set4ub(VERTEX_COLOR, m_topColor.r, m_topColor.g, m_topColor.b, m_topColor.a);
	m_vertices[1].set4ub(VERTEX_COLOR, m_bottomColor.r, m_bottomColor.g, m_bottomColor.b, m_bottomColor.a);
	m_vertices[2].set4ub(VERTEX_COLOR, m_topColor.r, m_topColor.g, m_topColor.b, m_topColor.a);
	m_vertices[3].set4ub(VERTEX_COLOR, m_bottomColor.r, m_bottomColor.g, m_bottomColor.b, m_bottomColor.a);
	graphicsContext->drawPrimitives(GraphicsContext::PRIMITIVE_TRIANGLE_STRIP, m_vertices, 4);

	// Draw sun/moon
	int hour = m_timeOfDay->getHour();
	if(hour >= 6 && hour < 18)
	{
		spriteBatch->drawSprite(m_sun);
	}
	else
	{
		spriteBatch->drawSprite(m_moon);
	}

	// Draw background layers
	/*for(Layer *layer : m_layers)
	{
		float ratio = m_window->getWidth() / 1920.0f;
		Vector2I cameraPos = m_camera->getCenter(e->getAlpha());
		Vector2I layerSize = Vector2I(m_window->getWidth(), (int) (layer->sprite.getTexture()->getHeight() * ratio));
		layer->sprite.setSize(layerSize);
		if(cameraPos.y > 0.0f)
		{
			layer->sprite.setPosition(0.0f, -cameraPos.y + m_window->getHeight() * 0.5f - layerSize.y - layer->yOffset * ratio);
		}
		else
		{
			layer->sprite.setPosition(0.0f, -cameraPos.y * layer->depth * 0.1f + m_window->getHeight() * 0.5f - layerSize.y - layer->yOffset * ratio);
		}
		layer->sprite.setRegion(TextureRegion((float) (cameraPos.x * layer->depth) / (float) layerSize.x, 0.0f, (float) (cameraPos.x * layer->depth + layerSize.x) / (float) layerSize.x, 1.0f));
		spriteBatch->drawSprite(layer->sprite);
	}*/

	// Draw clouds
	for(Cloud *cloud : m_clouds)
	{
		spriteBatch->drawSprite(cloud->sprite);
	}
}