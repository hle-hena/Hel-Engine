/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: basic.frag                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:07:46 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/17 19:39:52                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout (location = 0) out vec4		outColor;

layout (location = 0) in vec3		inColor;
layout (location = 1) in vec3		inPos;
layout (location = 2) in vec3		inNormal;

layout (binding = 0) uniform UniformBufferObject {
	mat4	viewProjection;
	float	elapsedTime;
}	ubo;

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

vec3	getColorFromSpotlight(vec3 surfaceNormal, Spotlight light) {
	vec3	toLight = inPos - light.pos;
	float	dist = length(toLight);
	toLight = normalize(toLight);

	float	edgeSoftness = 1.0 - (1. / (dist + 1.));
	float	coneIntensity = smoothstep(light.dir.w, light.dir.w + edgeSoftness, dot(toLight, light.dir.xyz));

	float	diffuse = max(dot(surfaceNormal, -toLight), 0.);
	return (light.color.xyz * (light.color.w * diffuse * (1. / (dist * dist + 1.)) * coneIntensity));
}

vec3	getThreeLightsColor(vec3 surfaceNormal) {
	float	lightOffset = 2.0943951023931953;
	Spotlight	redSpotlight = Spotlight(
		vec3(cos(ubo.elapsedTime * 0.5) * 3, 10., sin(ubo.elapsedTime * 0.5) * 3),
		vec4(normalize(vec3(0., -1., 0.)), 0.5 + cos(ubo.elapsedTime * 3) * 0.1),
		vec4(1., 0., 0., (sin(ubo.elapsedTime) * 0.25 + 0.75) * 100.)
	);
	Spotlight	greenSpotlight = Spotlight(
		vec3(cos(ubo.elapsedTime * 0.5 + lightOffset) * 3, 10., sin(ubo.elapsedTime * 0.5 + lightOffset) * 3),
		vec4(normalize(vec3(0., -1., 0.)), 0.5 + cos(ubo.elapsedTime * 3) * 0.1),
		vec4(0., 1., 0., (sin(ubo.elapsedTime) * 0.25 + 0.75) * 100.)
	);
	Spotlight	blueSpotlight = Spotlight(
		vec3(cos(ubo.elapsedTime * 0.5 + 2 * lightOffset) * 3, 10., sin(ubo.elapsedTime * 0.5 + 2 * lightOffset) * 3),
		vec4(normalize(vec3(0., -1., 0.)), 0.5 + cos(ubo.elapsedTime * 3) * 0.1),
		vec4(0., 0., 1., (sin(ubo.elapsedTime) * 0.25 + 0.75) * 100.)
	);

	return (getColorFromSpotlight(surfaceNormal, redSpotlight) +
		getColorFromSpotlight(surfaceNormal, greenSpotlight) +
		getColorFromSpotlight(surfaceNormal, blueSpotlight));
}

vec3	getOneAlternatingLight(vec3 surfaceNormal) {
	float	interval = 2.0943951023931953;
	Spotlight	light = Spotlight(
		vec3(10., 20., 10.),
		vec4(normalize(vec3(-2., -2, -2.)), 0.8),
		vec4(cos(ubo.elapsedTime),
			cos(ubo.elapsedTime + interval),
			cos(ubo.elapsedTime + 2 * interval),
			1000.)
	);

	return (getColorFromSpotlight(surfaceNormal, light));
}

void	main() {
	bool	triangleDebug = false;
	bool	normalDebug = false;
	bool	lightDebug = false;
	vec3	surfaceNormal = normalize(inNormal);

	float	ambientLight = 0.01;
	vec3	lightRecieved = vec3(ambientLight) + getThreeLightsColor(surfaceNormal);

	vec3 baseColor = triangleDebug ? hashColor(gl_PrimitiveID) :
					(normalDebug ? vec3(normalize(inNormal) * 0.5 + 0.5) : inColor);
	outColor = vec4(baseColor * lightRecieved, 1.);
}
