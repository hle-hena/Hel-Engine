/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: GlobalData.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/27 16:38:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/27 16:38:50                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <glm/glm.hpp>

namespace	hel {

struct	GlobalUBO {
	glm::mat4	viewProjection;
	float		elapsedTime;
};

}
