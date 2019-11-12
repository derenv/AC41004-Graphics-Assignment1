/**
 * 
 */

#ifndef BASE_H
#define BASE_H

#include "wrapper_glfw.h"
#include <glm/glm.hpp>

class WindmillBase
{
private:
	glm::vec3 colour;
	GLfloat radius, length;
	GLuint definition;
	GLuint windmillbaseBufferObject, windmillbaseNormals, windmillbaseColours, windmillbaseElementbuffer;
	GLuint num_pvertices;
	GLuint isize;
	GLuint numberOfvertices;

	GLuint attribute_v_coord;
	GLuint attribute_v_normal;
	GLuint attribute_v_colours;

	void defineVertices();

public:
	WindmillBase();
	WindmillBase(glm::vec3 c);
	~WindmillBase();
	void makeWindmillBase();
	void drawWindmillBase(int drawmode);
};

#endif