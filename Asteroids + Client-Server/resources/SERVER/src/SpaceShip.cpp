#include "SpaceShip.h"

SpaceShip::SpaceShip()
{
	position.x = 0;
	position.y = 0;
	speed = 0;
	direction = ofPoint(0, 1);
	thrust = false;
	isRotatingLeft = false; 
	isFiring = false;
	isRotatingRight = false;
	rotation = 0;
	coolDown = 0;
	score = 0;
	movementCount = 0; 
	ofEvent<ofSpaceShipFireEventArgs> spaceShipFires = ofEvent<ofSpaceShipFireEventArgs>();
}

SpaceShip::~SpaceShip()
{
	for(vector<Bullet*>::iterator it=bullets.begin(); it != bullets.end(); it++)
		delete *it;
}

bool SpaceShip::setup()
{
	return true;
}

bool SpaceShip::setup(const ofPoint & position, const string & pathImage, const ofPoint & initialPos)
{
	// TODO
	// Initialize spaceships:
	// - initial positions
	// - initial directions
	setPosition(position); 
	setInitialPosition(initialPos);
	setImage(pathImage);
	setSize(40);
	score = 0;
	speedboost = fireboost = scoreboost = -1;
	// - maybe subscribe to keyboard or mouse events
	// [...]
	ofRegisterKeyEvents(this);
	// ADVISE
	// Default setup return could be true, you should control that if smth
	// does not setup properly, then setup could return false and control it
	return true;
}

void SpaceShip::update(float elapsedTime)
{
	if(thrust)	
		addThrust(5); 
	/*else 
	{
		addThrust(-2);
	}*/

	if(isFiring && coolDown < 0) 
	{

		Bullet * b = new Bullet();
		ofPoint positionBullet = ofPoint(position.x + img.width/2, position.y + img.height / 2); 
		b->setup(positionBullet, direction, 1.5, 400, 250, rotation); 
		bullets.push_back(b);
		coolDown = 10;
		ofSpaceShipFireEventArgs e = {position, ofPoint(cos(rotation), sin(rotation))};
		ofNotifyEvent(spaceShipFires, e, this);
		
	}

	//Check the lifeTime bullets
	for(int i=0; i < bullets.size(); i++)
	{
		bullets[i]->update(elapsedTime);
		if(bullets[i]->getLifeTime() <= 0)
			bullets.erase(bullets.begin() + i);
	}

	if(isRotatingLeft)
		addRotation(-5);
	else if(isRotatingRight)
		addRotation(5);
	
	//Calculates the total angle through the ship rotation for calculates the direction point
	float angle = 90 - rotation; 
		
	if(angle <= 0)
	{
		angle = 360 - angle;
		rotation -= angle;
	}	

	if (angle >= 360)
		angle = angle - 360; 
	
	direction = ofPoint(cos(angle*PI/180), sin(angle*PI/180));
	if(speedboost > 0)
	{
		position.x += direction.x * speed * 2 * elapsedTime;
		position.y += -direction.y * speed * 2 * elapsedTime; 
	}
	else
	{
		position.x += direction.x * speed * elapsedTime;
		position.y += -direction.y * speed * elapsedTime; 
	}
	
	if(coolDown >= 0)
		coolDown --;

	marginsWrap(img.width, img.height);

	if(movementCount > 60)
	{
		thrust = true; 
		setDeadState(false);
	}
	else
		movementCount++;

	if(speedboost > 0)
		--speedboost;
	if(fireboost > 0)
	{
		--fireboost;
		coolDown --;
	}
	if(scoreboost > 0)
		--scoreboost;
}

void SpaceShip::draw(bool debug)
{
	for(int i=0; i < bullets.size(); i++)
	{
		bullets[i]->draw(debug);
	}

	//Translate and rotate spaceship image
	ofPushMatrix();
		ofTranslate(position.x + img.width/2,position.y + img.height/2, 0);//move pivot to centre
		ofRotate(rotation*1.01); //rotate from centre
		img.draw(-img.width/2,-img.height/2);
	ofPopMatrix();
}

void SpaceShip::addThrust(float thrust)
{
	
	if(thrust > 0)
	{
		if(speed+thrust <=200)
			speed += thrust;
	}
	else
		if(speed-thrust >= 0)
			speed += thrust;
		
}

void SpaceShip::keyPressed(ofKeyEventArgs & args)
{

	if (args.key == accelerateKey && !thrust ) 
	{
		thrust = true;
	}
	if (args.key == fireKey ) isFiring = true;
	if(args.key == rotateLeftKey) isRotatingLeft = true;
	if(args.key == rotateRightKey) isRotatingRight = true;
}

void SpaceShip::keyReleased(ofKeyEventArgs & args)
{
	//if (args.key == accelerateKey )thrust = false;
	if (args.key == fireKey ) isFiring = false;
	if(args.key == rotateLeftKey) isRotatingLeft = false;
	if(args.key == rotateRightKey) isRotatingRight = false;
}

void SpaceShip::setImage(const string & pathImage)
{
	img.loadImage(pathImage);
	img.resize(img.getWidth() /8 , img.getHeight() /8);
	
}

void SpaceShip::spaceshipControls(int acc, int rotLeft, int rotRight, int fire)
{
	this->accelerateKey = acc;
	this->rotateLeftKey = rotLeft;
	this->rotateRightKey = rotRight;
	this->fireKey = fire;

}

float SpaceShip::getWidth()
{
	return img.getWidth()/8;
}

float SpaceShip::getHeight()
{
	return img.getHeight()/8;
}

ofPoint SpaceShip::getImageDimensions()
{
	return ofPoint(img.width, img.height);
}

vector<Bullet*> SpaceShip::getBulletVector()
{
	return bullets;
}

int SpaceShip::getScore()
{
	return this->score;
}

void SpaceShip::addScore(int points)
{
	if(scoreboost > 0)
		this->score += (points*2);
	else
		this->score += points;
}

void SpaceShip::setMovementCount(const unsigned int & number)
{
	movementCount = number; 
}

void SpaceShip::setThrust(const bool & state)
{
	thrust = state; 
}
void SpaceShip::setSpeedBoost(int num){speedboost = num;}
void SpaceShip::setFireBoost(int num){fireboost = num;}
void SpaceShip::setScoreBoost(int num){scoreboost = num;}
void SpaceShip::setDeadState(const bool & state) { dead = state; }
bool SpaceShip::isDead() { return dead; }

void SpaceShip::setIsFiring(int & status){
	if(status)
		isFiring = true;
	else
		isFiring = false;
}
void SpaceShip::setIsRotatingLeft(int & status){isRotatingLeft = status;}
void SpaceShip::setIsRotatingRight(int & status){isRotatingRight=status;}

char* SpaceShip::scoreToString()
{
	/*
	TRANSFORMA LA PUNTUACION DE UN ENTERO A UN ARRAY DE CHARS DONDE CADA POSICION DEL ARRAY ES EL DIGITO CORRESPONDIENTE
	LOS 0 A LA IZQUIERDA CUENTAN Y SE PONEN IGUALMENTE
	ALGORITMO NO DINAMICO, SE TIENE QUE MODIFICAR UN POCO LA LOGICA SI CAMBIA LA PUNTUACION

	Ejemplo:	4316
				4316/1000 = 4 => primer digito
				(4316-4000)/100 = 316/100 = 3 => segundo digito
				(4316-4000-300)/10 = 16/10 = 1 => tercer digito
				(4316-4000-300-10) = 6 => cuarto digito
	*/
	char * textscore = new char();
	textscore[0] = this->score/1000;
	textscore[1] = (this->score-textscore[0]*1000)/100;
	textscore[2] = (this->score-textscore[0]*1000-textscore[1]*100)/10;
	textscore[3] = (this->score-textscore[0]*1000-textscore[1]*100-textscore[2]*10);

	textscore[0] += 48;
	textscore[1] += 48;
	textscore[2] += 48;
	textscore[3] += 48;

	return textscore;
}