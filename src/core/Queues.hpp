/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Queues.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/04/13 15:14:30 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/13 18:42:26                                        */
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
#include <memory>
#include <ui/ImGui/imgui.h>
#include <unordered_map>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace	hel {

class	Read {
	private:
		struct	Request;

		template<typename ReadType>
		struct	Builder;

	public:
		struct	Context;

		class	Queue;
};

struct	Read::Request {
	Image		*srcImage;
	Buffer		*dstBuffer;
	VkOffset3D	offset;
	VkExtent3D	extent;
};

struct	Read::Context {
	std::unique_ptr<Buffer>	buffer;
	uint32_t				frameIndex;
};

template	<typename ReadType>
struct	Read::Builder {
	SETTER(Offset, VkOffset3D, _request.offset);
	SETTER(Extent, VkExtent3D, _request.extent);
	SETTER(SrcImage, Image *, _request.srcImage);
	Context	push(Device &device);

	private:
		Builder(uint32_t frameIndex);

		Context	_context;
		Request	_request;

	friend class	Queue;
};

class	Read::Queue {
	public:
		template <typename ReadType>
		static Builder<ReadType>	newRequest(uint32_t frameIndex) {
			return (Builder<ReadType>(frameIndex));
		}
		static void		execute(VkCommandBuffer commandBuffer);

	private:
		static std::vector<Request>	_requests;

	template <typename ReadType>
	friend struct	Builder;
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

#include "core/Queues.tpp"
