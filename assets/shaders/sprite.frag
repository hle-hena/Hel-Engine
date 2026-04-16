/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: sprite.frag                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/04/16 18:05:30 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/16 18:24:05                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout(location = 0) out vec4	outColor;
layout(location = 1) out uint	outEntityIndex;

layout(location = 0) in     vec2 inUV;

layout(set = 2, binding = 0) uniform sampler2D iconTexture;

layout (push_constant) uniform Push {
	uint	entityIndex;
	uint	transformIndex;
	uint	cameraIndex;
}	push;


void	main() {
    vec4 texColor = texture(iconTexture, inUV);
    outColor = texColor;
    outEntityIndex = push.entityIndex;

    if (outColor.a < 0.01)
        discard;
}
