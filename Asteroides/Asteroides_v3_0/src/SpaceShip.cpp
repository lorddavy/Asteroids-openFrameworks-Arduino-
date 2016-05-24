#include "SpaceShip.h"

//GLOBALS
int maxPlayerNo = 2;

SpaceShip::SpaceShip(const int player, bool ia)
{
	position.x = 0;
	position.y = 0;

	direction = ofPoint(0, 0);

	color = ofColor( 200 + 50 / maxPlayerNo * playerNo, rand() % 250, 100 + 155 / ( maxPlayerNo - playerNo + 1 ) );
	rotation = 0;

	remainingLives = 3;

	playerNo = player;
	this->ia = ia;
	isTurning = false;
	setup();

	ofEvent<ofSpaceShipFireEventArgs> spaceShipFires = ofEvent<ofSpaceShipFireEventArgs>();

	points = 0;
}

SpaceShip::~SpaceShip() {}

bool SpaceShip::setup()
{
	// TODO
	// Initialize spaceships:
	// - maybe subscribe to keyboard or mouse events
	ofRegisterKeyEvents(this);
	ofRegisterMouseEvents(this);

	// [...]

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

	// - initial directions
	//direction = ofPoint( sin( rotation ), -cos( rotation ) );

	// ADVISE
	// Default setup return could be true, you should control that if smth
	// does not setup properly, then setup could return false and control it

	return true;
}

void SpaceShip::reset() {
	rotation = playerNo * PI - PI/2;
	position.y = ofGetHeight() / 2 - size / 2 * cos( rotation );
	position.x = ofGetWidth() / maxPlayerNo * ( playerNo - 0.5 );

	//points = 0;

	thrust = false;
	speed = 100;
	
	invulnerable = true;
}

void SpaceShip::update(float elapsedTime)
{
	// Example on how to accelerate SpaceShip
	if (!ia) {
		if(thrust){
			addThrust(5);
		} 
		else 
		{
			addThrust(-2);
		}
	}

	// How firing could be handled with events
	if(isFiring)
	{
		ofSpaceShipFireEventArgs e = {position, ofPoint(cos(rotation), sin(rotation))};
		ofNotifyEvent(spaceShipFires, e, this);
		invulnerable = false;
	}

	// TODO
	// - add spaceship state update
	// - control spacehsip and window boundaries (i.e. marginSwap)

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

void SpaceShip::draw(bool debug)
{
	// each space ship manages and draws each of its bullets
	for (Bullet* bullet : bullets) bullet->draw(debug);

	// TODO
	// Draw correctly the SpaceShip, in the Hands On PDF you can find an explanation
	// on how to manage translations & rotations and how you should send data to OpenGL
	ofPoint size;
	size.x = 15;
	size.y = 20;
	float ratio = 0.8;

	ofPushStyle();
	ofPushMatrix();	

	if (!invulnerable){
		ofSetColor(color);
	}else{
	ofSetColor(ofColor(255,255,0));
	}

	ofTranslate( position.x, position.y );
	// because of the sin cos difference we need a + PI/2
	ofRotate( - 90 + rotation * RAD_TO_DEG );

	
	ofTriangle(ofPoint(0-size.x*0.5, size.y-size.y*0.5), ofPoint(size.x/2-size.x*0.5, 0-size.y*0.5), ofPoint(size.x-size.x*0.5, size.y-size.y*0.5));	
	

	ofPopMatrix();
	ofPopStyle();	
}

void SpaceShip::addThrust(float thrust)
{
	speed += thrust;

	// ADVICE
	// would be a good idea to clip the maximum speed
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

void SpaceShip::mouseMoved(ofMouseEventArgs & args) {

}

// a player could also be controlled with the mouse but it was too 
// difficult to play so we decided to remove the feature
void SpaceShip::mouseDragged(ofMouseEventArgs & args) {
	/*if (playerNo == 2) {
	//direction = ofPoint(-args.x+position.x, -args.y+position.y);
	rotation -= ( position.x - args.x ) * 0.0002;
	}*/
}

void SpaceShip::mousePressed(ofMouseEventArgs & args) {	
	/*cout << args.button << endl;
	if (playerNo == 2) {
	if (args.button == 0) {
	thrust = true;
	} else if (args.button == 2) {
	isFiring = true;
	}
	}*/
}

void SpaceShip::mouseReleased(ofMouseEventArgs & args) {
	/*if (playerNo == 1) {
	if (args.button == 0) {
	thrust = false;
	} else if (args.button == 2) {
	isFiring = false;
	}
	}*/

}

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