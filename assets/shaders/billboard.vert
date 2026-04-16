/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: billboard.vert                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/24 17:02:11 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/16 17:55:17                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout (location = 0) out vec2	outUV;

layout (binding = 0) uniform UniformBufferObject {
	mat4	viewProjection;
	float	elapsedTime;
}	ubo;

struct	Transform {
	mat4	modelMatrix;
	mat4	normalMatrix;
};
layout(set = 1, binding = 0) readonly buffer Transforms {
	Transform	data[];
}	transforms;

layout (push_constant) uniform Push {
	uint	entityIndex;
	uint	transformIndex;
	float	size;
}	push;

void	main() {
	vec2	offsets[4] = vec2[](
		vec2(-0.5,  0.5),
		vec2(-0.5, -0.5),
		vec2( 0.5,  0.5),
		vec2( 0.5, -0.5)
	);
	vec2	uvs[4] = vec2[](
		vec2(0.0, 0.0),
		vec2(0.0, 1.0),
		vec2(1.0, 0.0),
		vec2(1.0, 1.0)
	);

	mat4	mat = ubo.viewProjection;
	vec3	right = normalize(vec3(mat[0][0], mat[1][0], mat[2][0]));
	vec3	up = normalize(vec3(mat[0][1], mat[1][1], mat[2][1]));
	vec2	offset = offsets[gl_VertexIndex] * push.size;
	vec3	pointPos = transforms.data[push.transformIndex].modelMatrix[3].xyz;
	vec3	worldPos = pointPos + right * offset.x + up * offset.y;

	gl_Position = ubo.viewProjection * vec4(worldPos, 1.0);
	outUV = uvs[gl_VertexIndex];
}
