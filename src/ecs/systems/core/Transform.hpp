/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/15 18:00:58                                        */
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
#include "ecs/Registry.hpp"
#include "ecs/Component.hpp"
#include "ecs/systems/ISystem.hpp"
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>
#include <optional>
#include <tuple>
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
			void	dragScale(const FrameContext &ctx);

			struct	EntityFactory;
			void	initMove(void);
			void	initScale(void);
			void	initAction(void);

			Action							action{Action::Scale};
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

		void	renderGizmo(const Renderer &renderer, GizmoContext &gizmoContext);

		void	registerDrag(const FrameContext &ctx, GizmoContext &gizmo);
		void	registerClick(const FrameContext &ctx, GizmoContext &gizmo);

		AssetManager			*_assetManager;
		InputState				*_inputState{nullptr};
		PipelineMap				*_simplePipeline;

		std::unordered_map<RenderRequest, GizmoContext, RenderRequest::Hasher>	_gizmoContexts;

	friend struct	GizmoContext;
	friend struct	GizmoContext::EntityFactory;
};

struct	Transform::GizmoContext::EntityFactory {
	using transformComp = ComponentHandle<hel::comp::Transform>;

	EntityFactory(Transform::GizmoContext *baseSystem,
				transformComp &parentTransform, float scale,
				const std::string &entityName);
	~EntityFactory(void);

	EntityFactory	&setTint(float r, float g, float b);
	EntityFactory	&setModel(const std::string &filepath);
	EntityFactory	&setOffScale(const glm::vec3 &offScale);
	EntityFactory	&setOffPos(const glm::vec3 &offPos);
	EntityFactory	&setOffRot(const glm::quat &offRot);

	private:
		Transform::GizmoContext	*_baseGizmo;
		transformComp			&_parentTransform;
		float					_scale;
		Entity::id				_handle;

		std::tuple<ComponentHandle<comp::Model>,
			ComponentHandle<comp::Transform>,
			ComponentHandle<comp::OffsetTransform>,
			ComponentHandle<comp::HideEntityTag>,
			ComponentHandle<comp::HideEntityInHierarchyTag>,
			ComponentHandle<comp::NonSelectableTag>,
			ComponentHandle<comp::Tint>>			_addedComp;
};

}
