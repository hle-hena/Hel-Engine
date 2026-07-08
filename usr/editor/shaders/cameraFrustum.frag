/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: cameraFrustum.frag                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/20 10:45:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/20 15:27:03                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

# version 450

layout (location = 0) out vec4	outColor;

layout (location = 0) in vec3	inColor;

layout (binding = 0) uniform UniformBufferObject {
	mat4	viewProjection;
	float	elapsedTime;
}	globalUBO;

void	main() {
	outColor = vec4(inColor, 1.);
}
