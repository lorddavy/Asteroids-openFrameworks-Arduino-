#include "Asteroid.h"

Asteroid::Asteroid()
{
	position.x = 0;
	position.y = 0;
	
	direction = ofPoint(0, 0);

	rotation = 0;
}

Asteroid::~Asteroid()
{
	asteroidShape.clear();
}

bool Asteroid::setup()
{
	rotation = PI / 2;

	// Check if it is an Asteroid that can be splitted or not
	// as we know, there are 4 possible sizes 120, 80, 40, and 20
	// being 20 the minimum
	if(size > 20)
	{
		canSplit = true;
	}
	else
	{
		canSplit = false;
	}

	return true;
}

bool Asteroid::setup(vector<ofPoint> & shape, float size, float speed, float rotation, ofPoint position, ofPoint direction)
{
	// We add the loaded shaper vertices to our own shape vector
	for(int i = 0; i < shape.size(); i++)
	{
		asteroidShape.push_back(shape.at(i));
	}

	// We initialize all the Asteroid variables
	this->size = size;
	this->speed = speed;
	this->rotation = rotation;
	this->position = position;
	this->direction = direction;

	// We finally call our entity base setup
	return setup();
}

void Asteroid::update(float elapsedTime)
{
	// Update position by integrating position with actual speed and
	// multiplied by the last frame time
	position += direction * speed * elapsedTime;

	marginsWrap();
}

void Asteroid::draw(bool debug)
{
	ofPolyline p;
	p.addVertices(asteroidShape);
	
	ofPushMatrix();
		glTranslatef(position.x, position.y, 0);
		glRotatef(rotation / PI * 180, 0,0,1);

		if(debug)
		{
			ofPushStyle();
				ofNoFill();
				ofCircle(0, 0,size);
			ofPopStyle();
		}

		ofScale(size/60,size/60,0);
		glTranslatef(-50, -50, 0);
		p.draw();
	ofPopMatrix();
}

bool Asteroid::canBeSplit()
{
	return canSplit;
}