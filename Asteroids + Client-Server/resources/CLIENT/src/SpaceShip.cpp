#include "SpaceShip.h"

//GLOBALS
int maxPlayerNo = 2;

SpaceShip::SpaceShip(const int player, bool ia)
{
	position.x = 0;
	position.y = 0;

	direction = ofPoint(0, 0);

	if (player == 1) color = ofColor( 0,255,0 );
	else color = ofColor( 0,0,255 );
	rotation = 0;

	remainingLives = 3;

	playerNo = player;
	this->ia = ia;

	isTurning = false;
	isFiring = false;

	setup();

	ofEvent<ofSpaceShipFireEventArgs> spaceShipFires = ofEvent<ofSpaceShipFireEventArgs>();

	points = 0;
}

SpaceShip::~SpaceShip() {}

bool SpaceShip::setup()
{
	ofRegisterKeyEvents(this);
	ofRegisterMouseEvents(this);

	// initial stuff
	size = 40;
	thrust = false;
	speed = 100;	
	rotation = 0;
	invulnerable = false;

	// - initial positions
	// screen is divided horizontally by the number of players and each spaceship is placed in the center of each region
	rotation = playerNo * PI - PI/2;
	position.y = ofGetHeight() / 2 - size / 2 * cos( rotation );
	position.x = ofGetWidth() / maxPlayerNo * ( playerNo - 0.5 );

	return true;
}

void SpaceShip::reset() {
	rotation = playerNo * PI - PI/2;
	position.y = ofGetHeight() / 2 - size / 2 * cos( rotation );
	position.x = ofGetWidth() / maxPlayerNo * ( playerNo - 0.5 );

	thrust = false;
	speed = 100;

	invulnerable = true;
}

void SpaceShip::addPoints(int bullet) {
	points += this->getBullets()[bullet]->getSize();
	this->getBullets().erase(this->getBullets().begin() + bullet);
}

////////////////////////////////////////////////////////////////////////
// CLIENT DOES NOT USE THIS METHOD // CLIENT DOES NOT USE THIS METHOD //
////////////////////////////////////////////////////////////////////////

void SpaceShip::update(float elapsedTime)
{
	// Example on how to accelerate SpaceShip
	if(thrust){
		addThrust(5);
	} 
	else 
	{
		speed = 0;
	}

	// How firing could be handled with events
	if(isFiring)
	{
		ofSpaceShipFireEventArgs e = {position, ofPoint(cos(rotation), sin(rotation))};
		ofNotifyEvent(spaceShipFires, e, this);
		invulnerable = false;
		this->createBullets();
	}

	if (isTurning) {
		float temp = PI / (speed+10);
		if (turningRight) rotation += temp;
		else rotation -= temp;
	}

	direction = ofPoint( cos(rotation), sin(rotation) );

	position.x -= speed*direction.x*elapsedTime/2;
	position.y -= speed*direction.y*elapsedTime/2;

	marginsWrap();

	// each player updates its own bullets
	for (Bullet* bullet : bullets) bullet->update(elapsedTime);
}

void SpaceShip::setImage(const string & pathImage)
{
	img.loadImage(pathImage);
	img.resize(img.getWidth() /8 , img.getHeight() /8);
	
}

void SpaceShip::draw(bool debug)
{
	for(int i=0; i < bullets.size(); i++) bullets[i]->draw(debug);

	// translate and rotate spaceship image
	ofPushMatrix();
		ofTranslate(position.x + img.width/2,position.y + img.height/2, 0); // move pivot to centre
		ofRotate(rotation*1.01); // rotate from centre
		img.draw(-img.width/2,-img.height/2);
	ofPopMatrix();
}

void SpaceShip::addThrust(float thrust)
{
	speed += thrust;
	speed = CLAMP(speed, 10, 500);
}

void SpaceShip::keyPressed(ofKeyEventArgs & args)
{ 
	if (playerNo == 1)
		switch(args.key){
		case 'w':
			thrust = true;
			break;
		case 'a':
			isTurning = true;
			turningRight = false;
			//rotation -= PI / pow(speed+10, 0.45);
			break;
		case 'd':
			isTurning = true;
			turningRight = true;
			//rotation += PI / pow(speed+10, 0.45);
			break;
		case OF_KEY_LEFT_CONTROL:
			isFiring = true;
			break;
	}

	if (playerNo == 2)
		switch(args.key){
		case OF_KEY_UP:
			thrust = true;
			break;
		case OF_KEY_LEFT:
			isTurning = true;
			turningRight = false;
			//rotation -= PI / pow(speed+10, 0.45);
			break;
		case OF_KEY_RIGHT:
			isTurning = true;
			turningRight = true;
			//rotation += PI / pow(speed+10, 0.45);
			break;
		case OF_KEY_RIGHT_CONTROL:
			isFiring = true;
			break;
	}
}

void SpaceShip::keyReleased(ofKeyEventArgs & args)
{
	if (playerNo == 1) {
		switch(args.key){
		case 'w':
			thrust = false;
			break;
		case 'a':
			isTurning = false;
			break;
		case 'd':
			isTurning = false;
			break;
		case OF_KEY_LEFT_CONTROL:
			isFiring = false;
			break;
		}
	}
	if (playerNo == 2) {
		switch(args.key){
		case OF_KEY_UP:
			thrust = false;
			break;
		case OF_KEY_LEFT:
			isTurning = false;
			break;
		case OF_KEY_RIGHT:
			isTurning = false;
			break;
		case OF_KEY_RIGHT_CONTROL:
			isFiring = false;
			break;
		}
	}
}

void SpaceShip::mouseMoved(ofMouseEventArgs & args) {}

void SpaceShip::mouseDragged(ofMouseEventArgs & args) {}

void SpaceShip::mousePressed(ofMouseEventArgs & args) {}

void SpaceShip::mouseReleased(ofMouseEventArgs & args) {}

unsigned int SpaceShip::getId() {
	return playerNo;
}

void SpaceShip::createBullets() {
	Bullet* bullet;	
	bullet = new Bullet();
	// bullet setup requires the initial position, the direction (same as the player's), its size, the speed and the maximum life time value
	bullet->setup(this->getPosition(), this->getDirection(), 1.25, 500, 20);
	bullets.push_back(bullet);
}

//////////////////////////////////////////////////////////////////////////////////////
// ARDUINO BYTES PROCESSING // ARDUINO BYTES PROCESSING // ARDUINO BYTES PROCESSING //
//////////////////////////////////////////////////////////////////////////////////////

void SpaceShip::processArduino(unsigned char* receivedBytes) 
{
	if(playerNo == 1) {
		potentiometerValue = receivedBytes[0];
		potentiometerValue <<= 8;
		potentiometerValue += receivedBytes[1];

		if ( potentiometerValue > 2 ) 
		{
			isTurning = turningRight = true;
		}
		else if ( potentiometerValue < 2 )
		{
			isTurning = true;
			turningRight = false;
		}
		else 
		{
			isTurning = false;
		}

		buttonValue = receivedBytes[2];
		buttonValue <<= 8;
		buttonValue += receivedBytes[3];

		if ( buttonValue == 0 ) isFiring = false;
		else { 
			isFiring = true;
			//cout << "fire" << endl;
		}

		tiltValue = receivedBytes[4];
		tiltValue <<= 8;
		tiltValue += receivedBytes[5];

		if ( tiltValue == 0 ) thrust = true;
		else thrust = false;		
	}
	else if (playerNo = 2) {
		joystickX = receivedBytes[6];
		joystickX <<= 8;
		joystickX += receivedBytes[7];

		if(joystickX>=1020){ isFiring=true;
		}else{ 
			joystickX = (joystickX * 9 / 1024) + 48;
			isFiring=false;
			if(joystickX == 52)
			{
				isTurning = false;
			}
			else if(joystickX > 52 )
			{
				isTurning = turningRight = true;
			}
			else {
				isTurning = true;
				turningRight = false;
			}

		}

		joystickY = receivedBytes[8];
		joystickY <<= 8;
		joystickY += receivedBytes[9];

		joystickY = (joystickY * 9 / 1024) + 48;


		if(joystickY == 50)	thrust = false;
		else if(joystickY > 52) thrust = true;

	}
}

//////////////////////////////////////////////////////
// OSC STUFF // OSC STUFF // OSC STUFF // OSC STUFF //
//////////////////////////////////////////////////////

int SpaceShip::arduinoStateToString()
{
	// create a message using (cool) bit shifting
	string msg = "";	
	int entero = 0;

	if (isFiring) entero += 4;
	if (isTurning && !turningRight) entero += 2;
	if (isTurning && turningRight) entero += 1;

	msg = entero;
	return entero;
}