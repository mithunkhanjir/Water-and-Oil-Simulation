#pragma once
#include <SFML/Graphics.hpp>

class Particle
{
	public :
		sf::Vector2f position;
		sf::Vector2f force;
		sf::Vector2f velocity;

		float massWater;
		float massOil;
		float density;
		float pressure;

		float color;
		sf::Vector2f normal;

		Particle();
		Particle(sf::Vector2f position);

		sf::Color renderColor;
		float getVelocityLength() const;
		float getForceLength() const;
		float getNormalLength() const;
};