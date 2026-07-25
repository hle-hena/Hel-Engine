/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Transform.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/16 15:31:50 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/25 17:32:12                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "core/HelSystem.hpp"
#include "components/Transform.hpp"
#include "components/Model.hpp"
#include "components/Texture.hpp"
#include "components/Tint.hpp"
#include "components/HideTag.hpp"
#include "components/SelectionTag.hpp"

#include <glm/gtc/quaternion.hpp>
#include <optional>
#include <tuple>
#include <vector>
#include <unordered_map>

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

		void	update(const ExecutionContext &ctx);
		void	gizmoAction(const ExecutionContext &ctx);

		void	renderInteraction(const Renderer &renderer);

	private:
		struct	EntityData {
			uint32_t	entityIndex{0};
			uint32_t	transformIndex{0};
			uint32_t	tintIndex{0};
		};
		static void	initSimpleLayout(Device &device,
				std::vector<VkDescriptorSetLayout> &setLayouts,
				std::vector<VkPushConstantRange> &pushConstants);
		static void	configureSimplePipeline(PipelineConfig &config);
		static void	initNDCLayout(Device &device,
				std::vector<VkDescriptorSetLayout> &setLayouts,
				std::vector<VkPushConstantRange> &pushConstants);
		static void	configureNDCPipeline(PipelineConfig &config);

		void	updateEntity(Entity::id handle);

		struct	GizmoContext {
			GizmoContext(Transform *baseSystem, Window *window, Entity::id requestHandle);
			~GizmoContext(void);
			
			operator bool(void) const	{ return (_fullyInit); }

			void	freeHandles(void);

			bool	teleportMouse(const ExecutionContext &ctx);
			void	dragMove(const ExecutionContext &ctx);
			void	dragScale(const ExecutionContext &ctx);
			void	dragRotate(const ExecutionContext &ctx);

			struct	EntityFactory;
			void	initMove(void);
			void	initScale(void);
			void	initRotate(void);
			void	initAction(void);

			static Action					action;
			std::unordered_map<std::string,
								Entity::id>	handles{};
 
			private:
				static constexpr float			GIZMO_SENSIBILITY = 0.00025f;

				uint32_t						_life{1};
				Transform						*_baseSystem;
				Registry						*_registry;
				Window							*_window;
				Entity::id						_requestHandle;
				std::optional<Read::Context>	_read;
				std::optional<std::string>		_dragName;
				bool							_startDrag{false};
				bool							_fullyInit{false};
			friend class	Transform;
		};

		void	registerDrag(const ExecutionContext &ctx, GizmoContext &gizmo);
		void	registerClick(const ExecutionContext &ctx, GizmoContext &gizmo);
		void	renderGizmo(const Renderer &renderer, GizmoContext &gizmoContext);
		void	renderUI(const Renderer &renderer, GizmoContext &gizmoContext);

		AssetManager			*_assetManager;
		PipelineMap				*_simplePipeline;
		PipelineMap				*_NDCPipeline;

		std::unordered_map<RenderRequest, GizmoContext, RenderRequest::Hasher>	_gizmoContexts;

	friend struct	GizmoContext;
	friend struct	GizmoContext::EntityFactory;
};

struct	Transform::GizmoContext::EntityFactory {
	using transformComp = ComponentHandle<hel::comp::Transform>;

	EntityFactory(Transform::GizmoContext *baseGizmo,
				transformComp parentTransform, float scale,
				const std::string &entityName);	
	EntityFactory	&setTint(float r, float g, float b);
	EntityFactory	&setModel(const std::string &modelName);
	EntityFactory	&setOffScale(const glm::vec3 &offScale);
	EntityFactory	&setOffPos(const glm::vec3 &offPos);
	EntityFactory	&setOffRot(const glm::quat &offRot);


	EntityFactory(Transform::GizmoContext *baseGizmo,
		const std::string &entityName);
	EntityFactory	&setPos(float x, float y);
	EntityFactory	&setScale(float x, float y);
	EntityFactory	&setTexture(const std::string &filePath);


	~EntityFactory(void);

	private:
		Transform::GizmoContext	*_baseGizmo;
		transformComp			_parentTransform;
		float					_scale;
		Entity::id				_handle;

		std::tuple<ComponentHandle<comp::Model>,
			ComponentHandle<comp::Texture>,
			ComponentHandle<comp::Transform>,
			ComponentHandle<comp::OffsetTransform>,
			ComponentHandle<comp::Tint>,
			ComponentHandle<comp::HideEntityTag>,
			ComponentHandle<comp::NonSelectableTag>,
			ComponentHandle<comp::HideEntityInHierarchyTag>>	_addedComp;
};

}
