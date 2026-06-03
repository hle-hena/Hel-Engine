/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ISystem.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 14:44:05 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/03 18:21:00                                        */
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
		Color = 1,
		Depth = 2,
		Stencil = 4,
		DepthStencil = Depth | Stencil,
		MAX_ENUM
	};

	SETTER(ImageName, std::string, imageName)
	SETTER(ImageUsage, Usage, usage)
	SETTER(ImageConfig, Image::Config, config)
	SETTER(FormatAsked, VkFormat, format)
	SETTER(LoadOp, VkAttachmentLoadOp, load)
	SETTER(StoreOp, VkAttachmentStoreOp, store)
	SETTER(ClearValue, VkClearValue, clear)

	std::string							imageName;
	VkFormat							format{VK_FORMAT_MAX_ENUM};
	Usage								usage{MAX_ENUM};
	std::optional<Image::Config>		config;
	std::optional<VkClearValue>			clear;
	VkAttachmentLoadOp					load{VK_ATTACHMENT_LOAD_OP_MAX_ENUM};
	VkAttachmentStoreOp					store{VK_ATTACHMENT_STORE_OP_MAX_ENUM};
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
