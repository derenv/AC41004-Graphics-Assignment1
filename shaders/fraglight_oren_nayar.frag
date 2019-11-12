// Fragment shader that implements Oren Nayar (diffuse)
// per-fragment lighting for multiple light sources

#version 400

// Global constants (for this vertex shader)
vec4 global_ambient = vec4(0.05, 0.05, 0.05, 1.0);

// Inputs from the vertex shader
in vert_data{
	vec4 vert_pos, vert_colour;
	vec3 vert_normal, light_direction1, light_direction2;
};

uniform uint attenuationmode;
uniform uint emitmode;
uniform float ambient_constant;

const float PI = 3.141592653;
const float roughness = 0.5;

// Output pixel fragment colour
out vec4 outputColor;

//
vec4 calc_oren_nayar(vec3 light_direction, vec4 emissive, vec4 ambient, vec3 normal, vec3 view_dir){
	//calculate light info
    vec3 light_dir = normalize(light_direction);
	float distancetolight = length(light_direction);
	
	// Calculate these dot products which are used in the Oren Nayar equations below
    float NdotL = dot(normal, light_dir);
    float NdotV = dot(normal, view_dir); 

    float angleVN = acos(NdotV);
    float angleLN = acos(NdotL);

	// Implement the Oren Nayar equations
    float alpha = max(angleVN, angleLN);
    float beta = min(angleVN, angleLN);
    float gamma = dot(view_dir - normal * dot(view_dir, normal), light_dir - normal * dot(light_dir, normal));

    float roughnessSquared = roughness * roughness;
    float roughnessSquared9 = (roughnessSquared / (roughnessSquared + 0.09));

    // calculate C1, C2 and C3
    float C1 = 1.0 - 0.5 * (roughnessSquared / (roughnessSquared + 0.33));
    float C2 = 0.45 * roughnessSquared9;

    if(gamma >= 0.0) {
        C2 *= sin(alpha);
    } else {
        C2 *= (sin(alpha) - pow((2.0 * beta) / PI, 3.0));
    }

    float powValue = (4.0 * alpha * beta) / (PI * PI);
    float C3  = 0.125 * roughnessSquared9 * powValue * powValue;

    // now calculate both main parts of the formula
    float A = gamma * C2 * tan(beta);
    float B = (1.0 - abs(gamma)) * C3 * tan((alpha + beta) / 2.0);

    // put it all together
    float L1 = max(0.0, NdotL) * (C1 + A + B);

    // also calculate interreflection
    float twoBetaPi = 2.0 * beta / PI;

    float L2 = 0.17 * max(0.0, NdotL) * (roughnessSquared / (roughnessSquared + 0.13)) * (1.0 - gamma * twoBetaPi * twoBetaPi);

    vec4 oren_nayar = vec4(vert_colour.xyz * (L1 + L2), 1.0);
	
	// Calculate the attenuation factor;
	// Turn off attenuation if attenuationmode is not set to 1 (on)
	float attenuation;
	if (attenuationmode != 1)
	{
		attenuation = 1.0;
	}
	else
	{
		// Define attenuation constants. These could be uniforms for greater flexibility
		float attenuation_k1 = 0.5;
		float attenuation_k2 = 0.5;
		float attenuation_k3 = 0.5;
		attenuation = 1.0 / (attenuation_k1 + attenuation_k2*distancetolight + attenuation_k3 * pow(distancetolight, 2));
	}
	

	// Calculate the output colour, includung attenuation on the diffuse and specular components
	// Note that you may want to exclude the ambient from the attenuation factor so objects
	// are always visible, or include a global ambient
	vec4 output_color = attenuation*(ambient + oren_nayar) + emissive + global_ambient;

	return output_color;
}

void main()
{
	vec4 emissive = vec4(0);
	// If emitmode is 1 then we enable emmissive lighting
	if (emitmode == 1) emissive = vec4(1.0, 1.0, 0.8, 1.0);
	vec4 ambient = vert_colour * ambient_constant;
	
	// Normalise our input vectors, these may be the same for other BRDFs
	vec3 normal = normalize(vert_normal);
    vec3 view_dir = normalize(-vert_pos.xyz);

	//calculate colour for each light
	outputColor = calc_oren_nayar(light_direction1, emissive, ambient, normal, view_dir);
	//outputColor = outputColor + calc_oren_nayar(light_direction2, emissive, ambient, normal, view_dir);
	outputColor = outputColor;// + emissive + global_ambient;

}