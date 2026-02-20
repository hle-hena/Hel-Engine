/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: cameraFrustum.vert                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/20 10:44:59 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/20 19:58:15                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

# version 450

layout (location = 0) out vec3	fragColor;

layout (location = 0) in vec3	inPos;
layout (location = 1) in vec3	inNormal;
layout (location = 2) in vec3	inColor;

layout (binding = 0) uniform UniformBufferObject {
	mat4	viewProjection;
	float	elapsedTime;
}	globalUBO;

layout (push_constant) uniform Push {
	mat4	modelMatrix;
	mat4	invViewProjection;
}	push;

void	main() {
	float	visualScale = 0.75;
	vec4	ndcPos;
	if (inPos.z == 1) {
		ndcPos = vec4(inPos * visualScale, 1.);
	} else {
		ndcPos = vec4(inPos.x, inPos.y, inPos.z * visualScale, 1.);
	}
	vec4	positionInWorld = push.invViewProjection * ndcPos;
	gl_Position = globalUBO.viewProjection * positionInWorld;
	fragColor = vec3(1.);
}
