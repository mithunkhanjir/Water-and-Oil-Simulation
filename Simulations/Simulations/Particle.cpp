#include "Particle.h"
#include "Constants.h"

using namespace sf;


Particle::Particle()
{
	Particle(Vector2f());
}

//intializing the particle & creating a vector to hold the particles
Particle::Particle(Vector2f pos)
{
	position = pos;
	velocity = Vector2f();
	force = Vector2f();
	massWater = Constants::PARTICLE_MASS_WATER;
	massOil = Constants::PARTICLE_MASS_OIL;

	density = 0;
	pressure = 0;

	color = 0;
	normal = Vector2f();

}

float Particle::getVelocityLength() const
{
	return (velocity.x * velocity.x) + (velocity.y * velocity.y);

}

float Particle::getForceLength() const
{
	return (force.x * force.x) + (force.y + force.y);
}

float Particle::getNormalLength() const
{
	return (normal.x * normal.x) + (normal.y + normal.y);
}