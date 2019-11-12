//fragment shader AC41001 Assignment 1
//Deren Vural 2/11/2019
//equation from: 
//method from: 

// Specify minimum OpenGL version
#version 420 core

//in
in vert_data{
	vec4 vert_pos, vert_colour;
	vec3 vert_normal, light_direction1, light_direction2;
};

//out
out vec4 outputColor;

//globals
vec3 specular_albedo = vec3(1.0, 0.8, 0.6);
vec3 global_ambient = vec3(0.05, 0.05, 0.05);
int  shininess = 8;

//cook torrance
const float PI = 3.141592653;
float roughness = 0.5f;

//uniforms
uniform uint emitmode;
uniform float ambient_constant;

float beckmann(float NdotH){
	//microfacet distribution by beckmann
	float r1 = 1.0f / (4.0f * pow(roughness,2) * pow(NdotH,4.0f) * PI);
	float r2 = exp(-pow(tan(acos(NdotH)),2.0) / (pow(roughness,2)));
	//float r2 = exp((NdotH * NdotH - 1.0f) / (pow(roughness,2) * NdotH));
	return r1 * r2;
}

float geometric_shadow(float NdotH, float NdotV, float VdotH, float NdotL){
	//geometric shadowing
	float g1 = (2.0f * NdotH * NdotV) / VdotH;
	float g2 = (2.0f * NdotH * NdotL) / VdotH;
	return min(1.0f, min(g1, g2));
}

vec3 fresnels(float VdotH){
	float F0 = 0.8f;

	//fresnel reflectance
	float F = pow(1.0f - VdotH, 5.0);
	F *= (1.0f - F0);
	F += F0;

	return F * vert_colour.xyz;
}

vec4 CookTorrance(vec3 light_direction){
	/*
	Cook-Torrance Equation

	f(1,v) = D(h)F(v,h)G(l,v,h)
				    /
				4(n.l)(n.v)

	(where alpha = roughness^2)

	D[Bekmann](m) = (alpha^2 / (PI * alpha^2 * (n.m)^4)) * exp[((n.m^2 - 1) / alpha^2 * (n.m)^2)]

	G[cook-torrance](l,v,h) = min[1,(2(n.h)(n.v)/v.h),(2(n.h)(n.l)/v.h)]

	F[cook-torrance](v,h) = (1/2) * ((g-c)/(g+c))^2 * (1+((g+c)*c - 1 / (g-c)c + 1)^2)
	*/
	vec3 specular_light = vec3(0);

	vec3 n = normalize(vert_normal);
	vec3 v = normalize(vert_pos.xyz);
	vec3 l = normalize(light_direction);
	vec3 h = normalize(v + l);

	float NdotH = max(0, dot(n, h));
	float VdotH = max(0, dot(v, h));
	float NdotV = max(0, dot(n, v));
	float NdotL = max(0, dot(n, l));
	if(NdotL > 0 && NdotV > 0){
		specular_light = (beckmann(NdotH) * geometric_shadow(NdotH, NdotV, VdotH, NdotL) * fresnels(VdotH)) / (NdotL * NdotV);
	}
	
	vec3 beta = vec3(1.0,1.0,1.0) / (4.0 * PI * pow(length(light_direction1),2.0));
	float ambient = 0.2;
	vec3 Kd = vert_colour.xyz;
	float s = 0.2;
	vec4 xcol = vec4(beta * NdotL * ((1.0-s)*Kd + s*specular_light) + ambient*Kd, 1.0);

	return xcol;
}

void main()
{
	//diffuse
	
	vec3 diffuse = max(dot(vert_normal, normalize(light_direction1)), 0.0) * vert_colour.xyz;
	
	//ambient
	vec3 ambient = vert_colour.xyz * ambient_constant;

	//specular (metal)
	vec4 specular = CookTorrance(light_direction1) * vec4(specular_albedo,1.0);

	//attenuation
	float attenuation;
	float distanceToLight = length(light_direction1);
	float attenuation_k1 = 0.5;
	float attenuation_k2 = 0.5;
	float attenuation_k3 = 0.5;
	attenuation = 1.0 / (attenuation_k1 + attenuation_k2*distanceToLight + attenuation_k3 * pow(distanceToLight, 2));

	//emissive
	vec3 emissive = vec3(0);
	if (emitmode == 1) emissive = vec3(1.0, 1.0, 0.8); 

	outputColor = vec4(attenuation*(ambient + diffuse + specular.xyz) + emissive + global_ambient, 1.0);
}