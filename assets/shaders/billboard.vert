/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: billboard.vert                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/24 17:02:11 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/25 10:26:44                                        */
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

layout (push_constant) uniform Push {
	vec3	worldPos;
	float	size;
} push;

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

	vec3	right = vec3(ubo.viewProjection[0][0], ubo.viewProjection[1][0], ubo.viewProjection[2][0]);
	vec3	up = vec3(ubo.viewProjection[0][1], ubo.viewProjection[1][1], ubo.viewProjection[2][1]);
	vec2	offset = offsets[gl_VertexIndex] * push.size;
	vec3	worldPos = push.worldPos + right * offset.x + up * offset.y;

	gl_Position = ubo.viewProjection * vec4(worldPos, 1.0);
	outUV = uvs[gl_VertexIndex];
}
