#include "ofApp.h"
#define IDONTCARE 0

//SOUNDS
ofSoundPlayer explosion;
ofSoundPlayer shot;
ofSoundPlayer music;

//////////////////////////////////////////////////////////////////////
// SETUP METHODS // SETUP METHODS // SETUP METHODS // SETUP METHODS //
//////////////////////////////////////////////////////////////////////

void ofApp::setup()
{
	// Set framerate to 60 FPS
	ofSetFrameRate(60);

	// ARDUINO SETUP
	setupArduino();
	
	// CLIENT SERVER SETUP
	setupOSC();

	
	// ASTEROIDS GAME SETUP
	// we decided to remove the sounds for this delivery because audio was not suported in the classrooms (54.008 and 54.009)
	loadAsteroids();
	noPlayers = 2;
	SpaceShip* player;	
	player = new SpaceShip(1);
	players.push_back(player);
	players[0]->setImage("ship_green.png");
	player = new SpaceShip(2);
	players.push_back(player);
	players[1]->setImage("ship_blue.png");
	ofBackground(0); // Set background to black
	debug = false;
	//shot.setSpeed(4.0f);
	//shot.setVolume(0.3f);
	
	
	ofxLabel line; // no idea what is this line for

	// GUI SETUP
	// the gui shows the remaining lifes and the score of each player but since we use 
	// the game logic of the other team, the lifes do not decrease when the player dies
	// now, the game finishes when a player reaches 5000 points
	gui.setup();
	gui.add(Player1Lifes.setup("Player1 Lifes", "3"));
	gui.add(Player2Lifes.setup("Player2 Lifes", "3"));
	gui.add(Player1Score.setup("Player1 Score", "0"));
	gui.add(Player2Score.setup("Player2 Score", "0"));
	serialConnection.writeByte('h');
}

void ofApp::setupOSC()
{
	ofSetVerticalSync(true);

	// LOG SETUP
	// Enable some logging information
	ofSetLogLevel(OF_LOG_VERBOSE); 

	// SEND DESTINATION
	// this is the last IP we tested the game on, replace with your own IP or with "localhost"
	serverDestination	= "10.200.184.22";
	serverPort = 9000;
	serverSender.setup(serverDestination, serverPort);

	// RECEIVE DESTINATION
	clientPort = 9001; 
	clientReceiver.setup(clientPort);
}

void ofApp::setupArduino()
{
	// COM PORT
	// change the arduino port if necessary!
	serialConnection.enumerateDevices();
	serialConnection.setup("COM4", 9600); 

	cyclesCounter = 0;
	cyclesJumped = 5;

	readAndSendMessage = false;
	memset(receivedBytes, 0, NUM_BYTES);
}

//////////////////////////////////////////////////////////////////////////
// UPDATE METHODS // UPDATE METHODS // UPDATE METHODS // UPDATE METHODS //
//////////////////////////////////////////////////////////////////////////

void ofApp::update()
{
	// ARDUINO MESSAGES
	updateArduino();

	// OSC MESSAGES
	sendMessage();
	getRenderFromServer();

	// PROCESSING BYTES
	players.at(1)->processArduino(receivedBytes);
}

void ofApp::updateArduino() {
	if(readAndSendMessage)
	{
		serialConnection.writeByte(0); //ERROR
		memset(receivedBytes, 0, NUM_BYTES);

		while ( serialConnection.readBytes(receivedBytes, NUM_BYTES) > 0 ) {}
		readAndSendMessage = false;
	}

	cyclesCounter++;
	if(cyclesCounter >= cyclesJumped)
	{
		readAndSendMessage = true;
		cyclesCounter = 0;
	}
}

//////////////////////////////////////////////////////////////////////
// DRAWING STUFF // DRAWING STUFF // DRAWING STUFF // DRAWING STUFF //
//////////////////////////////////////////////////////////////////////

void ofApp::draw()
{
	// DRAWING TIME
	// just render everything the server sent: players, asteroids, the scores and the powerup !
	for(int i = 0; i < asteroids.size(); i++) asteroids[i]->draw(debug);
	for (SpaceShip* player : players) player->draw(debug);	
	powerup.draw(debug);

	if(debug)
	{
		ofPushStyle();
		ofSetColor(255);
		ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 20);
		ofPopStyle();
	}

	drawScore();
}

void ofApp::drawScore() {
	//GUI Data Update
	if(getSpaceShip(1)){
		Player1Lifes = ofToString (getSpaceShip(1)->getLifes());
		Player1Score = ofToString (getSpaceShip(1)->getPoints());
	}
	if(getSpaceShip(2)){
		Player2Lifes = ofToString (getSpaceShip(2)->getLifes());
		Player2Score = ofToString (getSpaceShip(2)->getPoints());
	}

	//Draw the GUI
	gui.draw();
}

void ofApp::gameFinish()
{
	ofPushStyle();
		ofSetColor(ofColor(255));

		serialConnection.writeByte('g');

		ofDrawBitmapString("OMG! THE GAME IS OVER!", (ofGetWidth()/2)-100, (ofGetHeight())/2);
		if(ofToInt(Player1Score) > ofToInt(Player2Score)){ofDrawBitmapString("PLAYER 1 WINS!", (ofGetWidth()/2)-100, (ofGetHeight()/2)+20);}
		else{ofDrawBitmapString("PLAYER 2 WINS!", (ofGetWidth()/2)-100, (ofGetHeight()/2)+20);}
		ofDrawBitmapString("https://youtu.be/HaoySOGlZ_U?t=58s", (ofGetWidth()/2)-100, (ofGetHeight()/2)+40);
		ofDrawBitmapString("EXIT WITH ESC. BYE!", (ofGetWidth()/2)-100, (ofGetHeight()/2)+60);
	ofPopStyle();
}

void ofApp::loadAsteroids()
{
	// Load the file where asteroids definitions are saved
	if(XML.loadFile("asteroids.xml"))
	{
		cout << "Load Asteroids definition xml" << endl;
	}
	else
	{
		cout << "unable to load asteroids.xml check data/ folder" << endl;
	}

	// Check if there is the main xml tag defined or not (as an entry point)
	int areAsteroids = XML.getNumTags("asteroids");
	if(areAsteroids > 0)
	{
		cout << "Found asteroids defined" << endl;
		XML.pushTag("asteroids");

		// Parse all the asteroid definitions
		int numAsteroids = XML.getNumTags("asteroid");
		if(numAsteroids > 0)
		{
			cout << "There are " << numAsteroids << " asteroids defined" << endl;

			for(int i = 0; i < numAsteroids; i++)
			{
				XML.pushTag("asteroid", i);
				vector<ofPoint> tempDefinition;

				int numPoints = XML.getNumTags("point");
				cout << " defined by " << numPoints << " points" << endl;

				// Create a vector for each asteroid definition with the points that define its shape
				for(int i = 0; i < numPoints; i++)
				{
					cout << "  " << XML.getAttribute("point", "x", 0, i) << "x" << XML.getAttribute("point", "y", 0, i) << endl;

					tempDefinition.push_back(ofPoint(XML.getAttribute("point", "x", 0, i), XML.getAttribute("point", "y", 0, i)));
				}

				asteroidsDefinitions.push_back(tempDefinition);
				XML.popTag();
			}
		}
		XML.popTag();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// KEY METHODS // KEY METHODS // KEY METHODS // KEY METHODS // KEY METHODS // KEY METHODS //
////////////////////////////////////////////////////////////////////////////////////////////

void ofApp::keyPressed(int key)
{
	switch(key)
	{
		// If pressed F change debug mode
	case 'f':
	case 'F':
		debug = !debug;
		break;	
	case OF_KEY_LEFT_CONTROL:
		if (getSpaceShip(1)){
			//getSpaceShip(1)->createBullets();
			shot.play();
		}
		break;
	case OF_KEY_RIGHT_CONTROL:
		if (getSpaceShip(2)){
			//getSpaceShip(2)->createBullets();
			shot.play();
		}
		break;
	case OF_KEY_ESC:
		serialConnection.writeByte('e');
		//music.unloadSound();
		//explosion.unloadSound();
		//shot.unloadSound();
		exit();
		break;
	}	
}

void ofApp::keyReleased(int key) {}

void ofApp::mouseMoved(int x, int y ) {}

///////////////////////////////////////////////////////////////////////////////////////
// MOUSE METHODS // MOUSE METHODS // MOUSE METHODS // MOUSE METHODS // MOUSE METHODS //
///////////////////////////////////////////////////////////////////////////////////////

void ofApp::mouseDragged(int x, int y, int button) {}

void ofApp::mousePressed(int x, int y, int button)
{
	for(int i = 0; i < asteroids.size(); i++) 
	{
		if(asteroids[i]->getCollision(ofPoint(x, y))) 
		{
			cout << "Mouse hit Asteroid " << i << endl;
			splitAsteroid(i);

		}
	}
}

void ofApp::mouseReleased(int x, int y, int button) {}

void ofApp::windowResized(int w, int h) {}

void ofApp::gotMessage(ofMessage msg) {}

void ofApp::dragEvent(ofDragInfo dragInfo) {}

SpaceShip* ofApp::getSpaceShip(unsigned int player) {
	// getter for the spaceships based on their player id (noPlayer)
	for (SpaceShip* spaceship : players) if (spaceship->getId() == player) return spaceship;
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// OSC METHODS // OSC METHODS // OSC METHODS // OSC METHODS // OSC METHODS //
/////////////////////////////////////////////////////////////////////////////

string ofApp::getOscMsgAsString(ofxOscMessage m){	
	// method from the client server ofxOsc example !
	string msg_string;
	msg_string ="";
	for(int i = 0; i < m.getNumArgs(); i++){
		// get the argument type
		if(m.getArgType(i) == OFXOSC_TYPE_INT32){
			msg_string += ofToString(m.getArgAsInt32(i));
		}
		else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
			msg_string += ofToString(m.getArgAsFloat(i));
		}
		else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
			msg_string += m.getArgAsString(i);
		}
	}
	return msg_string;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CLIENT SERVER STUFF // CLIENT SERVER STUFF // CLIENT SERVER STUFF // CLIENT SERVER STUFF //
//////////////////////////////////////////////////////////////////////////////////////////////

void ofApp::sendMessage() 
{
	ofxOscMessage m;
	if (players.size() == 2)
	{
		// send all the information about the arduino controller to the server
		m.addIntArg(players[1]->arduinoStateToString());
		serverSender.sendMessage(m);

		clientTyping = ""; // clear out "typing"
	}
}

void ofApp::getRenderFromServer()
{	
	while(clientReceiver.hasWaitingMessages())
	{
		// the data is updated and (therefore) replaced every frane !
		asteroids.clear();
		players[0]->bullets.clear();
		players[1]->bullets.clear();

		// message m from the server will contain:
		// numer of asteroids
		// position of each asteroid
		// rotation of each asteroid
		// size of each asteroid
		// position of each player
		// rotation of each player
		// number of bullets of each player
		// position of each bullet of each player
		// points of each player
		// powerup type
		// game finished (bool)

		ofxOscMessage m;
		clientReceiver.getNextMessage(&m);

		///////////////////////////////////////////////////////////////////
		// ASTEROIDS // ASTEROIDS // ASTEROIDS // ASTEROIDS // ASTEROIDS //
		///////////////////////////////////////////////////////////////////

		int numasteroids = m.getArgAsInt32(0); // num of asteroids

		Asteroid* temp;

		// for each asteroid we will receive: 1 position x, 2 position y, 3 size, and 4 rotation
		for (int i = 0; i < numasteroids; i++)
		{						
			temp = new Asteroid();
			// SETUP ( shape = 0, size, speed = 0, rotation, position, direction )
			temp->setup( asteroidsDefinitions.at(IDONTCARE), m.getArgAsFloat(4*i+3), IDONTCARE, m.getArgAsFloat(4*i+4), ofPoint(m.getArgAsInt32(4*i+1), m.getArgAsInt32(4*i+2)), ofPoint(1.0, 1.0));
			// we decided to use shape 0 to avoid sending the shape over udp but 
			// it would be as easy as to add a new integer as a message argument
			asteroids.push_back(temp);
		}
		
		int lastposition = numasteroids*4; // to avoid long formulas we will be using a last position index

		//////////////////////////////////////////////////////////////////////////
		// PLAYER 1 // PLAYER 1 // PLAYER 1 // PLAYER 1 // PLAYER 1 // PLAYER 1 //
		//////////////////////////////////////////////////////////////////////////

		// for PLAYER 1 we will receive its position x and y, its rot and the information about its bullets
		players[0]->setPosition(ofPoint(m.getArgAsInt32(lastposition+1), m.getArgAsInt32(lastposition+2)));
		players[0]->setRotation(m.getArgAsFloat(lastposition+3));

		Bullet* newbullet;
		int numbullets = m.getArgAsInt32(lastposition+4);
		// for each bullet we will receive: 1 position x, 2 position y
		for (int j = 0; j < numbullets; j++)
		{
			newbullet = new Bullet();
			newbullet->setSize(1.2); // fixed bullet size
			newbullet->setPosition(ofPoint(m.getArgAsInt32(lastposition+2*j+5), m.getArgAsInt32(lastposition+2*j+6)));
			players[0]->bullets.push_back(newbullet);
		}

		players[0]->points = m.getArgAsInt32(lastposition+2*numbullets+5);

		lastposition += 2*numbullets+5; // last position + all the information about player 1

		//////////////////////////////////////////////////////////////////////////
		// PLAYER 2 // PLAYER 2 // PLAYER 2 // PLAYER 2 // PLAYER 2 // PLAYER 2 //
		//////////////////////////////////////////////////////////////////////////

		// for PLAYER 2 we will receive its position x and y, its rot and the information about its bullets
		players[1]->setPosition(ofPoint(m.getArgAsInt32(lastposition+1), m.getArgAsInt32(lastposition+2)));
		players[1]->setRotation(m.getArgAsFloat(lastposition+3));

		int numbullets2 = m.getArgAsInt32(lastposition+4);
		// for each bullet we will receive: 1 position x, 2 position y
		for (int j = 0; j < numbullets2; j++)
		{
			newbullet = new Bullet();
			newbullet->setSize(1.2); // fixed bullet size
			newbullet->setPosition(ofPoint(m.getArgAsInt32(lastposition+2*j+5), m.getArgAsInt32(lastposition+2*j+6)));
			players[1]->bullets.push_back(newbullet);
		}

		players[1]->points = m.getArgAsInt32(lastposition+numbullets2*2+5);
		cout << players[1]->points << endl;

		lastposition += numbullets2*2+5;
		
		//////////////////////////////////////////////////////////////////////////////////////////
		// POWERUP // POWERUP // POWERUP // POWERUP // POWERUP // POWERUP // POWERUP // POWERUP //
		//////////////////////////////////////////////////////////////////////////////////////////

		powerup.setType(m.getArgAsInt32(lastposition+1));
		ofPoint powerupposition = ofPoint(m.getArgAsInt32(lastposition+2),m.getArgAsInt32(lastposition+3));

		switch (powerup.getType())
		{
		case 1:
			powerup.setup("firePowerUp.png", powerupposition, powerup.getType(), 10);
			break;
		case 2:
			powerup.setup("speedPowerUp.png", powerupposition, powerup.getType(), 10);
			break;
		case 3:
			powerup.setup("scorePowerUp.png", powerupposition, powerup.getType(), 10);
			break;
		default:
			break;
		}

		//////////////////////////////////////////////////////////////////////////////////
		// GAME ENDED ? // GAME ENDED ? // GAME ENDED ? // GAME ENDED ? // GAME ENDED ? //
		//////////////////////////////////////////////////////////////////////////////////

		if (m.getArgAsInt32(lastposition+4) == 1) gameFinish();

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// THE CLIENT DOES NOT NEED THESE METHODS // THE CLIENT DOES NOT NEED THESE METHODS // THE CLIENT DOES NOT NEED THESE METHODS //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ofApp::generateAsteroids(int numAsteroids)
{
	float randSize = 0.0f;

	for ( int i = 0; i < numAsteroids; i++ ) 
	{
		// Logic to generate the Asteroids in different sizes
		// these are right now 120, 80, 40 and 20 (minimum)
		int randValue = ofRandom(0, 3);
		if(randValue == 2){
			randSize = 120;
		}
		else 
		{
			if(randValue == 1)
			{
				randSize = 80;
			}
			else 
			{
				randSize = 40;
			}
		}

		int asteroidShape = ofRandom(asteroidsDefinitions.size());

		Asteroid* temp = new Asteroid();
		temp->setup(asteroidsDefinitions.at(asteroidShape), randSize, ofRandom(100, 250), ofRandom(-PI, PI), 
			ofPoint(ofRandom(0, ofGetWidth()), ofRandom(0, ofGetHeight())), 
			ofPoint(ofRandom(-1, 1), ofRandom(-1, 1)).normalize());
		asteroids.push_back(temp);
	}
}

void ofApp::splitAsteroid(int which)
{
	int asteroidShape = 0;

	if(asteroids[which]->getSize() > 20) 
	{
		for(int i = 0; i < (asteroids[which]->getSize()/40)+1; i++) 
		{
			asteroidShape = ofRandom(asteroidsDefinitions.size());

			Asteroid* newAsteroid = new Asteroid();
			newAsteroid->setup(asteroidsDefinitions.at(asteroidShape), asteroids[which]->getSize()/2, asteroids[which]->getSpeed(), asteroids[which]->getRotation(), asteroids[which]->getPosition(), ofPoint(-(ofRandom(-1, 1)), ofRandom(-1, 1)));
			asteroids.push_back(newAsteroid);
		}
	} 

	asteroids.erase(asteroids.begin() + which);

	if(asteroids.size() == 0) 
	{
		generateAsteroids(ofRandom(4,6));
	}

	//Play sound explosion
	explosion.play();

	//GUI Data Update
	if(getSpaceShip(1)){
		Player1Lifes = ofToString (getSpaceShip(1)->getLifes());
		Player1Score = ofToString (getSpaceShip(1)->getPoints());
		switch(getSpaceShip(1)->getLifes())	{
		case 0: serialConnection.writeByte('s'); break;
		case 1: serialConnection.writeByte('o'); break;
		case 2: serialConnection.writeByte('w'); break;
		case 3: serialConnection.writeByte('h'); break;
		}
	}
	if(getSpaceShip(2)){
		Player2Lifes = ofToString (getSpaceShip(2)->getLifes());
		Player2Score = ofToString (getSpaceShip(2)->getPoints());
		switch(getSpaceShip(2)->getLifes())	{
		case 0: serialConnection.writeByte('f'); break;
		case 1: serialConnection.writeByte('u'); break;
		case 2: serialConnection.writeByte('d'); break;
		case 3: serialConnection.writeByte('t'); break;
		}
	}
}

bool ofApp::checkCollisions(Asteroid* currentasteroid) {
	float dist;
	// for each existing player check if any of its bullets is colliding with the current asteroid
	for(int currentPlayer = 0; currentPlayer < players.size(); currentPlayer++) {
		for(int currentBullet = 0; currentBullet < players[currentPlayer]->getBullets().size(); currentBullet++) {			
			if ( currentasteroid->getCollision(players[currentPlayer]->getBullets()[currentBullet]) ) {
				// if a bullet collides, the player gets points and the bullet is destroyed
				players[currentPlayer]->addPoints(currentBullet);
				// checkCollisions returns true and the current asteroid will be split or destroyed
				// since the return breaks the iteration, we cannot read outside the bullet vector
				return true;

			}
		}
		// if no bullet collided with the current asteroid, let's check the space ship
		if ( players[currentPlayer]->getCollision(currentasteroid) ) {
			if(!players[currentPlayer]->invulnerable){ //Look if the SpaceShip is invulnerable
				if (players[currentPlayer]->remainingLives > 0) {
					// if the player still has lifes left, it goes to the start (is resetted) but its points are kept
					players[currentPlayer]->reset();
					players[currentPlayer]->remainingLives = players[currentPlayer]->remainingLives - 1;
				} else {
					players.erase(players.begin() + currentPlayer);
					break;
				}
				return true;
			}
		}
	}
	return false;
}