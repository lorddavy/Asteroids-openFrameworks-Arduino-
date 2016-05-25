#pragma once
#include "ofMain.h"

class Entity
{
	public:
		Entity();
		~Entity();
	
		virtual bool setup() = 0;
		virtual void update(float deltaTime) = 0;
		virtual void draw(bool debug) = 0;

		virtual bool getCollision(Entity* obj);
		virtual bool getCollision(ofPoint pos);
		virtual bool getCollision(ofPoint pos, ofPoint img, float size, float rotation);
		virtual void marginsWrap();
		virtual void marginsWrap(float width, float height); 

		virtual void setPosition(ofPoint pos);
		virtual void setInitialPosition(ofPoint pos);

		virtual ofPoint getPosition();
		virtual ofPoint getInitialPosition();

		virtual void setDirection(ofPoint direction);
		virtual ofPoint getDirection();

		virtual void setRotation(float angle);
		virtual void addRotation(float angle);
		virtual float getRotation();

		virtual void setSize(float size);
		virtual float getSize();

		virtual void setSpeed(float speed);
		virtual float getSpeed();

	protected:
		ofPoint position, direction, initialPosition;
		float rotation, size, speed;
};