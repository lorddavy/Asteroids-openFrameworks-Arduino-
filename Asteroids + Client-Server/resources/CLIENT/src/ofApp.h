#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"

#include "Asteroid.h"
#include "Bullet.h"
#include "SpaceShip.h"
#include "Powerup.h"

#define NUM_BYTES 10

class ofApp : public ofBaseApp{

	public:
		void setup();
		void setupArduino();
		void setupOSC();
		void update();
		void updateArduino();
		
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
		void drawScore();
		SpaceShip* getSpaceShip(unsigned int player);

		//GUI
		ofxPanel gui;
		ofxLabel Player1Lifes;
		ofxLabel Player2Lifes;
		ofxLabel Player1Score;
		ofxLabel Player2Score;

		// Client side:
		ofxOscSender serverSender; // all-important ofxOscSender object
		string serverDestination; // IP address we're sending to
		int serverPort; // port we're sending to
		string clientTyping; // what we're going to send: some stuff you typed

		ofxOscReceiver clientReceiver; // OSC receiver
		int clientPort; // port where we listen for messages
		string clientMessages; // string containing the received messages for display

		// Message display variables
		vector<string>serverMessages; //vector containing the received messages for display
		unsigned int maxServerMessages; //nr of messages fitting on the screen

		vector<string>knownClients; //collected IP's of chat participants

		void gameFinish();

		//OSC METHODS
		string getOscMsgAsString(ofxOscMessage m); // Parse an OscMessage into a string for easy logging
		void sendMessage();
		void getRenderFromServer();

	private:
		void loadAsteroids();

		void generateAsteroids(int numAsteroids);
		void splitAsteroid(int asteroidID);

		void normalizeOfPoint(ofPoint& point);

		// TODO
		// Add methods that are responsible of:
		// - check collisions (e.g. between bullets&asteroids, and between players&asteroids)
		// - method that creates bullets (if you want ofApp to manage them)
		// [...]
		bool checkCollisions(Asteroid* currentasteroid);
		void createBullets(SpaceShip* player);

		ofxXmlSettings XML;

		Powerup powerup;

		vector<vector<ofPoint>> asteroidsDefinitions;
		//vector<Bullet*> bullets;

		vector<Asteroid*> asteroids;
		// TODO
		// Add variables that manage players and maybe bullets
		vector<SpaceShip*> players;

		//ARDUINO
		bool debug;	
		int noPlayers;

		ofSerial serialConnection;

		int cyclesCounter;
		int cyclesJumped;

		bool readAndSendMessage;

		unsigned char receivedBytes[NUM_BYTES];

		
};
