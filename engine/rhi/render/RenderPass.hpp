/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: RenderPass.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/12 18:36:27 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/07/30 15:07:57                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <map>
#include <unordered_map>
#include <string>
#include <optional>

#include "rhi/render/PipelineMap.hpp"
#include "rhi/render/RenderRequest.hpp"
#include "rhi/render/ExecutionContext.hpp"
#include "rhi/render/RenderDependency.hpp"

namespace	hel::sys {

struct	CycleEntry;

}

namespace	hel {

class	Image;
class	Device;
class	Renderer;
class	ImagePool;
struct	ImageDep;
struct	PhaseDependencies;

class	RenderPass {
	public:
		RenderPass(Device &device, ExecutionContext &context, ImagePool *imagePool,
			const RenderDependency &dep);
		RenderPass(RenderPass &&other);
		~RenderPass(void);

		Renderer		beginPass(void);

		static void	newFrame(void)	{ _passIndex = 0; }

	private:
		bool	addColor(const ImageAccess &dep, ImagePool *pool);
		bool	addDepth(const ImageAccess &dep, ImagePool *pool);
		bool	addStencil(const ImageAccess &dep, ImagePool *pool);
		bool	addRead(const ImageAccess &dep);


		bool	addWrite(ImageDep &dep, ImagePool *imagePool);
		void	resolveOps(Image *img, ImageDep &dep);
		void	addWriteImage(Image *img, ImageDep &dep);

		bool	addRead(const std::string_view &readName);

		void	setViewport(void);
		void	endPass(void);

		Device				&_device;
		ExecutionContext	&_ctx;
		RenderRequest		*_req;
		VkCommandBuffer		_commandBuffer;
		VkExtent2D			_extent;
		bool				_invalidDep{false};
		bool				_passStarted{false};

		std::unordered_map<Ref<Image>, ImageAccess>	_colors;

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
