/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 14:44:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/02 19:51:01                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "api/vulkan/PipelineMap.hpp"
# include "api/vulkan/Renderer.hpp"
# include "core/Frame.hpp"
#include "ecs/Entity.hpp"

# include <vulkan/vulkan.h>
# include <vector>
# include <memory>

namespace	hel {

class	Device;
class	Registry;
class	ImagePool;
struct	EngineContext;
struct	FrameContext;

}

namespace	hel::sys {

struct	ImageDep {
	enum	Usage {
		Color,
		Depth,
		Stencil
	};

	std::string			imageName;
	Image::Config		config;
	VkAttachmentLoadOp	load{VK_ATTACHMENT_LOAD_OP_CLEAR};
	VkAttachmentStoreOp	store{VK_ATTACHMENT_STORE_OP_STORE};
	VkFormat			format;
	VkClearValue		clear;
	Usage				usage;
};

struct	PhaseDependencies {
	std::vector<std::string>	require{};
	std::vector<std::string>	block{};
	std::optional<std::string>	provides;

	std::vector<ImageDep>		write;
	std::vector<ImageDep>		read;
};

class	ISystem {
	public:
		ISystem(void) = default;
		virtual ~ISystem(void) = default;

		ISystem(const ISystem &other) = delete;
		ISystem	&operator=(const ISystem &other) = delete;

		virtual void	init(const EngineContext &engineCtx,
							const FrameContext &frameCtx) final;
		virtual void	init(void) = 0;

		virtual void	updateInteraction(const FrameContext &) {}
		virtual void	update(const FrameContext &) {}

		virtual void	render(const Renderer &) {}
		virtual void	postProcessing(const Renderer &) {}
		virtual void	renderInteraction(const Renderer &) {}

		PhaseDependencies	updateDeps;
		PhaseDependencies	updateInterDeps;
		PhaseDependencies	renderDeps;
		PhaseDependencies	postProcessDeps;
		PhaseDependencies	renderInterDeps;

	protected:
		virtual PipelineMap	*createPipeline(const
								PipelineMap::Config &config) final;

		virtual Renderer::Draw	drawCommand(const Renderer &renderer,
											PipelineMap *pipeline) const final;

		Device										*_device;
		Registry									*_registry;
		ImagePool									*_imagePool;
		std::vector<std::unique_ptr<PipelineMap>>	_pipelines;

	private:
		FrameContext	_frameCtx;
};

}
