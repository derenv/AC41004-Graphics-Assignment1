//fragment shader AC41001 Assignment 1
//Deren Vural 2/11/2019

// Specify minimum OpenGL version
#version 420 core

//in
in vert_data{
	vec4 vert_pos, vert_colour;
	vec3 vert_normal, light_direction;
};

//out
out vec4 outputColor;

//globals
vec3 specular_albedo = vec3(1.0, 0.8, 0.6);
vec3 global_ambient = vec3(0.05, 0.05, 0.05);
int  shininess = 8;

//uniforms
uniform uint emitmode;

void main()
{
	//diffuse
	
	vec3 diffuse = max(dot(vert_normal, normalize(light_direction)), 0.0) * vert_colour.xyz;
	
	//ambient
	vec3 ambient = vert_colour.xyz * 0.2;

	//specular
	vec3 V = normalize(-vert_pos.xyz);	
	vec3 R = reflect(-normalize(light_direction), vert_normal);
	vec3 specular = pow(max(dot(R, V), 0.0), shininess) * specular_albedo;
	
	//attenuation
	float attenuation;
	float distanceToLight = length(light_direction);
	float attenuation_k1 = 1.5;
	float attenuation_k2 = 1.5;
	float attenuation_k3 = 1.5;
	attenuation = 1.0 / (attenuation_k1 + attenuation_k2*distanceToLight + attenuation_k3 * pow(distanceToLight, 2));

	//emissive
	vec3 emissive = vec3(0);
	if (emitmode == 1) emissive = vec3(1.0, 1.0, 0.8); 

	outputColor = vec4(attenuation*(ambient + diffuse + specular) + emissive + global_ambient, 1.0);
}