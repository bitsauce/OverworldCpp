#include "camera.h"
#include "game.h"

Camera::Camera() :
	GameObject(DRAW_ORDER_CAMERA),
	m_position(0.0f, 0.0f)
{
	xd::Input::addMouseListener(this);
	xd::Window::addWindowListener(this);
	
	xd::Input::bind(xd::XD_KEY_PLUS, function<void()>(bind(&Camera::zoomIn, this)));
	xd::Input::bind(xd::XD_KEY_MINUS, function<void()>(bind(&Camera::zoomOut, this)));

	setZoomLevel(1.0f);
}

Vector2 Camera::getCenter() const
{
	return m_position + m_size * 0.5f;
}

Matrix4 Camera::getProjectionMatrix() const
{
	Matrix4 mat;
	mat.translate(-m_position.x, -m_position.y, 0.0f);
	mat.scale(m_zoomLevel);
	return mat;
}

void Camera::lookAt(Vector2i worldPoint)
{
	worldPoint -= m_size * 0.5f;
	m_position = worldPoint;
}

Vector2i Camera::getPosition()
{
	return m_position;
}

Vector2i Camera::getSize() const
{
	return m_size;
}

void Camera::setZoomLevel(const float zoomLevel)
{
	Vector2 center = getCenter();
	m_zoomLevel = zoomLevel;
	m_size = xd::Window::getSize() / m_zoomLevel;
	lookAt(center);
}

float Camera::getZoomLevel() const
{
	return m_zoomLevel;
}

void Camera::update()
{
	float speed = xd::Input::getKeyState(xd::XD_KEY_LCONTROL) ? 1.0f : 16.0f;
	if(xd::Input::getKeyState(xd::XD_KEY_LEFT)) {
		m_position.x -= speed/m_zoomLevel;
	}
	
	if(xd::Input::getKeyState(xd::XD_KEY_RIGHT)) {
		m_position.x += speed/m_zoomLevel;
	}
	
	if(xd::Input::getKeyState(xd::XD_KEY_UP)) {
		m_position.y -= speed/m_zoomLevel;
	}
	
	if(xd::Input::getKeyState(xd::XD_KEY_DOWN)) {
		m_position.y += speed/m_zoomLevel;
	}
}

void Camera::draw(xd::SpriteBatch*)
{
	Vector2 center = getCenter();
	World::getDebug()->setVariable("Camera", xd::util::floatToStr(center.x) + ", " + xd::util::floatToStr(center.y));
	World::getDebug()->setVariable("Zoom", xd::util::intToStr(m_zoomLevel*100) + "%");
}

void Camera::mouseWheelEvent(const int dt)
{
	Vector2 center = getCenter();
	if(dt > 0) zoomIn();
	if(dt < 0) zoomOut();
	lookAt(center);
}

void Camera::resizeEvent(uint width, uint height)
{
	setZoomLevel(m_zoomLevel);
}