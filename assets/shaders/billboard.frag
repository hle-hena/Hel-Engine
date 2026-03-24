/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: billboard.frag                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/24 17:02:25 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/24 17:06:57                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout(set = 1, binding = 0) uniform sampler2D iconTexture;

layout(push_constant) uniform Push {
	vec3	worldPos;
} push;

layout(location = 0) in  vec2 inUV;
layout(location = 0) out vec4 outColor;

void	main() {
    vec4 texColor = texture(iconTexture, inUV);
    outColor = texColor;

    if (outColor.a < 0.01)
        discard;
}
