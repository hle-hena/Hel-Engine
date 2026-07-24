/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: add.cpp                                                             */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/03 11:12:42 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 15:26:43                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "components/Controllers.hpp"
#include "components/HideTag.hpp"
#include "components/SelectionTag.hpp"
#include "components/Name.hpp"
#include "components/Transform.hpp"
#include "components/Model.hpp"
#include "components/Texture.hpp"
#include "components/Tint.hpp"
#include "components/Camera.hpp"

#include "core/ecs/ComponentManager.hpp"

namespace hel {

ComponentRegistrar<comp::EditorControllerTag>		_editorControllerTag;
ComponentRegistrar<comp::BaseControllerTag>			_baseControllerTag;
ComponentRegistrar<comp::HideEntityTag>				_hideEntityTag;
ComponentRegistrar<comp::HideEntityInHierarchyTag>	_hideEntityInHierarchyTag;
ComponentRegistrar<comp::NonSelectableTag>			_nonSelectableTag;
ComponentRegistrar<comp::SelectedTag>				_selectedTag;
ComponentRegistrar<comp::Name>						_name;
ComponentRegistrar<comp::SurfaceAllignement>		_surfaceAllignement;
ComponentRegistrar<comp::OffsetTransform>			_offsetTransform;
ComponentRegistrar<comp::Transform>					_transform;
ComponentRegistrar<comp::Model>						_model;
ComponentRegistrar<comp::Texture>					_texture;
ComponentRegistrar<comp::Tint>						_tint;
ComponentRegistrar<comp::Camera>					_camera;
ComponentRegistrar<comp::Controller>				_controller;

}
