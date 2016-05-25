#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	// Set framerate to 60 FPS
	ofSetFrameRate(60);

	syncClientServer();

	/*
	Aqui voliem fer que es trobi la arduino conectant-nos a cada serial device
	enviant un byte concret i esperant la resposta, si la resposta es la que haviem
	definit al sketch, volia dir que aquell device era l'arduino i ens conectariem a aquest.
	Com era una part opcional, ho vam deixar per el final i no hem conseguit tenir-ho a temps.

		*/
		//Search and Connect to arduino
		//Farem una especie de Handshake enviant un byte
		//esperant una resposta concreta.
		/*int deviceId;
		bool connectedToArduino = false;
		char bytesReadString[4];
		while(!connectedToArduino){
			for(deviceId = 0 ; deviceId < serialConnection.getDeviceList().size(); deviceId++){

				serialConnection.setup(deviceId, 9600);


			}
		}*/ 
	serialConnection.setup("COM4", 9600);
	
	
	// BACKGROUND MUSIC :: DISABLE IF OFSOUND CRASHES
	backgroundMusic.loadSound("ImperishableReisen.mp3");
	backgroundMusic.setLoop(true); //Sound will loop
	backgroundMusic.play();
	ofSoundSetVolume(1.0);
	
	backgroundImage.loadImage("background.jpg");
	// Load Asteroids from XML
	loadAsteroids();
	// Create Asteroids
	generateAsteroids(2);

	//Setup players and assign a controllers
	player1.setup(ofPoint(ofGetWidth()/2 +235 , ofGetHeight()/2), "ship_green.png", ofPoint(ofGetWidth()/2 +235 , ofGetHeight()/2));
	player2.setup(ofPoint(ofGetWidth()/2 -300 , ofGetHeight()/2), "ship_blue.png", ofPoint(ofGetWidth()/2 -300 , ofGetHeight()/2));

	//CONTROLED BY SERVER
	player1.spaceshipControls(OF_KEY_UP, OF_KEY_LEFT, OF_KEY_RIGHT, OF_KEY_CONTROL);

	//CONTROLED BY CLIENT
	//player2.spaceshipControls('w','a','d',' ');

	//ofBackground(0); // Set background to black

	// Debug
	debug = true;

	//Powerup generator
	powertime = 0;
	send = 0;
	byteInfoLength = 6;
}

//--------------------------------------------------------------
void ofApp::update()
{
	// We get the time that last frame lasted, and use it to update asteroids logic
	// so their behaviour is independent to the framerate
	
	float elapsedTime = ofGetLastFrameTime();
	ofSoundUpdate();
	/*
	Reading controls from arduino.

	Per descodificar el byte, el shiftejem i fem un bitwise AND 
	per saber si el bit a la posició shiftejada es 1 o 0, aquest bit
	el guardem en un array d'ints per utilitzarlo com a true or false.

	Exemple: en el byte llegit es troba el numero 32 (100000) que significa 
	que el jugador verd ha disparat.
	Primera Iteració, shift 0.
	100000 >> 0 = 100000
	100000 AND 000001 = 000000 -> byteData[0]
	Ultima Iteració, shift 5
	100000 >> 5 = 000001
	00001 AND 000001 = 000001 -> byteData[5]
	
	Despres aquest index, estan mapejats amb un enum, per facilitar la comprensió
	del codi.
	*/
	readSerialByte=true;
	
	if(readSerialByte){
		int receivedByte = serialConnection.readByte();
		int byteData[6];
		int bit;
		for (int i = 0 ; i < byteInfoLength ; i++){
			bit = receivedByte >> i;
			if (bit & 1) byteData[i]=1;
    		else byteData[i]=0;
		}

		readSerialByte=false; frameCount=0;
		

		//Update the spaceships with the received info
		player1.setIsFiring (byteData[greenFiresIndex]);
		player1.setIsRotatingLeft(byteData[greenRotatesLeftIndex]);
		player1.setIsRotatingRight(byteData[greenRotatesRightIndex]);
		
		//player2.setIsFiring (byteData[blueFiresIndex]);
		//player2.setIsRotatingLeft(byteData[blueRotatesLeftIndex]);
		//player2.setIsRotatingRight(byteData[blueRotatesRightIndex]);
		
	} 

	//--------------------------
	
	//If not exists a winner
	if(player1.getScore() < MAX_PUNTUATION && player2.getScore() < MAX_PUNTUATION)
	{
		for(int i = 0; i < asteroids.size(); i++) 
		{
			asteroids[i]->update(elapsedTime);
		}
	
		player1.update(elapsedTime);
		player2.update(elapsedTime);
		
		//Check if exists any collision
		vector<Bullet*> bullets = player1.getBulletVector(); 
	
		collisionPlayerAndAsteroids(player1);
		collisionPlayerAndAsteroids(player2);

		for(int i=0; i < bullets.size(); i++)
			collisionBulletAndAsteroids(player1, bullets[i]);
		bullets.clear();

		bullets = player2.getBulletVector();
		for(int i=0; i < bullets.size(); i++)
			collisionBulletAndAsteroids(player2, bullets[i]);
		bullets.clear();
		
		//Powerup generator
		if(powertime < 0)
		{
			int type=(rand()%3)+1;
			switch (type)
			{
				case 1:
					power.setup("firePowerUp.png", ofPoint(rand()%ofGetWidth(),rand()%ofGetHeight()), type, 10);
					break;
				case 2:
					power.setup("speedPowerUp.png", ofPoint(rand()%ofGetWidth(),rand()%ofGetHeight()), type, 10);
					break;
				case 3:
					power.setup("scorePowerUp.png", ofPoint(rand()%ofGetWidth(),rand()%ofGetHeight()), type, 10);
					break;
			default:
				break;
			}
		
			powertime = 500;
		}
		
		//Powerup checker
		takePowerup(player1, power);
		takePowerup(player2, power);

		--powertime;
		}

	//SEND INFORMATION TO CLIENT
	sendRenderToClient();
}
//--------------------------------------------------------------
//Check if a player collides with an asteroid
void ofApp::collisionPlayerAndAsteroids(SpaceShip & player)
{
	bool collision = false;
	if(!player.isDead())
	{
		for(int i=0; i < asteroids.size() && !collision; i++)
		{
			if(asteroids[i]->getCollision(player.getPosition(), player.getImageDimensions(), player.getSize(), player.getRotation()))
			{
				collision = true; 
				player.setPosition(player.getInitialPosition());
				player.setRotation(0); 
				player.setSpeed(0);
				player.setMovementCount(0);
				player.setThrust(false); 
				player.setDirection(ofPoint(0,1));
				player.setFireBoost(-1); 
				player.setScoreBoost(-1); 
				player.setSpeedBoost(-1);
				player.setDeadState(true);	
			}
		}
	}
}
//--------------------------------------------------------------
//Check if a bullet collides with an asteroid
void ofApp::collisionBulletAndAsteroids(SpaceShip & player, Bullet * bullet)
{
	for(int i=0; i < asteroids.size(); i++)
	{
		if(bullet->getCollision(asteroids[i]))
		{
			bullet->setLifeTime(0);
			player.addScore(150-asteroids[i]->getSize());
			splitAsteroid(i);
		}
	}
}
//--------------------------------------------------------------
void ofApp::draw()
{
	backgroundImage.draw(0,0);
	for(int i = 0; i < asteroids.size(); i++) 
	{
		asteroids[i]->draw(debug);
	}

	//Draw players
	player1.draw(true);
	player2.draw(true);
	
	if(debug)
	{
		ofPushStyle();
			ofSetColor(255);
			ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 20);
		ofPopStyle();
	}

	//Draw a player score and a winner if exists	
	ofPushStyle();
		ofDrawBitmapString("Score: ", ofGetWidth()-90, 20);
		ofSetColor(0,255,0);
		ofDrawBitmapString(ofToString(player1.getScore()), ofGetWidth() - 90, 40);
		ofSetColor(0,130,130);
		ofDrawBitmapString(ofToString(player2.getScore()), ofGetWidth() - 90, 60);
	ofPopStyle();

	
	if(player1.getScore() >= MAX_PUNTUATION && player2.getScore() >= MAX_PUNTUATION)
	{
		ofPushStyle();
			ofDrawBitmapString("IT'S A DRAW! ", ofGetWidth()/2, 20);
		ofPopStyle();
	}
	else if(player1.getScore() >= MAX_PUNTUATION && player2.getScore() < MAX_PUNTUATION)
	{
		ofPushStyle();
			ofSetColor(0,255,0);
			ofDrawBitmapString("GREEN WIN! ", ofGetWidth()/2, 20);
		ofPopStyle();
	}
	else if(player1.getScore() < MAX_PUNTUATION && player2.getScore() >= MAX_PUNTUATION)
	{
		ofPushStyle();
			ofSetColor(0,130,130);
			ofDrawBitmapString("BLUE WIN! ", ofGetWidth()/2, 20);
		ofPopStyle();
	}
	power.draw(debug);

	char * scoreGreen = player1.scoreToString();
    char * scoreBlue = player2.scoreToString();

	receiveMessage();
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
		if(randValue == 2)
		{
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

		//int asteroidShape = ofRandom(asteroidsDefinitions.size());
		int asteroidShape = 0;
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
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{


	/*switch(key)
	{
		// If pressed Dm change debug mode
		case 'd':
		case 'D':
			debug = !debug;
			break;
			
	}*/
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{ 

}
//--------------------------------------------------------------
ofApp::~ofApp()
{
	for(vector<Asteroid*>::iterator it = asteroids.begin(); it != asteroids.end(); it++)
		delete *it;
}

void ofApp::takePowerup(SpaceShip & player, Powerup & powerup)
{
	if(powerup.getCollision(player.getPosition(), player.getImageDimensions(), player.getSize(), player.getRotation()))
	{
		switch (power.getType())
		{
			case 1:
				player.setFireBoost(200);
				break;
			case 2:
				player.setSpeedBoost(200);
				break;
			case 3:
				player.setScoreBoost(200);
				break;
			default:
				break;
		}
		powerup.setPosition(ofPoint(-100,-100));
	}
}

void ofApp::syncClientServer()
{
	//SETUP CONECTION
	//Server side
	//listen for incoming messages on a port; setup OSC receiver with usage:
	serverPort = 9000; //MI PUERTO DE ENTRADA
	serverReceiver.setup(serverPort); //ASIGNAR PUERTO DE ENTRADA
	//maxServerMessages = 38;

	//Client side
	//clientDestination = "localhost";
	clientDestination = "10.200.184.20"; //IP DESTINO
	clientPort = 9001;
	clientSender.setup(clientDestination, clientPort);
}

void ofApp::receiveMessage()
{
	while(serverReceiver.hasWaitingMessages())
	{
		// get the next message
		ofxOscMessage m;
		serverReceiver.getNextMessage(&m);

		//Recogemos la puntuacion como un entero y la transformamos en un char de 8 bits
		int num = m.getArgAsInt32(0);

		char message = num;

		//Desplazamos los bits a la derecha, cada posicion significa una cosa diferente
		int bit = (message >> 2);
		int p_firing = (bit & 1);
		bit = (message >> 1);
		int p_left = (bit & 1);
		bit = (message);
		int p_right = (bit & 1);

		player2.setIsFiring(p_firing);
		player2.setIsRotatingLeft(p_left);
		player2.setIsRotatingRight(p_right);
	}
}

void ofApp::sendRenderToClient()
{
	ofxOscMessage m;
	//SEND ASTEROIDS
	m.addIntArg(asteroids.size());
	for(Asteroid * asteroid : asteroids)
	{
		m.addIntArg(asteroid->getPosition().x);
		m.addIntArg(asteroid->getPosition().y);
		m.addFloatArg(asteroid->getSize());
		m.addFloatArg(asteroid->getRotation());
		//We can send shape as well but we decided not to make it more dificult since it is a vector with ofpoints and stuff
	}
	
	//SEND PLAYER 1 AND BULLETS DATA
	m.addIntArg(player1.getPosition().x);
	m.addIntArg(player1.getPosition().y);
	m.addFloatArg(player1.getRotation());
	m.addIntArg(player1.getBulletVector().size());
	for (Bullet * bullet : player1.getBulletVector())
	{
		m.addIntArg(bullet->getPosition().x);
		m.addIntArg(bullet->getPosition().y);
	}
	m.addIntArg(player1.getScore());

	//SEND PLAYER 1 AND BULLETS DATA
	m.addIntArg(player2.getPosition().x);
	m.addIntArg(player2.getPosition().y);
	m.addFloatArg(player2.getRotation());
	m.addIntArg(player2.getBulletVector().size());
	for (Bullet * bullet : player2.getBulletVector())
	{
		m.addIntArg(bullet->getPosition().x);
		m.addIntArg(bullet->getPosition().y);
	}
	m.addIntArg(player2.getScore());

	//SEND POWERUP DATA
	m.addIntArg(power.getType());
	m.addIntArg(power.getPosition().x);
	m.addIntArg(power.getPosition().y);

	//SEND GAMEOVER FLAG
	if(player1.getScore() < MAX_PUNTUATION && player2.getScore() < MAX_PUNTUATION)
		m.addIntArg(0);
	else
		m.addIntArg(1);
	
	clientSender.sendMessage(m);
}