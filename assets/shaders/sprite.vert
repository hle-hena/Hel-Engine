/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: billboard.vert                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/24 17:02:11 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/16 19:17:49                                        */
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

struct	Camera {
	mat4	viewMatrix;
};
layout(set = 1, binding = 1) readonly buffer Cameras {
	Camera	data[];
}	cameras;

layout (push_constant) uniform Push {
	uint	entityIndex;
	uint	transformIndex;
	uint	cameraIndex;
}	push;

void	main() {
	vec2	offsets[4] = vec2[](
		vec2(-1.,  1.),
		vec2(-1., -1.),
		vec2( 1.,  1.),
		vec2( 1., -1.)
	);
	vec2	uvs[4] = vec2[](
		vec2(0.0, 0.0),
		vec2(0.0, 1.0),
		vec2(1.0, 0.0),
		vec2(1.0, 1.0)
	);

	mat4	modelMatrix = transforms.data[push.transformIndex].modelMatrix;
	vec3	right = normalize(modelMatrix[0].xyz);
	vec3	up = normalize(modelMatrix[1].xyz);
	vec2	offset = offsets[gl_VertexIndex];
	vec3	worldPos = modelMatrix[3].xyz
							+ right * offset.x * length(modelMatrix[0].xyz)
							+ up * offset.y * length(modelMatrix[1].xyz);

	gl_Position = ubo.viewProjection * vec4(worldPos, 1.0);
	outUV = uvs[gl_VertexIndex];
}
