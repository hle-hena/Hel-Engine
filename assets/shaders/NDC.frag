/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: NDC.frag                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/04/21 14:56:48 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/21 20:53:39                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout (location = 0) out vec4		outColor;
layout (location = 1) out uint		outEntityID;

layout (location = 0) in vec2		inUV;
layout (location = 1) in vec3		inColor;

layout(set = 2, binding = 0) uniform sampler2D iconTexture;

layout (push_constant) uniform Push {
	uint	entityIndex;
	uint	transformIndex;
	uint	tintIndex;
} push;

void	main(void) {
	vec4 texColor = texture(iconTexture, inUV);
	outColor = texColor;
	outEntityID = push.entityIndex;

	if (outColor.a < 0.01)
		outColor = vec4(inColor, 1.);
}
