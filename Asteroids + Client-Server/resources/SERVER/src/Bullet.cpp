#include "Bullet.h"

Bullet::Bullet()
{
	position.x = 0;
	position.y = 0;
	
	direction = ofPoint(0, 0);

	rotation = 0;
}

Bullet::~Bullet(void)
{

}

bool Bullet::setup()
{
	return false;
}

bool Bullet::setup(ofPoint shipPosition, ofPoint shipDirection, float size, float speed, float lifeTime, float rotation)
{
	position.x = shipPosition.x;
	position.y = shipPosition.y;

	direction.x = shipDirection.x;
	direction.y = shipDirection.y;
	this->size = size;
	this->speed = speed;
	this->lifeTime = lifeTime;
	this->rotation= rotation;
	return true;
}

void Bullet::update(float deltaTime)
{
	// Bullet's logic
	position.x += direction.x * speed * deltaTime;
	position.y += -direction.y * speed * deltaTime;
	lifeTime --;
}

void Bullet::draw(bool debug)
{	
	// Draw bullet
	ofCircle(position, size);
	
}

float Bullet::getLifeTime()
{
	return lifeTime;
}

void Bullet::setLifeTime(float life)
{
	lifeTime = life;
}