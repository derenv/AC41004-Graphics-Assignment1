/*
*/

#include "cog.h"
#include "cylinder.h"

using namespace std;

Cog::Cog() {
	this->drawmode = drawmode;
}

Cog::~Cog() {}

void Cog::makeCog() {
	this->drawmode = drawmode;
}

void Cog::drawCog(int drawmode) {
	cog_wheel.drawCylinder(drawmode);
	for (int i = 0; i < 5;i++) {
		cogs[i].drawCube(drawmode);
	}
}