/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: GLFW.hpp                                                            */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/10 12:04:15 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/10 15:59:57                                        */
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

class	GLFW {
	private:
		~GLFW(void) = delete;
		static inline uint32_t	_instanceCount = 0;
		static inline uint32_t	_maxInstanceCount = 1;

	public:
		static bool	acquire(void) {
			if (_instanceCount == _maxInstanceCount)
				return (false);
			if (_instanceCount == 0) {
				if (!glfwInit())
					return (false);
			}
			_instanceCount++;
			return (true);
		}
		static void	release(void) {
			if (_instanceCount == 0)
				return ;
			_instanceCount--;
			if (_instanceCount == 0)
				glfwTerminate();
		}
};

}
