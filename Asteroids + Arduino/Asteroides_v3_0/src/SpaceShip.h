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
		SpaceShip(const int player, bool ia = false);
		~SpaceShip();
	
		virtual bool setup();
		virtual void update(float deltaTime);
		virtual void draw(bool debug);
		unsigned int getId();
		
		void reset();

		void addThrust(float thrust);
		ofColor getColor() { return color; }

		//GUI
		int getLifes(){ return remainingLives; };
		int getPoints(){ return points; };

		void processArduino(unsigned char* receivedBytes);

		void keyPressed(ofKeyEventArgs & args);
		void keyReleased(ofKeyEventArgs & args);

		void mouseMoved(ofMouseEventArgs & args);
		void mouseDragged(ofMouseEventArgs & args);
		void mousePressed(ofMouseEventArgs & args);
		void mouseReleased(ofMouseEventArgs & args);

		//events
		ofEvent<ofSpaceShipFireEventArgs> spaceShipFires;
		static ofEvent<int> pressedKeyEvent;

		bool isTurning;
		bool turningRight;		

		//game stuff
		int remainingLives;
		void createBullets();
		vector<Bullet*>& getBullets() { return bullets; }
		void addPoints(int bullet) {
			points += this->getBullets()[bullet]->getSize();
			// calling the destructor ?
			//this->getBullets().at(bullet)->~Bullet();
			this->getBullets().erase(this->getBullets().begin() + bullet);
		}
		bool invulnerable;	
		

	private:
		// SpaceShip's behaviour control variables
		unsigned int playerNo;
		bool thrust, isFiring;
		bool ia;
		float mouseMovement;
		vector<Bullet*> bullets;
		unsigned int points;

		//sensors
		int buttonValue, potentiometerValue, tiltValue, joystickX, joystickY;
};