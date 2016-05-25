#pragma once
#include "ofMain.h"

#include "Entity.h"

class Bullet : public Entity
{
	public:
		Bullet();
		~Bullet();
	
		virtual bool setup();
		bool setup(ofPoint spaceShipPosition, ofPoint spaceShipDirection, float size, float speed, float lifeTime);
		virtual void update(float deltaTime);
		virtual void draw(bool debug);
		bool isDead();
	private:
		// Possible bullet's life control variables
		float fireDistance;
		float lifeTime;
};