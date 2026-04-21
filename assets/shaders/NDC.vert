/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: NDC.vert                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/04/21 14:56:31 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/21 21:06:51                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout (location = 0) out vec2	fragUV;
layout (location = 1) out vec3	fragColor;

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
	Transform	transform = transforms.data[push.transformIndex];
	Tint	tint = push.tintIndex != 0xFFFFFFFF ? tints.data[push.tintIndex] : Tint(vec3(1.));
	vec2	displacement = vec2(transform.modelMatrix[3][0], transform.modelMatrix[3][1]);
	vec2	scale = vec2(transform.modelMatrix[0][0], transform.modelMatrix[1][1]);
	gl_Position = vec4((inPos.xy * scale) + displacement, 0, 1.);
	fragColor = inColor * tint.tint;
	fragUV = inUV;
}
