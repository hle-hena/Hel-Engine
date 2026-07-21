/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ImagePool.tpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/21 17:32:57 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/21 18:02:19                                        */
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
Ref<Image>	ImagePool::acquire(const ImageConfig<T> &requested) {
	if (!_pools.contains(requested)) {
		auto	newImage = Image::create(_device, requested);
	}
}

}
