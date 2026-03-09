/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: SceneViewport.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/09 11:38:46 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/09 11:50:01                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/systems/core/ui/SceneViewport.hpp"
#include "api/vulkan/Image.hpp"
#include "api/ImGui/imgui.h"

namespace	hel::sys {

void	SceneViewport::render(Image *image) {
	ImGui::Begin("Viewport");
	ImTextureRef	textureRef{};
	ImGui::Image(textureRef, {image->getExtent().width, image->getExtent().height});
	ImGui::End();
}

}
