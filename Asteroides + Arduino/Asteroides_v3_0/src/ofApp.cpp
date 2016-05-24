#include "ofApp.h"

//SOUNDS
ofSoundPlayer explosion;
ofSoundPlayer shot;
ofSoundPlayer music;

//--------------------------------------------------------------
void ofApp::setup()
{
	// Set framerate to 60 FPS
	ofSetFrameRate(60);

	//ARDUINO
	setupArduino();
	//ARDUINO

	// Load Asteroids from XML
	loadAsteroids();

	// Create Asteroids
	generateAsteroids(4);

	// TODO
	// Setup the following elements:
	// - players
	// - listeners? (depends on how you handle messages between entities)

	noPlayers = 2;
	SpaceShip* player;	
	for (int i = 0; i < noPlayers; i++) {
		player = new SpaceShip(i+1);
		players.push_back(player);
	}

	if (noPlayers == 1) {
		player = new SpaceShip(2, true);
		players.push_back(player);
	}

	//??? ofAddListener(); ???
	//ofAddListener(SpaceShip::pressedKeyEvent, this, &ofApp::keyPressed);

	// General logic
	ofBackground(0); // Set background to black

	// Debug
	debug = false;

	//Load Sounds

	//Explosions
	explosion.loadSound("Explode.aif");
	explosion.setSpeed(1.5f);

	//Music
	music.loadSound("Gradius.mid");
	music.setLoop(true); //Sound will loop
	//music.play();

	//SpaceShip Shots
	shot.loadSound("missile_fire.wav");
	shot.setSpeed(4.0f);
	shot.setVolume(0.3f);
	
	ofxLabel line;

	//GUI Setup
	gui.setup(); // most of the time you don't need a name	
	gui.add(Player1Lifes.setup("Player1 Lifes", "3"));
	gui.add(Player2Lifes.setup("Player2 Lifes", "3"));
	gui.add(Player1Score.setup("Player1 Score", "0"));
	gui.add(Player2Score.setup("Player2 Score", "0"));

}

void ofApp::setupArduino()
{
	serialConnection.enumerateDevices();
	serialConnection.setup("COM4", 9600);

	cyclesCounter = 0;
	cyclesJumped = 5;

	readAndSendMessage = false;

	memset(receivedBytes, 0, NUM_BYTES);
}

//--------------------------------------------------------------
void ofApp::update()
{
	//ARDUINO
	updateArduino();
	//ARDUINO

	// We get the time that last frame lasted, and use it to update asteroids logic
	// so their behaviour is independent to the framerate
	float elapsedTime = ofGetLastFrameTime();

	for(int i = 0; i < asteroids.size(); i++) 
	{
		if (checkCollisions( asteroids[i] ) ) {
			splitAsteroid(i);
			break;
		}
		asteroids[i]->update(elapsedTime);
	}

	// TODO
	// Implement calls for logic:
	// - players
	// - bullets
	// - collisions
	// - game conditions
	// [...]

	//Destroying Bullets
	for (SpaceShip* player : players) 
	{
		player->processArduino(receivedBytes);
		player->update(elapsedTime);
	}
	
	for(int currentPlayer = 0; currentPlayer < players.size(); currentPlayer++) {
		for(int currentBullet = 0; currentBullet < players[currentPlayer]->getBullets().size(); currentBullet++) {			
			
				if(players[currentPlayer]->getBullets()[currentBullet]->isDead())
				{
				//delete players[currentPlayer]->getBullets()[currentBullet];
				}

		}
	}
}

void ofApp::updateArduino() {
	if(readAndSendMessage)
	{
		serialConnection.writeByte(0);

		memset(receivedBytes, 0, NUM_BYTES);

		//serialConnection.readBytes(receivedBytes, NUM_BYTES);

		//cout << "Byte received: " << byteReturned << endl;
		while ( serialConnection.readBytes(receivedBytes, NUM_BYTES) > 0 ) 
		{
		}

		readAndSendMessage = false;
	}

	cyclesCounter++;
	if(cyclesCounter >= cyclesJumped)
	{
		readAndSendMessage = true;
		cyclesCounter = 0;
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// TODO
	// Draw all game entities (bullets? players? effects?)
	for(int i = 0; i < asteroids.size(); i++) asteroids[i]->draw(debug);
	//for (Bullet* bullet : bullets) bullet->draw(debug);
	for (SpaceShip* player : players) player->draw(debug);	

	if(debug)
	{
		ofPushStyle();
		ofSetColor(255);
		ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 20);
		ofPopStyle();
	}
	drawScore();

	if (players.size() == 0) {
		ofPushStyle();
		ofSetColor(ofColor(255));

		ofDrawBitmapString("OMG! THE GAME IS OVER!", (ofGetWidth()/2)-100, (ofGetHeight())/2);
		if(ofToInt(Player1Score) > ofToInt(Player2Score)){ofDrawBitmapString("PLAYER 1 WINS!", (ofGetWidth()/2)-100, (ofGetHeight()/2)+20);}
		else{ofDrawBitmapString("PLAYER 2 WINS!", (ofGetWidth()/2)-100, (ofGetHeight()/2)+20);}
		ofDrawBitmapString("https://youtu.be/HaoySOGlZ_U?t=58s", (ofGetWidth()/2)-100, (ofGetHeight()/2)+40);
		ofDrawBitmapString("EXIT WITH ESC. BYE!", (ofGetWidth()/2)-100, (ofGetHeight()/2)+60);
		ofPopStyle();
	}

	
}

//-------------------------------------------------------------
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

//-------------------------------------------------------------
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

//-----------------------------------------------------------
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
	}
	if(getSpaceShip(2)){
	Player2Lifes = ofToString (getSpaceShip(2)->getLifes());
	Player2Score = ofToString (getSpaceShip(2)->getPoints());
	}
	
}

//--------------------------------------------------------------
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
		music.unloadSound();
		explosion.unloadSound();
		shot.unloadSound();
		exit();
		break;
	}	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
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

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}

void ofApp::drawScore() {
	//Draw the GUI
	gui.draw();
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

SpaceShip* ofApp::getSpaceShip(unsigned int player) {
	// getter for the spaceships based on their player id (noPlayer)
	for (SpaceShip* spaceship : players) if (spaceship->getId() == player) return spaceship;
	return NULL;
}

