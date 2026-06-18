/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderPass.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/12 18:36:27 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/18 14:49:44                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include "ecs/systems/ISystem.hpp"

namespace	hel {

class	Device;
class	ImagePool;
struct	FrameContext;
struct	RenderRequest;
struct	PhaseDependencies;

class	RenderPass {
	public:
		RenderPass(Device &device, FrameContext &context, ImagePool *imagePool,
			const std::vector<sys::ISystem::Func*> &systems);
		RenderPass(Device &device, FrameContext &context, ImagePool *imagePool,
			PhaseDependencies dep);
		RenderPass(RenderPass &&other);
		~RenderPass(void);

		Renderer		beginPass(void);

		static void	newFrame(void)	{ _passIndex = 0; }

	private:
		bool	addWrite(ImageDep &dep, ImagePool *imagePool);
		void	resolveOps(Image *img, ImageDep &dep);
		void	addWriteImage(Image *img, ImageDep &dep);

		bool	addRead(const std::string_view &readName);

		void	setViewport(void);
		void	endPass(void);

		Device				&_device;
		FrameContext		&_ctx;
		RenderRequest		*_req;
		VkCommandBuffer		_commandBuffer;
		VkExtent2D			_extent;
		bool				_invalidDep{false};
		bool				_passStarted{false};

		std::unordered_map<std::string, Image *>	_writes{};
		std::unordered_map<std::string, Image *>	_reads{};

		struct	Write {
			std::string					name;
			VkFormat					format;
			VkRenderingAttachmentInfo	info;
		};
		std::map<uint32_t, Write>	_colorInfos{};
		std::optional<Write>		_depthInfo{};
		std::optional<Write>		_stencilInfo{};
		RenderingConfig				_config;

		static uint32_t		_passIndex;
		static uint32_t		newPass(void)	{ return (_passIndex++); }

	friend class	Renderer;
};

}
