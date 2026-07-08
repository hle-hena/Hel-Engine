/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EntityIdAsColor.frag                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/04/01 18:00:39 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/01 18:05:22                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout (location = 0) out uint		outEntityId;

layout (location = 0) in vec3		inColor;
layout (location = 1) in vec3		inPos;
layout (location = 2) in vec3		inNormal;

layout (binding = 0) uniform UniformBufferObject {
	mat4	viewProjection;
	float	elapsedTime;
}	ubo;

layout (push_constant) uniform Push {
	uint	entityIndex;
	uint	transformIndex;
} push;

void	main(void) {
	outEntityId = push.entityIndex;
}
