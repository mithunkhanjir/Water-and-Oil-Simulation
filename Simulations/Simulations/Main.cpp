
#include <SFML/Graphics.hpp>
#include <iostream>


#include "Constants.h"
#include "Simulations.h"

using namespace std;
using namespace Constants;

Visualizaton vis = Visualizaton::Water;

//shaders for water and oil
sf::Shader shader;
sf::Shader shaderOil;
bool isOil = false;

Simulations sph = Simulations(isOil);
bool pauseAnimation = false;

void init();
void update(float);
void render(sf::RenderWindow &, sf::RenderTexture &);

int main()
{
	//creating  a window
	sf::RenderWindow window(sf::VideoMode(RENDER_WIDTH * WINDOW_SCALE, RENDER_HEIGHT * WINDOW_SCALE), "Fluid Simulation using SPH");
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(60);

	sf::RenderTexture renderTexture;
	renderTexture.create(RENDER_WIDTH, RENDER_HEIGHT);
	init();
	
	float time = 0.0f;

	//till the window stays open
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}

				if (event.key.code == sf::Keyboard::Space)
				{
					pauseAnimation = !pauseAnimation;
				}

				//visualization for water
				if (event.key.code == sf::Keyboard::Num1)
				{
					isOil = false;
					sph = Simulations(isOil);
					vis = Visualizaton::Water;
				}

				//visualization for oil
				if (event.key.code == sf::Keyboard::Num2)
				{
					isOil = true;
					sph = Simulations(isOil);
					vis = Visualizaton::Oil;
				}
				
			}


		}

		if (!pauseAnimation)
		{
			update(TIMESTEP);
			time += TIMESTEP;
		}

		renderTexture.clear(sf::Color::Black);

		render(window, renderTexture);

		window.display();
	}
	return 0;
}

//intializing the shaders
void init()
{
	shader.loadFromFile("shader.vert", "shader.frag");
	shaderOil.loadFromFile("shaderO.vert", "shader.frag");
}

//updating the visualization
void update(float dt)
{
	sph.update(dt, vis);
}

//render the window and the particles
void render(sf::RenderWindow &window, sf::RenderTexture &renderTexture)
{
	sph.render(renderTexture, vis);
	renderTexture.display();
	renderTexture.setSmooth(true);

	sf::Sprite sprite = sf::Sprite(renderTexture.getTexture());
	sprite.setScale(WINDOW_SCALE, WINDOW_SCALE);

	if (vis== Visualizaton::Oil)
	{
		shaderOil.setUniform("texture", sf::Shader::CurrentTexture);
		window.draw(sprite, &shaderOil);

	}
	else
	{
		shader.setUniform("texture", sf::Shader::CurrentTexture);
		
		window.draw(sprite, &shader);
	}
	//shader.setUniform("texture", sf::Shader::CurrentTexture);
	//window.draw(sprite, &shader);

}