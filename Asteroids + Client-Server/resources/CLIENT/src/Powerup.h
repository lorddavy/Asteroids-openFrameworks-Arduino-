#pragma once
#include "ofMain.h"

#include "Entity.h"

class Powerup : public Entity
{
	public:
		Powerup();
		~Powerup();
	
		//virtual bool setup();
		bool setup();
		bool setup(const string & pathImage, ofPoint pos, int type, int size);
		virtual void update(float deltaTime);
		virtual void draw(bool debug);
		void setImage(const string & pathImage);
		bool takePowerup(Entity *ent);
		int getType();
		void setType(int type);
	

	private:
		ofImage img;
		int type;
		/*
		TYPES:
		1 - Speedboost
		2 - Fireboost
		3 - Scoreboost
		*/
};