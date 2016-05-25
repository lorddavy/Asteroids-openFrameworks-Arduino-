#pragma once
#include "ofMain.h"

#include "Entity.h"

class Asteroid : public Entity
{
	public:
		Asteroid();
		~Asteroid();
	
		virtual bool setup();
		bool setup(vector<ofPoint> & shape, float size, float speed, float rotation, ofPoint position, ofPoint direction);
		virtual void update(float deltaTime);
		virtual void draw(bool debug);
	
		bool canBeSplit();

	private:
		// Variable that controls if an Asteroid has been destroyed or not
		bool canSplit;
	
		// vector that keeps all asteroid's shape vertices
		vector<ofPoint> asteroidShape;
};