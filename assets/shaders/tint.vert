/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: tint.vert                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/31 12:20:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/31 13:32:20                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

void	main(void) {
	vec2	pos = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(pos * 2. - 1.0, 0.0, 1.0);
}
