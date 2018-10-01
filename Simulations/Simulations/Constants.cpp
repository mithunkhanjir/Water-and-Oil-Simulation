#include "Constants.h"

//all the required constants
namespace Constants
{
	const float WIDTH= 3;
	const float HEIGHT= 1.5;

	const float SCALE= 400;

	const int RENDER_WIDTH = SCALE * WIDTH;
	const int RENDER_HEIGHT = SCALE * HEIGHT;

	const float WINDOW_SCALE = 1.0f;
	const float TIMESTEP = 0.0001f;

	const int NUMBER_PARTICLES = 70;
	const float REST_DENSITY_WATER = 1000;
	const float REST_DENSITY_OIL = 910;

	const float STIFFNESS = 10000;
	const float VISCOSITY = 3000;
	const float VISCOSITY_OIL =19500;
	const float TENSION =10000.0f;

	const float GRAVITY = 12000;

	const float PARTICLE_SPACING = 1.0/NUMBER_PARTICLES;
	const float PARTICLE_VOLUME = PARTICLE_SPACING * PARTICLE_SPACING;
	const float PARTICLE_MASS_WATER= PARTICLE_VOLUME * REST_DENSITY_WATER;
	const float PARTICLE_MASS_OIL = PARTICLE_VOLUME * REST_DENSITY_OIL;
	const float KERNEL_RANGE = 2 * PARTICLE_SPACING;
}