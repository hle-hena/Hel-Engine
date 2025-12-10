/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: KeyEvent.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 17:15:43 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/10 17:38:15                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# define GLFW_INCLUDE_VULKAN
# include <GLFW/glfw3.h>

namespace	hel {

void	keyEventCallback(GLFWwindow *window, int key, int scanCode,
						int action, int modifier);

}
