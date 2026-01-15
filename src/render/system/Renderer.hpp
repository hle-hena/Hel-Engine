/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Renderer.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/06 16:35:00 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/15 15:01:22                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <cassert>

#include "render/system/MeshSystem.hpp"

namespace hel {

class	Window;
class	Device;

class	Renderer {
	public:
		Renderer(Device &device);
		~Renderer();

		Renderer(const Renderer &) = delete;
		Renderer &operator=(const Renderer &) = delete;

		void	render(Window &window);

	private:
		Device			&_device;
		MeshSystem		_meshSystem;

};

}