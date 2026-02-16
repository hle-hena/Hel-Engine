/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: basic.vert                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:07:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/16 18:21:38                                        */
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

layout (binding = 0) uniform UniformBufferObject {
	mat4	viewProjection;
}	ubo;

layout (push_constant) uniform Push {
	mat4	modelMatrix;
	mat4	normalMatrix;
} push;

void	main() {
	vec4	positionInWorld = push.modelMatrix * vec4(inPos, 1.0);
	gl_Position = ubo.viewProjection * positionInWorld;
	fragColor = inColor;
	fragNormal = normalize(mat3(push.normalMatrix) * inNormal);
}
