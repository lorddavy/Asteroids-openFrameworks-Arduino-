#include "Entity.h"

Entity::Entity()
{
	position.x = 0;
	position.y = 0;
	
	direction = ofPoint(0, 0);

	rotation = 0;
}

Entity::~Entity()
{
	
}

bool Entity::getCollision(Entity *obj)
{
	// Calculate distance between objects
	float distanceBetweenObjects = ofDist(position.x, position.y, obj->getPosition().x, obj->getPosition().y);

	// If it is shorther than the size of one of the objects
	if(distanceBetweenObjects <= size || distanceBetweenObjects <= obj->getSize()) 
	{
		return true;
	}
	
	return false;
}

bool Entity::getCollision(ofPoint pos)
{
	// Calculate distance between objects
	float distanceBetweenObjects = ofDist(position.x, position.y, pos.x, pos.y);

	// If it is shorther than the size of the object
	if(distanceBetweenObjects <= size) 
	{
		return true;
	}
	
	return false;
}

void Entity::marginsWrap()
{
	// Check i object is outside screen horitzontal boundaries
	if(position.x-size > (float)ofGetWidth())
	{
		// When position is greater than right margin, set to the left one
		position.x = -size;
	}
	if(position.x+size < 0)
	{
		// When position is lower than left margin, set to the right side
		position.x = ofGetWidth();
	}
	
	// Check i object is outside screen vertical boundaries
	if(position.y-size > (float)ofGetHeight())
	{
		// When position is greater than the bottom, set it to the top side
		position.y = -size;
	}
	
	if(position.y+size < 0)
	{
		// When position is higher than screen top margin, set it in the bottom
		position.y = ofGetHeight();
	}
}

void Entity::setPosition(ofPoint pos)
{
	position = pos;
}

ofPoint Entity::getPosition()
{
	return position;
}

void Entity::setDirection(ofPoint direction)
{
	this->direction = direction;
}

ofPoint Entity::getDirection()
{
	return direction;
}

void Entity::setRotation(float angle)
{
	rotation = angle;
}

void Entity::addRotation(float _angle)
{
	rotation += _angle;
}

float Entity::getRotation()
{
	return rotation;
}

void Entity::setSize(float size)
{
	this->size = size;
}

float Entity::getSize()
{
	return size;
}

void Entity::setSpeed(float speed)
{
	this->speed = speed;
}

float Entity::getSpeed()
{
	return speed;
}
