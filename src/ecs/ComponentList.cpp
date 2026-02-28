#include "ecs/ComponentList.hpp"

namespace	hel {

std::vector<const char *>	ComponentList::_componentList{"EditorControllerTag", "BaseControllerTag", "Name", "Transform", "SurfaceAllignement", "Model", "Camera", "Controller", "Hierarchy"};

const void	*ComponentList::addComponent(Registry &registry, Entity::id handle, const char *componentName) {


	if (componentName == "EditorControllerTag")
		return (registry.addComponent<comp::EditorControllerTag>(handle));
	if (componentName == "BaseControllerTag")
		return (registry.addComponent<comp::BaseControllerTag>(handle));
	if (componentName == "Name")
		return (registry.addComponent<comp::Name>(handle));
	if (componentName == "Transform")
		return (registry.addComponent<comp::Transform>(handle));
	if (componentName == "SurfaceAllignement")
		return (registry.addComponent<comp::SurfaceAllignement>(handle));
	if (componentName == "Model")
		return (registry.addComponent<comp::Model>(handle));
	if (componentName == "Camera")
		return (registry.addComponent<comp::Camera>(handle));
	if (componentName == "Controller")
		return (registry.addComponent<comp::Controller>(handle));
	if (componentName == "Hierarchy")
		return (registry.addComponent<comp::Hierarchy>(handle));
	return (NULL);
}

}
