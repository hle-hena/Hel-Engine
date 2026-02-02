/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: basic.vert                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:07:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/02 13:01:27                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout(location = 0) out vec3 fragColor;

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec3 inColor;

layout(push_constant) uniform Push {
	mat4 viewProjection;
} push;

void	main() {
	gl_Position = push.viewProjection * vec4(inPos, 0.0, 1.0);
	fragColor = inColor;
}
