#pragma once
#include <vector>
#include "Particle.h"
#include "Grid.h"

//2 fluids
enum class Visualizaton
{
	Water,
	Oil
};

class Simulations
{
	public:
		Simulations(bool isOil);
		void update(float dt, Visualizaton vis);
		void render(sf::RenderTarget& rt, Visualizaton vis);


	private:
		bool oil;
		int numberParticles;
		std::vector<Particle> particles;
		std::vector<std::vector<int>>neighborhoods;
		Grid grid;

		float kernel(sf::Vector2f x, float h);
		sf::Vector2f gradKernel(sf::Vector2f x, float h);
		float laplaceKernel(sf::Vector2f x, float h);

		void findNeighborhoods();

		void calculateDensity();

		void calculatePressure();

		void calculateForceDensity();

		void integrationStep(float dt);

		void collisionHandling();

		//void setParticlesColors(Visualizaton vis);

		sf::Texture metaballTexture;


};