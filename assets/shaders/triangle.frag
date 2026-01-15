/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: triangle.frag                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/12 16:11:39 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/12 16:14:55                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void	main() {
outColor = vec4(fragColor, 1.0);
}