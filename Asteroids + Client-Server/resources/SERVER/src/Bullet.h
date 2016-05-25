#pragma once
#include "ofMain.h"

#include "Entity.h"

class Bullet : public Entity
{
	public:
		Bullet();
		~Bullet();
	
		virtual bool setup();
		bool setup(ofPoint spaceShipPosition, ofPoint spaceShipDirection, float size, float speed, float lifeTime, float rotation);
		virtual void update(float deltaTime);
		virtual void draw(bool debug);
		float getLifeTime();	
		void setLifeTime(float life);
	
	private:
		float fireDistance;
		float lifeTime;
		float rotation;
};