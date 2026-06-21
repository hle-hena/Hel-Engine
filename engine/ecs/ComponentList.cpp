#include "ecs/ComponentList.hpp"
#include "ecs/Component.hpp"

namespace	hel {

std::vector<const char *>	ComponentList::_componentList{"EditorControllerTag", "BaseControllerTag", "HideEntityTag", "HideEntityInHierarchyTag", "NonSelectableTag", "Name", "Transform", "OffsetTransform", "SurfaceAllignement", "Model", "Texture", "Tint", "Camera", "Controller", "Hierarchy"};

void	ComponentList::addComponent(Registry &registry, Entity::id handle, const char *componentName) {


	if (componentName == std::string("EditorControllerTag"))
		return ((void)registry.addComponent<comp::EditorControllerTag>(handle));
	if (componentName == std::string("BaseControllerTag"))
		return ((void)registry.addComponent<comp::BaseControllerTag>(handle));
	if (componentName == std::string("HideEntityTag"))
		return ((void)registry.addComponent<comp::HideEntityTag>(handle));
	if (componentName == std::string("HideEntityInHierarchyTag"))
		return ((void)registry.addComponent<comp::HideEntityInHierarchyTag>(handle));
	if (componentName == std::string("NonSelectableTag"))
		return ((void)registry.addComponent<comp::NonSelectableTag>(handle));
	if (componentName == std::string("Name"))
		return ((void)registry.addComponent<comp::Name>(handle));
	if (componentName == std::string("Transform"))
		return ((void)registry.addComponent<comp::Transform>(handle));
	if (componentName == std::string("OffsetTransform"))
		return ((void)registry.addComponent<comp::OffsetTransform>(handle));
	if (componentName == std::string("SurfaceAllignement"))
		return ((void)registry.addComponent<comp::SurfaceAllignement>(handle));
	if (componentName == std::string("Model"))
		return ((void)registry.addComponent<comp::Model>(handle));
	if (componentName == std::string("Texture"))
		return ((void)registry.addComponent<comp::Texture>(handle));
	if (componentName == std::string("Tint"))
		return ((void)registry.addComponent<comp::Tint>(handle));
	if (componentName == std::string("Camera"))
		return ((void)registry.addComponent<comp::Camera>(handle));
	if (componentName == std::string("Controller"))
		return ((void)registry.addComponent<comp::Controller>(handle));
	if (componentName == std::string("Hierarchy"))
		return ((void)registry.addComponent<comp::Hierarchy>(handle));
	return ;
}

}
