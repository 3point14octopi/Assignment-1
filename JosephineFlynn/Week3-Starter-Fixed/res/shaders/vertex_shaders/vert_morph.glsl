#version 440

#include "../fragments/vs_common.glsl"

layout(location = 6) in vec3 inPos2;
layout(location = 7) in vec3 inNorm2;

uniform float t_val;
uniform bool MorphOn;
uniform bool aOn;
uniform bool dOn;
uniform bool sOn;

layout (location = 10) out vec3 boolVals;


void main(){
	vec3 PosVec;
	vec3 NorVec;
	
	//LERP the positions and normals if effect is toggled on
	if(MorphOn) {
		PosVec = vec3(mix(inPosition, inPos2, t_val));
		NorVec = vec3(mix(inNormal, inNorm2, t_val));
	}else{
		PosVec = vec3(inPosition);
		NorVec = vec3(inNormal);
	}
	//set position
	gl_Position = u_ModelViewProjection * vec4(PosVec, 1.0);

	// Pass vertex pos in world space to frag shader
	outWorldPos = (u_Model * vec4(PosVec, 1.0)).xyz;

	// Normals
	outNormal = mat3(u_NormalMatrix) * NorVec;

    // We use a TBN matrix for tangent space normal mapping
    vec3 T = normalize(vec3(mat3(u_NormalMatrix) * inTangent));
    vec3 B = normalize(vec3(mat3(u_NormalMatrix) * inBiTangent));
    vec3 N = normalize(vec3(mat3(u_NormalMatrix) * inNormal));
    mat3 TBN = mat3(T, B, N);

    // We can pass the TBN matrix to the fragment shader to save computation
    outTBN = TBN;

	// Pass our UV coords to the fragment shader
	outUV = inUV;

	//Pass colour to the fragment shader
	outColor = inColor;

	float a_val = (aOn)?1.0:0.0;
	float d_val = (dOn)?1.0:0.0;
	float s_val = (sOn)?1.0:0.0;

	boolVals = vec3(aOn, dOn, sOn);
}