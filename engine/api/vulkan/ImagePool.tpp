/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImagePool.tpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/21 17:32:57 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/22 14:21:33                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/ImagePool.hpp"

namespace	hel {

template <ImageType T>
Ref<Image>	ImagePool::acquire(uint32_t frameIndex,
							const ImageConfig<T> &requestedConfig)
{
	auto	&slots = _pools[requestedConfig];//Does this create the bucket if needed ?

	if (slots.unusedImages.empty()) {
		auto	image = Image::create(_device, requestedConfig);
		slots.usedImages[frameIndex].push_back(image);
		return image;
	} else {
		auto	image = slots.unusedImages.back();
		slots.unusedImage.pop_back();
		slots.usedImages[frameIndex].push_back(image);
		return image;
	}
}

}
