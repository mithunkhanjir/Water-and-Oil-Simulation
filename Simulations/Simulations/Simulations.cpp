
#include <iostream>
#include <cmath>

#include "Simulations.h"
#include "Constants.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

using namespace std;
using namespace sf;
using namespace Constants;

//initializing 
Simulations::Simulations(bool isOil)
{
	oil = isOil;
	int particlesX = NUMBER_PARTICLES / 2.0f;
	int particlesY = NUMBER_PARTICLES;

	numberParticles = particlesX * particlesY;
	particles = vector<Particle>();


	float width = WIDTH / 4.2f;
	float height = 3.0f *HEIGHT / 4.0f;

	FloatRect particleRect = FloatRect((WIDTH - width) / 2, HEIGHT - height, width, height);

	float dx = particleRect.width / particlesX;
	float dy = particleRect.height / particlesY;

	for (int i = 0; i < NUMBER_PARTICLES/2.0f; i++)
	{
		for (int j = 0; j < NUMBER_PARTICLES; j++)
		{
			Vector2f pos = Vector2f(particleRect.left, particleRect.top) + Vector2f(i*dx, j*dy);
			Particle p = Particle(pos);
			particles.push_back(p);
		}
	}

	grid.updateStructure(particles);
	metaballTexture.loadFromFile("metaball.png");
	cout << "Simulations intialized with " << numberParticles << " particles." << endl;
}

//rendering the particles
void Simulations::render(RenderTarget &rt, Visualizaton vis)
{

	for (int i = 0; i < numberParticles; i++)
	{
		particles[i].force = Vector2f(0.0f, 0.0f);
	}

	if (vis == Visualizaton::Water || vis == Visualizaton::Oil)
	{
		Sprite sprite;
		sprite.setTexture(metaballTexture);
		sprite.setOrigin(metaballTexture.getSize().x / 2.0f, metaballTexture.getSize().y / 2.0f);
		float scale = 4.2f * PARTICLE_SPACING*SCALE / 256.0f;
		sprite.setScale(scale, scale);

		for (int i = 0; i < numberParticles; i++)
		{
			sprite.setPosition(particles[i].position * SCALE);
			rt.draw(sprite, BlendAdd);
		}
	}

}

//updating the simulation
void Simulations::update(float dt, Visualizaton vis)
{
	findNeighborhoods();
	calculateDensity();
	calculatePressure();
	calculateForceDensity();
	integrationStep(dt);
	collisionHandling();

	grid.updateStructure(particles);
}

// Poly6 Kernel
float Simulations::kernel(sf::Vector2f x, float h)
{
	float r2 = x.x * x.x + x.y * x.y;
	float h2 = h * h;

	if (r2 < 0 || r2 > h2) return 0.0f;

	return 315.0f / (64.0f * M_PI * pow(h, 9)) * pow(h2 - r2, 3);
}

// Gradient of Spiky Kernel
Vector2f Simulations::gradKernel(sf::Vector2f x, float h)
{
	float r = sqrt(x.x * x.x + x.y * x.y);
	if (r == 0.0f) return Vector2f(0.0f, 0.0f);

	float t1 = -45.0f / (M_PI * pow(h, 6));
	Vector2f t2 = x / r;
	float t3 = pow(h - r, 2);


	return  t1 * t2 * t3;
}

// Laplacian of Viscosity Kernel
float Simulations::laplaceKernel(sf::Vector2f x, float h)
{
	float r = sqrt(x.x * x.x + x.y * x.y);
	return 45.0f / (M_PI * pow(h, 6)) * (h - r);
}

//finding the neighborhood for each particle
void Simulations::findNeighborhoods()
{
	neighborhoods = vector<vector<int>>();
	float maxDist2 = KERNEL_RANGE * KERNEL_RANGE;
	for(const Particle &p:particles)
	{
		vector<int> neighbors = vector<int>();
		vector<Cell> neighboringCells = grid.getNeighboringCells(p.position);

		for(const Cell &cell:neighboringCells)
		{
			for(int index:cell)
			{
				Vector2f x = p.position - particles[index].position;
				float dist2 = x.x * x.x + x.y * x.y;
				if (dist2 <= maxDist2) {
					neighbors.push_back(index);
				}
			}
		}

		neighborhoods.push_back(neighbors);
	}
}

//density of the particles based on the neighborhood
void Simulations::calculateDensity()
{
	for (int i = 0; i < numberParticles; i++)
	{
		vector<int> neighbors = neighborhoods[i];
		float densitySum = 0.0f;

		for (int n = 0; n < neighbors.size(); n++)
		{
			int j = neighbors[n];

			Vector2f x = particles[i].position - particles[j].position;
			if (oil == false)
			{
				densitySum += particles[j].massWater * kernel(x, KERNEL_RANGE);
			}
			else
			{
				densitySum += particles[j].massOil * kernel(x, KERNEL_RANGE);
			}
			
		}

		particles[i].density = densitySum;
	}
}

//pressure for the particles
void Simulations::calculatePressure()
{
	for (int i = 0; i < numberParticles; i++)
	{
		if (oil == false) 
		{
			particles[i].pressure = max(STIFFNESS * (particles[i].density - REST_DENSITY_WATER), 0.0f);
		}
		else
		{
			particles[i].pressure = max(STIFFNESS * (particles[i].density - REST_DENSITY_OIL), 0.0f);
		}
		
	}
}

//force for the particles
void Simulations::calculateForceDensity()
{
	for (int i = 0; i < numberParticles; i++)
	{
		Vector2f fPressure = Vector2f(0.0f, 0.0f);
		Vector2f fViscosity = Vector2f(0.0f, 0.0f);
		Vector2f fGravity = Vector2f(0.0f, 0.0f);

		vector<int> neighbors = neighborhoods[i];

		//particles[i].color = 0;

		for (int n = 0; n < neighbors.size(); n++)
		{
			int j = neighbors[n];
			Vector2f x = particles[i].position - particles[j].position;

			// Pressure force density
			if (oil == false)
			{
				fPressure += particles[j].massWater * (particles[i].pressure + particles[j].pressure) / (2.0f * particles[j].density) * gradKernel(x, KERNEL_RANGE);
			}
			else
			{
				fPressure += particles[j].massOil * (particles[i].pressure + particles[j].pressure) / (2.0f * particles[j].density) * gradKernel(x, KERNEL_RANGE);
			}
			

			// Viscosity force density
			if (oil == false)
			{
				fViscosity += particles[j].massWater * (particles[j].velocity - particles[i].velocity) / particles[j].density * laplaceKernel(x, KERNEL_RANGE);
			}
			else
			{
				fViscosity += particles[j].massOil * (particles[j].velocity - particles[i].velocity) / particles[j].density * laplaceKernel(x, KERNEL_RANGE);
			}
			
		}

		// Gravitational force density
		fGravity = particles[i].density * Vector2f(0, GRAVITY);

		fPressure *= -1.0f;
		if (oil == false)
		{
			fViscosity *= VISCOSITY;
		}
		else
		{
			fViscosity *= VISCOSITY_OIL;
		}

		
		particles[i].force += fPressure + fViscosity + fGravity;
	}
}

void Simulations::integrationStep(float dt)
{
	for (int i = 0; i < numberParticles; i++)
	{
		particles[i].velocity += dt * particles[i].force / particles[i].density;
		particles[i].position += dt * particles[i].velocity;
	}
}

//handling collision with the walls
void Simulations::collisionHandling()
{
	for (int i = 0; i < numberParticles; i++)
	{
		//left
		if (particles[i].position.x < 0.0f)
		{
			particles[i].position.x = 0.0f;
			particles[i].velocity.x = -0.5f * particles[i].velocity.x;
		}

		//right
		else if (particles[i].position.x > WIDTH)
		{
			particles[i].position.x = WIDTH;
			particles[i].velocity.x = -0.5f * particles[i].velocity.x;
		}

		//up
		if (particles[i].position.y < 0.0f)
		{
			particles[i].position.y = 0.0f;
			particles[i].velocity.y = -0.5f * particles[i].velocity.y;
		}

		//right
		else if (particles[i].position.y > HEIGHT)
		{
			particles[i].position.y = HEIGHT;
			particles[i].velocity.y = -0.5f * particles[i].velocity.y;
		}
	}
}