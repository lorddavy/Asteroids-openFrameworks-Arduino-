#include "Bullet.h"

Bullet::Bullet()
{
	position.x = 0;
	position.y = 0;
	
	direction = ofPoint(0, 0);

	rotation = 0;
	lifeTime = 20;

}

Bullet::~Bullet(void)
{
	/*delete &color;
	delete &direction;
	delete &fireDistance;
	delete &lifeTime;
	delete &position;*/
}

bool Bullet::setup()
{

	return false;
}

bool Bullet::setup(ofPoint shipPosition, ofPoint shipDirection, float size, float speed, float lifeTime)
{
	position.x = shipPosition.x;
	position.y = shipPosition.y;

	direction.x = -shipDirection.x;
	direction.y = -shipDirection.y;
	
	this->size = size;
	this->speed = speed + 50;
	this->lifeTime = lifeTime;

	return true;
}

void Bullet::update(float deltaTime)
{
	// TODO
	// Bullet's logic
	lifeTime -= deltaTime;
	if (lifeTime > 0) {
		position.x += speed*direction.x*deltaTime;
		position.y += speed*direction.y*deltaTime;
	} else {
		//delete this;
	}
}

void Bullet::draw(bool debug)
{	
	// TODO
	// Draw bullet
	ofPushStyle();
	ofPushMatrix();	
	
	ofSetColor(ofColor(255, 255, 255));
	ofCircle(position, size);

	ofPopMatrix();
	ofPopStyle();
}

bool Bullet::isDead()
{
	if(lifeTime <= 0) return true;
}