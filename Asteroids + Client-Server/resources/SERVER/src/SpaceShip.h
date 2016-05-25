#pragma once
#include "ofMain.h"

#include "Entity.h"
#include "Bullet.h"

// Possible SpaceShip implementation

// Structure to save initial bullet data, so whenever user shoots
// we can send a message with this structure for knowing bullet's
// initial position and direction
struct ofSpaceShipFireEventArgs
{
	ofPoint spaceShipPosition;
	ofPoint spaceShitDirection;
};

class SpaceShip : public Entity
{	
	public:
		SpaceShip();
		~SpaceShip();
	
		virtual bool setup();
		virtual bool setup(const ofPoint & position, const string & pathImage, const ofPoint & initialPosition);
		
		virtual void update(float deltaTime);
		virtual void draw(bool debug);

		void addThrust(float thrust);
		
		void setImage(const string & pathImage);
		void setMovementCount(const unsigned int & number); 
		void setThrust(const bool & state); 
		void setSpeedBoost(int num);
		void setFireBoost(int num);
		void setScoreBoost(int num);
		void setDeadState(const bool & state); 
		void setIsFiring(int & status);
		void setIsRotatingLeft(int & status);
		void setIsRotatingRight(int & status);

		void keyPressed(ofKeyEventArgs & args);
		void keyReleased(ofKeyEventArgs & args);
		void spaceshipControls(int acc, int rotLeft, int rotRight, int fire);
		void addScore(int points);
		int getScore();
		bool isDead(); 

		float getWidth();
		float getHeight();
		char * scoreToString();

		ofPoint getImageDimensions();		

		ofEvent<ofSpaceShipFireEventArgs> spaceShipFires;
		vector<Bullet*> getBulletVector();

	private:
		// SpaceShip's behaviour control variables
		bool thrust, isFiring, isRotatingLeft, isRotatingRight, dead;
		ofImage img;
		vector<Bullet*> bullets; 
		int accelerateKey, rotateLeftKey, rotateRightKey, fireKey, coolDown, score, movementCount;

		int speedboost, fireboost, scoreboost;
};