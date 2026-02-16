/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: basic.frag                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:07:46 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/16 22:16:08                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout (location = 0) in vec3		inColor;
layout (location = 1) in vec3		inPos;
layout (location = 2) in vec3		inNormal;

layout (location = 0) out vec4		outColor;

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

const vec3	spotlightPos = vec3(0., 10., 10.);
const vec3	spotlightDir = normalize(vec3(-0., -1., -1.));
const vec3	spotLightColor = vec3(1., 1., 1.);
const float	spotLightIntensity = 1.;
const float	spotLightAngle = 0.8f;

const vec3	godRayDirection = normalize(vec3(10., 10., 1.));
const float	ambientLight = 0.1f;

void	main() {
	bool	triangleDebug = false;
	bool	normalDebug = false;
	bool	lightDebug = false;
	vec3	surfaceNormal = normalize(inNormal);

	vec3	toLight = inPos - spotlightPos;
	float	distSquared = dot(toLight, toLight);
	toLight = normalize(toLight);
	float	edgeSoftness = distSquared / 10000.;
	float	coneIntensity = smoothstep(spotLightAngle, spotLightAngle + edgeSoftness, dot(toLight, spotlightDir));
	float	diffuse = max(dot(surfaceNormal, -toLight), 0.);

	vec3	intensity = vec3((diffuse * coneIntensity) + ambientLight);

	vec3 baseColor = triangleDebug ? hashColor(gl_PrimitiveID) :
					(normalDebug ? vec3(normalize(inNormal) * 0.5 + 0.5) : inColor);
	outColor = vec4(baseColor * intensity, 1.);
}
