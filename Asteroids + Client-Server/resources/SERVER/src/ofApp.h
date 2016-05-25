#pragma once

#include "ofMain.h"

#include "ofxXmlSettings.h"

#include "Asteroid.h"
#include "Bullet.h"
#include "SpaceShip.h"
#include "Powerup.h"
#include "ofxOsc.h"

#define MAX_PUNTUATION 5000
#define NUM_BYTES 30

class ofApp : public ofBaseApp{

	public:
		~ofApp();
		void setup();
		void update();
		void draw();
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void receiveMessage();
		void syncClientServer();
		string getOscMsgAsString(ofxOscMessage m);
		void sendRenderToClient();

	private:
		void loadAsteroids();
		void generateAsteroids(int numAsteroids);
		void splitAsteroid(int asteroidID);
		void normalizeOfPoint(ofPoint& point);
		void collisionPlayerAndAsteroids(SpaceShip & player);
		void collisionBulletAndAsteroids(SpaceShip & player, Bullet * bullet);
		void takePowerup(SpaceShip & player, Powerup & powerup);


		ofxXmlSettings XML;
		ofSerial serialConnection; 

		vector<vector<ofPoint>> asteroidsDefinitions;
		vector<Asteroid*> asteroids;
		SpaceShip player1;
		SpaceShip player2;
		Powerup power;
		ofImage backgroundImage;
		ofSoundPlayer backgroundMusic;
		bool debug, readSerialByte;
		int powertime;
		int send, frameCount;
		unsigned byteInfoLength;
		enum playersControlIndexs {blueRotatesLeftIndex,blueRotatesRightIndex,blueFiresIndex,
								   greenRotatesLeftIndex, greenRotatesRightIndex, greenFiresIndex };
		

		//SERVER DATA
		ofxOscReceiver serverReceiver; // OSC receiver
		int serverPort; // port we're listening on: must match port from sender!
		string serverTyping; //messages you've received from the clientes

		ofxOscSender clientSender; // all-important ofxOscSender object
		string clientDestination; // IP address we're sending to
		int clientPort; // port we're sending to
		string clientTyping; // what we're going to send: some stuff you typed

};
