/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: gizmo.frag                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/04/09 19:38:54 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/09 19:58:00                                        */
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

layout (binding = 0) uniform UniformBufferObject {
	mat4	viewProjection;
	float	elapsedTime;
}	ubo;

struct	Tint {
	vec3	tint;
};
layout(set = 1, binding = 1) readonly buffer Tints {
	Tint data[];
} tints;

layout (push_constant) uniform Push {
	uint	entityIndex;
	uint	transformIndex;
	uint	tintIndex;
} push;

void	main() {
	Tint	fragTint = push.tintIndex != 0xFFFFFFFF ? tints.data[push.tintIndex] : Tint(vec3(1.));
	outColor = vec4(inColor * fragTint.tint, 1.);
	outEntityID = push.entityIndex;
}
