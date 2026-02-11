/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: basic.vert                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:07:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/11 16:55:57                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout (location = 0) out vec3	fragColor;
layout (location = 1) out vec3	fragNormal;

layout (location = 0) in vec3	inPos;
layout (location = 1) in vec3	inColor;
layout (location = 2) in vec3	inNormal;

layout(push_constant) uniform Push {
	mat4	viewProjection;
	mat4	objectTransform;
} push;

void	main() {
	vec4	positionInWorld = push.objectTransform * vec4(inPos, 1.0);
	gl_Position = push.viewProjection * positionInWorld;
	fragColor = inColor;
	mat4	normalMatrix = transpose(inverse(push.objectTransform));
	fragNormal = normalize(mat3(normalMatrix) * inNormal);
}
