/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: basic.vert                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:07:52 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/27 21:11:32                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout (location = 0) out vec3	fragColor;
layout (location = 1) out vec3	fragPos;
layout (location = 2) out vec3	fragNormal;
layout (location = 3) out vec2	fragUV;

layout (location = 0) in vec3	inPos;
layout (location = 1) in vec2	inUV;
layout (location = 2) in vec3	inNormal;
layout (location = 3) in vec3	inColor;

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

layout (push_constant) uniform Push {
	uint	entityIndex;
	uint	transformIndex;
} push;

void	main() {
	Transform	transform = transforms.data[push.transformIndex];
	vec4	positionInWorld = transform.modelMatrix * vec4(inPos, 1.0);
	gl_Position = ubo.viewProjection * positionInWorld;
	fragColor = inColor;
	fragPos = vec3(positionInWorld);
	fragNormal = normalize(mat3(transform.normalMatrix) * inNormal);
	fragUV = inUV;
}
