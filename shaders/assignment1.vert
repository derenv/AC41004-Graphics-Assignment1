//vertex shader AC41001 Assignment 1
//Deren Vural 2/11/2019

// Specify minimum OpenGL version
#version 440 core

//vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 normal;

//in
//

//out
out vert_data{
	vec4 vert_pos, vert_colour;
	vec3 vert_normal, light_direction;
};

//uniforms
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//uniform mat3 normalmatrix;
//uniform vec4 lightpos;
//uniform vec3 object_colour;
//uniform uint colourmode;

layout(location = 3) uniform model_data{
	mat4 model;
	mat4 view;
	mat4 projection;
	mat3 normalmatrix;
	vec4 lightpos;
	vec3 object_colour;
};

layout(location = 4) uniform model_switches{
	uint colourmode;
};

//globals
//

void main()
{
	//custom colours
	if (colourmode == 1){
		vert_colour = vec4(object_colour,1.0);
		//diffuse_albedo = colour;
	}else{
		vert_colour = vec4(1.0, 0, 0, 1.0);
	}

	//homogeneous coords
	vec4 position_h = vec4(position, 1.0);

	//precalculated lighting vectors
	mat4 mv_matrix = view * model;
	vert_pos = mv_matrix * position_h;
	vert_normal = normalize(normalmatrix * normal);

	//FOR FUTURE USE
	//lighting position(s) (make array if necessary)
//	light_direction = lightpos[0].xyz - vert_pos.xyz;
//	for(int i=1; i<sizeof(lightpos); i++){
//		vec3 cur_light_direction = lightpos[i].xyz - vert_pos.xyz;
//		if(cur_light_direction.x >= light_direction.x && cur_light_direction.y >= light_direction.y && cur_light_direction.z >= light_direction.z){
//			light_direction = cur_light_direction
//		}
//	}
	//calculate single light source direction
	light_direction = lightpos.xyz - vert_pos.xyz;

	gl_Position = (projection * view * model) * position_h;
}