/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderPass.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/12 18:36:27 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/07/31 12:28:03                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "rhi/render/PipelineMap.hpp"
#include "rhi/render/RenderRequest.hpp"
#include "rhi/render/ExecutionContext.hpp"
#include "rhi/render/RuntimeDependency.hpp"

namespace	hel::sys {

struct	CycleEntry;

}

namespace	hel {

class	Device;
class	Renderer;

class	RenderPass {
	public:
		RenderPass(Device &device, ExecutionContext &context,
			std::shared_ptr<PassDependencies> deps);
		RenderPass(RenderPass &&other);
		~RenderPass(void);

		Renderer		beginPass(void);

		static void	newFrame(void)	{ _passIndex = 0; }

	private:
		void	prepareWriteImage(const ResolvedAccess_Img &img);
		void	prepareReadImage(const ResolvedAccess_Img &img);

		// bool	addWrite(ImageDep &dep, ImagePool *imagePool);
		// void	resolveOps(Image *img, ImageDep &dep);
		// void	addWriteImage(Image *img, ImageDep &dep);

		// bool	addRead(const std::string_view &readName);

		void	setViewport(void);
		void	endPass(void);

		Device				&_device;
		ExecutionContext	&_ctx;
		RenderRequest		*_req;
		VkCommandBuffer		_commandBuffer;
		VkExtent2D			_extent;
		bool				_passStarted{false};

		std::shared_ptr<PassDependencies>	_deps;

		RenderingConfig				_config;

		static uint32_t		_passIndex;
		static uint32_t		newPass(void)	{ return (_passIndex++); }

	friend class	Renderer;
};

}
