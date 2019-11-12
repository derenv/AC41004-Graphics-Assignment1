/*
project desc
*/

#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#pragma comment(lib, "glloadD.lib")
#else
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glload.lib")
#endif
#pragma comment(lib, "opengl32.lib")

//glfw
#include "wrapper_glfw.h"
#include <iostream>
#include <stack>

//glm
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

//objects
#include "sphere.h"
#include "cylinder.h"
#include "tetrahedron.h"
#include "triangle.h"

//data & buffers
GLuint elementbuffer;
GLuint vao;
GLuint program;

GLfloat aspect_ratio;

//variables
//switches
GLuint drawmode;
//light
GLfloat light_x, light_y, light_z;
//sphere
GLuint numlats, numlongs;
//scene rotations
GLfloat angle_x, angle_inc_x;
GLfloat angle_y, angle_inc_y;
GLfloat angle_z, angle_inc_z;
//arm rotations
GLfloat arm_speed, arm_speed_inc;
//view rotations/movement
GLfloat x,y,z;
GLfloat view_move_x, view_move_y, view_move_z;

//uniforms
GLuint modelID, viewID, projectionID, lightposID, normalmatrixID, object_colourID;//data
GLuint emitmodeID, colourmodeID;//switches
GLuint emitmode, colourmode;//switches

//objects
Sphere aSphere;
Cylinder aCylinder;
Tetrahedron aTetrahedron;
Triangle aTriangle;

using namespace std;
using namespace glm;

//animation
bool camera_anim_start;
float speed, anim_angle;
vec3 rotation_axis;
float animation_direction;

//colours
const vec3 colours[10] = {
	vec3(1, 0, 0), //red
	vec3(0, 1, 0), //green
	vec3(0, 0, 1), //blue
	vec3(1, 0, 1), //purple
	vec3(1, 1, 0), //?
	vec3(0, 1, 1), //?
	vec3(1, 1, 1), //white
	vec3(0, 0, 0), //black
	vec3(.59, .29, 0), //brown
	vec3(.2, .2, .2), //grey
};

//animations
//move camera round in a circle
mat4 camera_anim(mat4 view) {
	if ((glfwGetTime() >= 7 || anim_angle == 360) && camera_anim_start){
		//(end anim)
		cout << glfwGetTime() << endl;
		camera_anim_start = false;
		anim_angle = 0.f;
		cout << "end anim" << endl;
	} else if (glfwGetTime() >= 0 && camera_anim_start) {
		//animation part 2
		cout << glfwGetTime() << endl;
		anim_angle += speed;
		view = rotate(view, radians(anim_angle + speed), rotation_axis);//clockwise around x axis
	}
	return view;
}
//move cube (3 around )1

void show_controls() {
	cout << "\ncontrols:" << endl;
	cout << "esc : end program" << endl;
	cout << "- : change draw mode" << endl;
	cout << "= : change colour mode" << endl;
	cout << "LIGHT:" << endl;
	cout << "1 : x -= 0.05f" << endl;
	cout << "2 : x += 0.05f" << endl;
	cout << "3 : y -= 0.05f" << endl;
	cout << "4 : y += 0.05f" << endl;
	cout << "5 : z -= 0.05f" << endl;
	cout << "6 : z += 0.05f" << endl;
	cout << "SCENE ROTATION:" << endl;
	cout << "Q : x -= 0.05f" << endl;
	cout << "W : x += 0.05f" << endl;
	cout << "E : y -= 0.05f" << endl;
	cout << "R : y += 0.05f" << endl;
	cout << "T : z -= 0.05f" << endl;
	cout << "Y : z += 0.05f" << endl;
	cout << "VIEW:" << endl;
	cout << "Z : x -= 0.05f" << endl;
	cout << "X : x += 0.05f" << endl;
	cout << "C : y -= 0.05f" << endl;
	cout << "V : y += 0.05f" << endl;
	cout << "B : z -= 0.05f" << endl;
	cout << "N : z += 0.05f" << endl;
	cout << ", : reset position" << endl;
	cout << "VIEW AIM:" << endl;
	cout << "NUMPAD4 : x -= 0.05f" << endl;
	cout << "NUMPAD6 : x += 0.05f" << endl;
	cout << "NUMPAD8 : y -= 0.05f" << endl;
	cout << "NUMPAD2 : y += 0.05f" << endl;
	cout << "NUMPAD0 : z -= 0.05f" << endl;
	cout << "NUMPAD5 : z += 0.05f" << endl;
	cout << ". : reset aim" << endl;
	cout << "MOVE VIEW:" << endl;
	cout << "NUMPAD7 : add animation on x axis" << endl;
	cout << "NUMPAD9 : add animation on y axis" << endl;
	cout << "NUMPAD3 : add animation on z axis" << endl;
	cout << "NUMPAD1 : do animation" << endl;
	cout << "[ : reset animation axis" << endl;
	cout << "] : next animation axis clockwise/anticlockwise" << endl;
	cout << "ANIMATIONS:" << endl;
	cout << "U : spin windmill anticlockwise" << endl;
	cout << "I : spin windmill clockwise" << endl;
}

void init(GLWrapper* glw)
{
	show_controls();

	//user defined variables
	light_x = light_y = light_z = 0;//light
	numlats = numlongs = 60;//sphere
	emitmode = 0;//lightsource
	colourmode = 0;//object colours or 1 colour
	aspect_ratio = 1.3333f;
	angle_x = angle_y = angle_z = 0;//rotation
	angle_inc_x = angle_inc_y = angle_inc_z = 0;//rotation
	arm_speed = arm_speed_inc = 0;//animation
	x = y = z = 0.f;
	view_move_x = view_move_y = 0; view_move_z = 4;
	speed = 1.f, anim_angle = 0.f;
	camera_anim_start = false;
	rotation_axis = vec3(0,0,0);
	animation_direction = 1;

	// Generate index (name) for one vertex array object
	glGenVertexArrays(1, &vao);

	// Create the vertex array object and make it current
	glBindVertexArray(vao);

	/* Load and build the vertex and fragment shaders */
	try
	{
		program = glw->LoadShader("..\\..\\shaders\\assignment1.vert", "..\\..\\shaders\\assignment1.frag");
		//program[1] = glw->LoadShader("..\\..\\shaders\\basic.vert", "..\\..\\shaders\\basic.frag");
	}
	catch (exception &e)
	{
		cout << "Caught exception: " << e.what() << endl;
		cin.ignore();
		exit(0);
	}

	//vertex shader uniform data
	modelID = glGetUniformLocation(program, "model");
	viewID = glGetUniformLocation(program, "view");
	projectionID = glGetUniformLocation(program, "projection");
	normalmatrixID = glGetUniformLocation(program, "normalmatrix");
	lightposID = glGetUniformLocation(program, "lightpos");
	object_colourID = glGetUniformLocation(program, "object_colour");
	//vertex shader uniform switches
	colourmodeID = glGetUniformLocation(program, "colourmode");
	//fragment shader uniform switches
	emitmodeID = glGetUniformLocation(program, "emitmode");


	//create objects
	aSphere.makeSphere(numlats, numlongs);
	aCylinder.makeCylinder();
	aTetrahedron.defineTetrahedron();
	aTriangle.makeTriangle();

	//error check
	GLint x1;
	GLint* px = &x1;
	glGetProgramiv(program, GL_ATTACHED_SHADERS, px);//change GL_ATTACHED_SHADERS to some program property from https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetProgram.xhtml
	cout << "attached shaders: " << x1 << endl;
}

void display()
{
	/* Define the background colour */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/* Clear the colour and frame buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Enable depth test  */
	glEnable(GL_DEPTH_TEST);

	/* Make the compiled shader program current */
	glUseProgram(program);

	// Define our model transformation in a stack and 
	// push the identity matrix onto the stack
	stack<mat4> model;
	model.push(mat4(1.0f));

	// Define the normal matrix
	mat3 normalmatrix;
	
	// Projection matrix : 30° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	mat4 projection = perspective(radians(30.0f), aspect_ratio, 0.1f, 100.0f);

	// Camera matrix
	mat4 view = lookAt(
		vec3(view_move_x, view_move_y, view_move_z), // Camera is at (0,0,4), in World Space
		vec3(x, y, z), // and looks at the origin
		vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	//view rotations/transforms
	view = rotate(view, -radians(view_move_x), vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	view = rotate(view, -radians(view_move_y), vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	view = rotate(view, -radians(view_move_z), vec3(0, 0, 1));//rotating in clockwise direction around z-axis

	view = camera_anim(view);

	// Define the light position and transform by the view matrix
	vec4 lightpos = view * vec4(light_x, light_y, light_z, 1.0);

	//uniforms
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection[0][0]);
	glUniform4fv(lightposID, 1, value_ptr(lightpos));
	glUniform1ui(colourmodeID, colourmode);

	//lightsource
	model.push(model.top());
	{
		//transformations
		model.top() = translate(model.top(), vec3(light_x, light_y, light_z));
		model.top() = scale(model.top(), vec3(0.05f, 0.05f, 0.05f)); // make a small sphere

		//uniforms
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);
		
		//draw
		emitmode = 1;
		glUniform1ui(emitmodeID, emitmode);
		//draw obj
		aSphere.drawSphere(drawmode);
		emitmode = 0;
		glUniform1ui(emitmodeID, emitmode);
	}
	model.pop();

	//global transformations
	//model.top() = scale(model.top(), vec3(model_scale, model_scale, model_scale));//scale equally in all axis
	model.top() = rotate(model.top(), -radians(angle_x), glm::vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	model.top() = rotate(model.top(), -radians(angle_y), glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	model.top() = rotate(model.top(), -radians(angle_z), glm::vec3(0, 0, 1)); //rotating in clockwise direction around z-axis

	
	//VIEW POINTER
	glUniform3fv(object_colourID, 1, value_ptr(colours[1]));//green
	model.push(model.top());
	{
		//transformations
		model.top() = translate(model.top(), vec3(x, y, z));
		model.top() = scale(model.top(), vec3(1.f / 3.f, 1.f / 3.f, 1.f / 3.f));//scale equally in all axis

		//uniforms
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		//draw
		emitmode = 1;
		glUniform1ui(emitmodeID, emitmode);
		aTetrahedron.drawTetrahedron(drawmode); // Draw our tetra
		emitmode = 0;
		glUniform1ui(emitmodeID, emitmode);
	}
	model.pop();
	//WINDMILL BASE
	model.top() = translate(model.top(), vec3(.5, 0, 0));
	glUniform3fv(object_colourID, 1, value_ptr(colours[8]));//brown
	//sphere
	model.push(model.top());
	{
		//transformations
		model.top() = translate(model.top(), vec3(0, 0.2, 0));
		model.top() = scale(model.top(), vec3(1.f / 3.f, 1.f / 3.f, 1.f / 3.f));//scale equally in all axis

		//uniforms
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		//draw
		aSphere.drawSphere(drawmode); // Draw our sphere
	}
	model.pop();
	//windmilltower
	model.push(model.top());
	{
		//transformations
		model.top() = translate(model.top(), vec3(0, -.3, 0));
		model.top() = scale(model.top(), vec3(1.f / 3.f, 1.f, 1.f / 3.f));//scale equally in all axis

		//uniforms
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		//draw
		aCylinder.drawCylinder(drawmode); // Draw our cylinder
	}
	model.pop();
	glUniform3fv(object_colourID, 1, value_ptr(colours[9]));//grey
	//windmillbase
	model.push(model.top());
	{
		//transformations
		model.top() = translate(model.top(), vec3(0, -.75, 0));
		model.top() = scale(model.top(), vec3(1.f / 2.75f, 1.f / 3.f, 1.f / 2.75f));//scale equally in all axis

		//uniforms
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		//draw
		aCylinder.drawCylinder(drawmode); // Draw our windmill base
	}
	model.pop();

	//WINDMILL ARMS
	model.top() = translate(model.top(), vec3(0, .28, .35));
	glUniform3fv(object_colourID, 1, value_ptr(colours[8]));//brown
	//armsbase
	model.push(model.top());
	{
		//transformations
		model.top() = rotate(model.top(), -radians(90.0f), glm::vec3(1, 0, 0)); //rotating in clockwise direction around z-axis
		model.top() = scale(model.top(), vec3(.25f / 3.f, .25f / 3.f, .25f / 3.f));//scale equally in all axis

		//uniforms
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		//draw
		aCylinder.drawCylinder(drawmode); // Draw our windmill base
	}
	model.pop();
	//arm1
	model.push(model.top());
	{
		//transformations
		model.top() = rotate(model.top(), -radians(arm_speed), glm::vec3(0, 0, 1)); //rotating in clockwise direction around z-axis
		model.top() = scale(model.top(), vec3(.1f / 3.f, 1.f, .1f / 3.f));//scale equally in all axis

		//uniforms
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		//draw
		aCylinder.drawCylinder(drawmode); // Draw our windmill base
	}
	model.pop();
	//arm2
	model.push(model.top());
	{
		//transformations
		model.top() = rotate(model.top(), -radians(arm_speed - 90.f), glm::vec3(0, 0, 1)); //rotating in clockwise direction around z-axis
		model.top() = scale(model.top(), vec3(.1f / 3.f, 1.f, .1f / 3.f));//scale equally in all axis

		//uniforms
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		//draw
		aCylinder.drawCylinder(drawmode); // Draw our windmill base
	}
	model.pop();
	//SAILS
	glUniform3fv(object_colourID, 1, value_ptr(colours[7]));//white
	for (int i = 0; i < 4; i++) {
		//sails
		model.push(model.top());
		{
			//transformations
			//model.top() = translate(model.top(), vec3(0, 0, 0));
			model.top() = rotate(model.top(), radians(i * 90.f), glm::vec3(0, 0, 1)); //rotating in clockwise direction around x-axis
			model.top() = rotate(model.top(), -radians(arm_speed), glm::vec3(0, 0, 1)); //rotating in clockwise direction around z-axis
			model.top() = scale(model.top(), vec3(1.f / 2.f, 1.f / 2.f, 1.f / 3.f));//scale equally in all axis

			//uniforms
			glUniformMatrix4fv(modelID, 1, GL_FALSE, &(model.top()[0][0]));
			normalmatrix = transpose(inverse(mat3(view * model.top())));
			glUniformMatrix3fv(normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

			//draw
			aTriangle.drawTriangle(drawmode); // Draw our windmill base
		}
		model.pop();
	}
	
	//modify animation variables
	angle_x += angle_inc_x;
	angle_y += angle_inc_y;
	angle_z += angle_inc_z;

	arm_speed += arm_speed_inc;

	//check for error
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		if (err == GL_INVALID_VALUE) {
			cerr << "OpenGL error: " << err << " GL_INVALID_VALUE" << endl;
		} else if (err == GL_INVALID_OPERATION) {
			cerr << "OpenGL error: " << err << " GL_INVALID_OPERATION" << endl;
		} else {
			cerr << "OpenGL error: " << err << endl;
		}
	}
}

static void reshape(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	aspect_ratio = ((float)w / 640.f * 4.f) / ((float)h / 480.f * 3.f);
}

static void keyCallback(GLFWwindow* window, int key, int s, int action, int mods)
{
	/* Enable this call if you want to disable key responses to a held down key*/
	//if (action != GLFW_PRESS) return;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	/* Cycle between drawing vertices, mesh and filled polygons */
	if (key == '-' && action != GLFW_PRESS)
	{
		drawmode++;
		if (drawmode > 2) drawmode = 0;
	}

	if (key == '=' && action != GLFW_PRESS)
	{
		colourmode = !colourmode;
		cout << "colourmode=" << colourmode << endl;
	}

	//light controls
	if (key == GLFW_KEY_1) light_x -= 0.05f;
	if (key == GLFW_KEY_2) light_x += 0.05f;
	if (key == GLFW_KEY_3) light_y -= 0.05f;
	if (key == GLFW_KEY_4) light_y += 0.05f;
	if (key == GLFW_KEY_5) light_z -= 0.05f;
	if (key == GLFW_KEY_6) light_z += 0.05f;

	//scene rotations
	if (key == 'Q') angle_inc_x -= 0.05f;
	if (key == 'W') angle_inc_x += 0.05f;
	if (key == 'E') angle_inc_y -= 0.05f;
	if (key == 'R') angle_inc_y += 0.05f;
	if (key == 'T') angle_inc_z -= 0.05f;
	if (key == 'Y') angle_inc_z += 0.05f;

	//windmill arm controls
	if (key == 'U') arm_speed_inc -= 0.05f;
	if (key == 'I') arm_speed_inc += 0.05f;

	//view controls
	//looking at
	if (key == GLFW_KEY_KP_4) { x -= 0.05f; cout << "x" << x << endl; }
	if (key == GLFW_KEY_KP_6) { x += 0.05f; cout << "x" << x << endl; }
	if (key == GLFW_KEY_KP_8) { y += 0.05f; cout << "y" << y << endl; }
	if (key == GLFW_KEY_KP_2) { y -= 0.05f; cout << "y" << y << endl; }
	if (key == GLFW_KEY_KP_0) { z += 0.05f; cout << "z" << z << endl; }
	if (key == GLFW_KEY_KP_5) { z -= 0.05f; cout << "z" << z << endl; }
	//position
	if (key == 'Z') {
		view_move_x += 0.05f;
		cout << "x:" << view_move_x << " y:" << view_move_y << " z:" << view_move_z << endl;
	}
	if (key == 'X') {
		view_move_x -= 0.05f;
		cout << "x:" << view_move_x << " y:" << view_move_y << " z:" << view_move_z << endl;
	}
	if (key == 'C') {
		view_move_y += 0.05f;
		cout << "x:" << view_move_x << " y:" << view_move_y << " z:" << view_move_z << endl;
	}
	if (key == 'V') {
		view_move_y -= 0.05f;
		cout << "x:" << view_move_x << " y:" << view_move_y << " z:" << view_move_z << endl;
	}
	if (key == 'B') {
		view_move_z += 0.05f;
		cout << "x:" << view_move_x << " y:" << view_move_y << " z:" << view_move_z << endl;
	}
	if (key == 'N') {
		view_move_z -= 0.05f;
		cout << "x:" << view_move_x << " y:" << view_move_y << " z:" << view_move_z << endl;
	}

	//reset position
	if (key == ',' && action != GLFW_PRESS) {
		view_move_x = 0.f;
		view_move_y = 0.f;
		view_move_z = 4.f;
		cout << "x:" << view_move_x << " y:" << view_move_y << " z:" << view_move_z << endl;
	}
	//reset looking at
	if (key == '.' && action != GLFW_PRESS) {
		x = 0.f;
		y = 0.f;
		z = 0.f;
		cout << "x:" << x << " y:" << y << " z:" << z << endl;
	}

	//flip view
	if (key == GLFW_KEY_KP_7 && action != GLFW_PRESS) {
		rotation_axis = clamp((animation_direction * rotation_axis) + vec3(1,0,0), -1.0f, 1.0f);
		cout << "x:" << rotation_axis.x << " y:" << rotation_axis.y << " z:" << rotation_axis.z << endl;
	}
	if (key == GLFW_KEY_KP_9 && action != GLFW_PRESS) {
		rotation_axis = clamp((animation_direction * rotation_axis) + vec3(0, 1, 0), -1.0f, 1.0f);
		cout << "x:" << rotation_axis.x << " y:" << rotation_axis.y << " z:" << rotation_axis.z << endl;
	}
	if (key == GLFW_KEY_KP_3 && action != GLFW_PRESS) {
		rotation_axis = clamp((animation_direction * rotation_axis) + vec3(0, 0, 1), -1.0f, 1.0f);
		cout << "x:" << rotation_axis.x << " y:" << rotation_axis.y << " z:" << rotation_axis.z << endl;
	}
	if (key == GLFW_KEY_KP_1 && action != GLFW_PRESS) {
		if (rotation_axis == vec3(0, 0, 0)) {
			cout << "please pick spin axis:" << endl;
			cout << "x:" << rotation_axis.x << " y:" << rotation_axis.y << " z:" << rotation_axis.z << endl;
		} else if (camera_anim_start) {
			//reset anim
			glfwSetTime(0.0);
			anim_angle = 0;
		} else {
			//start timer
			glfwSetTime(0.0);
			camera_anim_start = true;
		}
	}
	if (key == '[' && action != GLFW_PRESS) {
		rotation_axis = vec3(0, 0, 0);
		camera_anim_start = false;
		anim_angle = 0;
		cout << "spin axis reset:" << endl;
		cout << "x:" << rotation_axis.x << " y:" << rotation_axis.y << " z:" << rotation_axis.z << endl;
	}
	if (key == ']' && action != GLFW_PRESS) {
		animation_direction = -animation_direction;
		if (animation_direction == 1) {
			cout << "clockwise" << endl;
		} else {
			cout << "anti-clockwise" << endl;
		}
	}
	if (key == 'L' && action != GLFW_PRESS) {
		GLint num_uniforms;
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &num_uniforms);
		GLchar uniform_name[256];
		GLsizei length;
		GLint size;
		GLenum type;
		cout << "===active uniforms : " << num_uniforms << "===" << endl;
		for (int i = 0; i < num_uniforms; i++)
		{
			glGetActiveUniform(program, i, sizeof(uniform_name), &length, &size, &type, uniform_name);
			cout << "=" << uniform_name << "=" << length << "=" << size << "=" << type << "=" << endl;
		}
	}
}

/* Entry point of program */
int main(int argc, char* argv[])
{
	GLWrapper* glw = new GLWrapper(1024, 768, "Assignment 1 Deren Vural");;

	if (!ogl_LoadFunctions())
	{
		fprintf(stderr, "ogl_LoadFunctions() failed. Exiting\n");
		return 0;
	}

	glw->setRenderer(display);
	glw->setKeyCallback(keyCallback);
	glw->setReshapeCallback(reshape);

	/* Output the OpenGL vendor and version */
	glw->DisplayVersion();

	init(glw);

	glw->eventLoop();

	delete(glw);
	return 0;
}