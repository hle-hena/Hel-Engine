/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/14 15:21:25                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "api/vulkan/PipelineMap.hpp"
#include "core/Frame.hpp"
#include "core/Queues.hpp"
#include "ecs/systems/ISystem.hpp"
#include <cstdint>
#include <optional>
#include <vector>
#include <unordered_map>

namespace	hel {

class	AssetManager;
class	InputState;

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
		void	updateInteraction(const FrameContext &ctx) override;
		void	renderInteraction(const Renderer &renderer) override;

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

			void	dragMove(const FrameContext &ctx);

			void	initMove(void);
			void	initAction(void);

			Action							action{Action::Move};
			std::unordered_map<std::string,
								Entity::id>	handles{};
			
			private:
				uint32_t						_life{1};
				Transform						*_baseSystem;
				Registry						*_registry;
				Window							*_window;
				Entity::id						_requestHandle;
				std::optional<Read::Context>	_read;
				std::optional<std::string>		_dragName;
				bool							_fullyInit{false};
			friend class	Transform;
		};

		void	renderMove(const Renderer &renderer, GizmoContext &gizmoContext);
		void	renderScale(const Renderer &renderer);
		void	renderRotate(const Renderer &renderer);

		void	registerDrag(const FrameContext &ctx, GizmoContext &gizmo);
		void	registerClick(const FrameContext &ctx, GizmoContext &gizmo);

		AssetManager			*_assetManager;
		InputState				*_inputState{nullptr};
		PipelineMap				*_simplePipeline;

		std::unordered_map<RenderRequest, GizmoContext, RenderRequest::Hasher>	_gizmoContexts;

	friend struct	GizmoContext;
};

}
