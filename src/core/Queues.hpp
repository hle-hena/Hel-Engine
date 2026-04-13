/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Queues.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/04/13 15:14:30 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/13 16:12:16                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "api/vulkan/Buffer.hpp"
#include "api/vulkan/Image.hpp"
#include "ecs/Entity.hpp"
#include "utils/Setters.hpp"
#include <cstdint>
#include <ui/ImGui/imgui.h>
#include <unordered_map>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace	hel {

class	Image;

struct	ReadRequest {
	Image		*srcImage;
	Buffer		*dstBuffer;
	VkOffset3D	offset;
	VkExtent3D	extent;
	uint32_t	frameIndex;

	SETTER(SrcImage, Image *, srcImage)
	SETTER(DstBuffer, Buffer *, dstBuffer)
	SETTER(Offset, VkOffset3D, offset)
	SETTER(Extent, VkExtent3D, extent)
	SETTER(FrameIndex, uint32_t, frameIndex)
};

class	ReadQueue {
	public:
		static void		push(const ReadRequest &request) {
			_requests.push_back(request);
		}
		static void		execute(VkCommandBuffer commandBuffer) {
			for (auto &req: _requests)
				req.srcImage->copyTo(commandBuffer, req.dstBuffer, req.offset, req.extent);
			_requests.clear();
		}

	private:
		static std::vector<ReadRequest>	_requests;
};



struct	RenderRequest {
	Entity::id									handle;
	ImVec2										origin{0.f, 0.f};
	Image										*mainImage;
	std::unordered_map<std::string, Image *>	secondaryImages{};

	bool	operator==(const RenderRequest &other) const;
	struct	Hasher {
		size_t	operator()(const RenderRequest &request) const;
	};
};

class	RenderQueue {
	public:
		static void		push(const RenderRequest &request) {
			_requests.push_back(request);
		}
		static std::vector<RenderRequest>	flush(void) {
			return (std::move(_requests));
		}

	private:
		static std::vector<RenderRequest>	_requests;
};

}
