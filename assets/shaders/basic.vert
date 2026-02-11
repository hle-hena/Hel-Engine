/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: basic.vert                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:07:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/11 15:52:34                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout (location = 0) out vec3		fragColor;

layout (location = 0) in vec3	inPos;
layout (location = 1) in vec3	inColor;

layout(push_constant) uniform Push {
	mat4	viewProjection;
	mat4	objectTransform;
} push;

void	main() {
	vec4	positionInWorld = push.objectTransform * vec4(inPos, 1.0);
	gl_Position = push.viewProjection * positionInWorld;
	fragColor = inColor;
}
