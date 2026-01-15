/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: triangle.vert                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/12 16:11:26 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/12 16:16:16                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout(location = 0) out vec3 fragColor;

vec2 positions[3] = vec2[](
	vec2(0.0, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5)
);

vec3 colors[3] = vec3[](
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0)
);

void	main() {
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
	fragColor = colors[gl_VertexIndex];
}