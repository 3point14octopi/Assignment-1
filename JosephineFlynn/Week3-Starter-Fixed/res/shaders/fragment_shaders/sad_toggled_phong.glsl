#version 440
layout(location = 10) in vec3 boolVals;

layout(location = 0) out vec4 frag_color;


struct Material {
	sampler2D Diffuse;
	float     Shininess;
};
// Create a uniform for the material
uniform Material u_Material;
#include "../fragments/fs_common_inputs.glsl"
#include "../fragments/multiple_point_lights.glsl"
#include "../fragments/frame_uniforms.glsl"

#include "../fragments/color_correction.glsl"


void main() {
	vec3 normal = normalize(inNormal);

	// Use the lighting calculation that we included from our partial file
	vec3 lightAccumulation = vec3(0.0, 0.0, 0.0);

	//ambient
	float ambientStrength = 0.1;
	vec3 custom_ambient = ambientStrength * Lights[0].ColorAttenuation.xyz;

	//diffuse
	vec3 lightDirection = normalize(Lights[0].Position.xyz - inWorldPos);
	float dif = max(dot(normal, lightDirection), 0.0);
	vec3 custom_diffuse = dif * Lights[0].ColorAttenuation.xyz;

	//specular
	float specStrength = 0.5;
	vec3 viewDirection = normalize(u_CamPos.xyz - inWorldPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specVal = pow(max(dot(viewDirection, reflectionDirection), 0.0), 32);
	vec3 custom_specular = specStrength * specVal * Lights[0].ColorAttenuation.xyz;



	lightAccumulation = (custom_ambient + custom_diffuse + custom_specular);

	//lightAccumulation = (custom_ambient + custom_diffuse + custom_specular);

	// Get the albedo from the diffuse / albedo map
	vec4 textureColor = texture(u_Material.Diffuse, inUV);

	// combine for the final result
	vec3 result = lightAccumulation  * inColor * textureColor.rgb;

	frag_color = vec4(ColorCorrect(result), textureColor.a);
}