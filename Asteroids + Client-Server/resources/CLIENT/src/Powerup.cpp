#include "Powerup.h"

Powerup::Powerup()
{
}

Powerup::~Powerup()
{
}

bool Powerup::setup()
{
	setImage("powerup.png");
	setPosition(ofPoint(300, 300));
	setSize(5);
	return true;
}
bool Powerup::setup(const string & pathImage, ofPoint pos, int type, int size)
{
	setImage(pathImage);
	setPosition(pos);
	setSize(size);
	setType(type);
	marginsWrap(img.width, img.height);
	return true;
}
void Powerup::update(float deltaTime)
{
}
void Powerup::draw(bool debug)
{
	//Translate and rotate spaceship image
	ofPushMatrix();
		img.draw(position.x,position.y);
	ofPopMatrix();
}

void Powerup::setImage(const string & pathImage)
{
	img.loadImage(pathImage);
	img.resize(img.getWidth() /4 , img.getHeight() /4);
}

bool Powerup::takePowerup(Entity *ent)
{
	return getCollision(ent);
}

int Powerup::getType()
{
	return type;
}
void Powerup::setType(int type){this->type = type;}