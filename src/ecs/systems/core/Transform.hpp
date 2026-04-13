/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/13 18:46:06                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "api/vulkan/PipelineMap.hpp"
#include "core/Queues.hpp"
#include "ecs/systems/ISystem.hpp"
#include <cstdint>
#include <vector>
#include <unordered_map>

namespace	hel {

class	AssetManager;

}

namespace	hel::sys {

class	Transform : public ISystem {
	public:
		enum class	Action {
			Move,
			Scale,
			Rotate
		};

		Transform(void) = default;
		~Transform(void) = default;

		void	init(void) override;

		void	update(const FrameContext &ctx) override;
		void	registerUI(const FrameContext &ctx) override;
		void	renderUI(const Renderer &renderer) override;

	private:
		struct	EntityData {
			uint32_t	entityIndex{0};
			uint32_t	transformIndex{0};
			uint32_t	tintIndex{0};
		};
		static void	initLayout(Device &device, std::vector<VkDescriptorSetLayout> &setLayouts,
				std::vector<VkPushConstantRange> &pushConstants);
		static void	configurePipeline(PipelineConfig &config);

		void	updateEntity(Entity::id handle);


		struct	GizmoContext {
			GizmoContext(Transform *baseSystem, Window *window, Entity::id requestHandle);
			~GizmoContext(void);
			
			operator bool(void) const	{ return (_fullyInit); }

			void	freeHandles(void);

			void	initMove(void);
			void	initAction(void);

			Action							action{Action::Move};
			std::unordered_map<std::string,
								Entity::id>	handles{};
			
			private:
				uint32_t				_life{1};
				Transform				*_baseSystem;
				Registry				*_registry;
				Window					*_window;
				Entity::id				_requestHandle;
				bool					_fullyInit{false};
			friend class	Transform;
		};
		void	renderMove(const Renderer &renderer, GizmoContext &gizmoContext);

		void	renderScale(const Renderer &renderer);
		void	renderRotate(const Renderer &renderer);

		AssetManager			*_assetManager;
		PipelineMap				*_simplePipeline;

		std::unordered_map<RenderRequest, GizmoContext, RenderRequest::Hasher>	_gizmoContexts;

		std::unordered_map<RenderRequest, Read::Context, RenderRequest::Hasher>	_requests;

	friend struct	GizmoContext;
};

}
