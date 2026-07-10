/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: basic.frag                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:07:46 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/09 11:01:48                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout (location = 0) out vec4		outColor;
layout (location = 1) out uint		outEntityID;

layout (location = 0) in vec3		inColor;
layout (location = 1) in vec3		inPos;
layout (location = 2) in vec3		inNormal;
layout (location = 3) in vec2		inUV;

layout (binding = 0) uniform UniformBufferObject {
	mat4	viewProjection;
	float	elapsedTime;
}	ubo;

struct	Transform {
	mat4	modelMatrix;
	mat4	normalMatrix;
};
layout(set = 1, binding = 0) readonly buffer Transforms {
	Transform data[];
} transforms;

layout(set = 2, binding = 0) uniform sampler2D materialTexture;

layout (push_constant) uniform Push {
	uint	entityIndex;
	uint	transformIndex;
} push;

float	squirrelHash(int position, uint noise1, uint noise2, uint noise3) {
	uint mangled = position;
	mangled *= noise1;
	mangled ^= (mangled >> 8);
	mangled += noise2;
	mangled ^= (mangled << 8);
	mangled *= noise3;
	mangled ^= (mangled >> 8);

	return (float(mangled) / float(0xFFFFFFFFu));
}

vec3	hashColor(int primId) {
	return (vec3(
		squirrelHash(primId, 0xB5297A4D, 0x68E31DA4, 0x1B56C4E9),
		squirrelHash(primId, 0x1B56C4E9, 0xB5297A4D, 0x68E31DA4),
		squirrelHash(primId, 0x68E31DA4, 0x1B56C4E9, 0xB5297A4D)
	));
}

struct	Spotlight {
	vec3	pos;
	vec4	dir;	//dir.w beeing the light out angle
	vec4	color;	//color.w beeing the intensity;
};

vec3	getColorFromLight(vec3 surfaceNormal, uint transformIndex, uint spotLightIndex) {
	vec3	lightPos = 
}

vec3	getColorFromSpotlight(vec3 surfaceNormal, Spotlight light, bool lightDebug) {
	vec3	toLight = inPos - light.pos;
	float	dist = length(toLight);
	toLight = normalize(toLight);

	float	edgeSoftness = lightDebug ? 0. : 1.0 - (1. / (dist + 1.));
	float	coneIntensity = smoothstep(light.dir.w, light.dir.w + edgeSoftness, dot(toLight, light.dir.xyz));

	float	falloff = lightDebug ? 1. : (1. / (dist * dist + 1.));
	float	diffuse = max(dot(surfaceNormal, -toLight), 0.);
	return (light.color.xyz * (light.color.w * diffuse * falloff * coneIntensity));
}



void	main() {
	bool	triangleDebug = false;
	bool	normalDebug = false;
	bool	lightDebug = false;
	vec3	surfaceNormal = normalize(inNormal);

	float	ambientLight = 0.01;
	vec3	lightRecieved = vec3(ambientLight) + getThreeLightsColor(surfaceNormal, lightDebug);

	vec3	texColor = texture(materialTexture, inUV).xyz;
	vec3	baseColor = triangleDebug ? hashColor(gl_PrimitiveID) :
					(normalDebug ? vec3(normalize(inNormal) * 0.5 + 0.5) : texColor);
	outColor = lightDebug ? vec4(lightRecieved, 1.) : vec4(baseColor * lightRecieved, 1.);
	outEntityID = push.entityIndex;
}
