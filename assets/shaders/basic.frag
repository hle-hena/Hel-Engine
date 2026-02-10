/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: basic.frag                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/27 17:07:46 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/10 19:18:02                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout (location = 0) in vec3		fragColor;
layout (location = 1) flat in int	primId;

layout (location = 0) out vec4		outColor;

float	squirrelHash(int position, uint noise1, uint noise2, uint noise3) {
	uint mangled = position;
	mangled *= noise1;
	mangled ^= (mangled >> 8);
	mangled += noise2;
	mangled ^= (mangled << 8);
	mangled *= noise3;
	mangled ^= (mangled >> 8);

	return (float(mangled) / float(0xFFFFFFFFu));
}

vec3	hashColor(int primId) {
	return (vec3(
		squirrelHash(primId, 0xB5297A4D, 0x68E31DA4, 0x1B56C4E9),
		squirrelHash(primId, 0x1B56C4E9, 0xB5297A4D, 0x68E31DA4),
		squirrelHash(primId, 0x68E31DA4, 0x1B56C4E9, 0xB5297A4D)
	));
}

void	main() {
	bool	debugColor = true;
	if (debugColor)
		outColor = vec4(hashColor(primId), 1.0);
	else
		outColor = vec4(fragColor, 1.0);
}
