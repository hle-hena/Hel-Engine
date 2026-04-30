#include "ecs/ComponentList.hpp"

namespace	hel {

std::vector<const char *>	ComponentList::_componentList{"EditorControllerTag", "BaseControllerTag", "HideEntityTag", "HideEntityInHierarchyTag", "NonSelectableTag", "Name", "Transform", "OffsetTransform", "SurfaceAllignement", "Model", "Texture", "Tint", "Camera", "Controller", "Hierarchy"};

void	ComponentList::addComponent(Registry &registry, Entity::id handle, const char *componentName) {


	if (componentName == "EditorControllerTag")
		return ((void)registry.addComponent<comp::EditorControllerTag>(handle));
	if (componentName == "BaseControllerTag")
		return ((void)registry.addComponent<comp::BaseControllerTag>(handle));
	if (componentName == "HideEntityTag")
		return ((void)registry.addComponent<comp::HideEntityTag>(handle));
	if (componentName == "HideEntityInHierarchyTag")
		return ((void)registry.addComponent<comp::HideEntityInHierarchyTag>(handle));
	if (componentName == "NonSelectableTag")
		return ((void)registry.addComponent<comp::NonSelectableTag>(handle));
	if (componentName == "Name")
		return ((void)registry.addComponent<comp::Name>(handle));
	if (componentName == "Transform")
		return ((void)registry.addComponent<comp::Transform>(handle));
	if (componentName == "OffsetTransform")
		return ((void)registry.addComponent<comp::OffsetTransform>(handle));
	if (componentName == "SurfaceAllignement")
		return ((void)registry.addComponent<comp::SurfaceAllignement>(handle));
	if (componentName == "Model")
		return ((void)registry.addComponent<comp::Model>(handle));
	if (componentName == "Texture")
		return ((void)registry.addComponent<comp::Texture>(handle));
	if (componentName == "Tint")
		return ((void)registry.addComponent<comp::Tint>(handle));
	if (componentName == "Camera")
		return ((void)registry.addComponent<comp::Camera>(handle));
	if (componentName == "Controller")
		return ((void)registry.addComponent<comp::Controller>(handle));
	if (componentName == "Hierarchy")
		return ((void)registry.addComponent<comp::Hierarchy>(handle));
	return ;
}

}
