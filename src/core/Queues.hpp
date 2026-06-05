/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Queues.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/04/13 15:14:30 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/05 12:44:08                                        */
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
#include "core/PhaseDependancy.hpp"
#include <cstdint>
#include <memory>
#include <ui/ImGui/imgui.h>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace	hel::sys {

struct	DepHasher;

}

namespace	hel {

struct	RenderRequest {
	std::string									requestType;
	Entity::id									handle;
	ImVec2										origin{0.f, 0.f};
	std::unordered_map<std::string, Image *>	images{};

	bool	operator==(const RenderRequest &other) const;
	struct	Hasher {
		size_t	operator()(const RenderRequest &request) const;
	};
};

}

//TODO this is not beautifull. The ideal would be to split the files I think.
#include "api/vulkan/Renderer.hpp"

namespace	hel {

class	RenderQueue {
	public:
		static void		push(const RenderRequest &request) {
			if (!request.images.empty() && request.images.contains("mainColor"))
				_requests.push_back(request);
		}
		static std::vector<RenderRequest>	flush(void) {
			return (std::move(_requests));
		}

	private:
		static std::vector<RenderRequest>	_requests;
};

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
	Buffer		*dstBuffer{nullptr};
	VkOffset3D	offset{0, 0, 0};
	VkExtent3D	extent{1, 1, 1};
};

struct	Read::Context {
	std::unique_ptr<Buffer>	buffer{nullptr};
	uint32_t				frameIndex;
};

template	<typename ReadType>
struct	Read::Builder {
	SETTER(Offset, VkOffset3D, _request.offset);
	SETTER(Extent, VkExtent3D, _request.extent);
	SETTER(SrcImage, Image *, _request.srcImage);
	Context	push(Device &device);//TODO -> Check the current request cache to try to re use

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



class	DrawQueue {
	public:
		struct	RequestVector {
			PhaseDependencies		dep;
			std::vector<Renderer::Draw>	draws;
		};
		using InnerMap = std::map<uint32_t, std::vector<RequestVector>>;
		struct	RequestMap {
			public:
				RequestVector	*at(uint32_t levelAsked, const PhaseDependencies &depAsked);
				void			clear(void);
			private:
				InnerMap	_data{};
			
			friend class DrawQueue;
		};

		static void	requestDraw(uint32_t level, Renderer::Draw &&drawCommand,
								PhaseDependencies &dep);
		static InnerMap	flush(void) { return std::move(_requests._data); };

	private:
		static RequestMap	_requests;

	template <typename ReadType>
	friend struct	Builder;
};

}

#include "core/Queues.tpp"
