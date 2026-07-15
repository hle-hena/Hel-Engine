/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ReadQueue.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/05 18:30:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/15 15:06:08                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "utils/Setters.hpp"
#include "api/vulkan/Buffer.hpp"
#include "HelExpected.hpp"

namespace	hel {

class	Device;
class	Image;

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
	Image		*srcImage{nullptr};
	Ref<Buffer>	dstBuffer{};
	VkOffset3D	offset{0, 0, 0};
	VkExtent3D	extent{1, 1, 1};
};

struct	Read::Context {
	Ref<Buffer>				buffer{};
	uint32_t				frameIndex;
};

template	<typename ReadType>
struct	Read::Builder {
	SETTER(Offset, VkOffset3D, _request.offset);
	SETTER(Extent, VkExtent3D, _request.extent);
	SETTER(SrcImage, Image *, _request.srcImage);
	expected<Context>	push(Device *device);

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

}

#include "core/ReadQueue.tpp"
