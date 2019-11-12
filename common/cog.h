/*
* cog.h
*/

#include "wrapper_glfw.h"
#include <vector>
#include <glm/glm.hpp>

//objects
#include "cylinder.h"
#include "cube.h"

class Cog
{
private:
	Cylinder cog_wheel;
	Cube cogs[5];

	int numvertices;
	int drawmode;
public:
	Cog();
	~Cog();

	void makeCog();
	void drawCog(int drawmode);
};
