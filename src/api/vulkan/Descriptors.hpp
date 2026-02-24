/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Descriptors.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/22 18:47:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/24 21:30:17                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <memory>
# include <vector>
# include <unordered_map>
# include <vulkan/vulkan.h>

namespace	hel {

class	Device;
class	DescriptorSet;

class	DescriptorPool {
	public:
		struct	Builder;
		struct Pool {
			using ptr = std::unique_ptr<Pool>;

			VkDescriptorPool	_pool{nullptr};
			uint32_t			_allocatedSets{0};
		};

		~DescriptorPool(void);
		DescriptorPool(const DescriptorPool &other) = delete;
		DescriptorPool	&operator=(const DescriptorPool &other) = delete;

		bool	allocateSets(VkDescriptorSetLayout setLayout,
							DescriptorSet &handle, uint32_t setCount);
		void	resetPools(void);
		void	freeSets(DescriptorSet &handle);

	private:
		DescriptorPool(Device &device, uint32_t pageSize,
					VkDescriptorPoolCreateFlags flags,
					std::vector<VkDescriptorPoolSize> &poolSizes);

		Pool::ptr	createNewPool(void);
		void		getNewPool(void);

		Device								&_device;
		uint32_t							_pageSize;
		VkDescriptorPoolCreateFlags			_poolCreationFlags;
		std::vector<VkDescriptorPoolSize>	_poolSizes{};
		std::vector<Pool::ptr>				_pools{};
		Pool								*_activeHandle{nullptr};
		std::vector<Pool *>					_fullPools{};
		std::vector<Pool *>					_emptyPools{};
};

struct	DescriptorPool::Builder {
	Builder(Device &device) : _device{device} {};
	~Builder(void) = default;

	Builder		&addDescriptor(VkDescriptorType type, float ratio = 1.f)
		{ _descriptorsRatio[type] += ratio; return (*this); }

	Builder		&setCreationFlag(VkDescriptorPoolCreateFlags flags)
		{ _poolCreationFlags = flags; return (*this); }

	Builder		&setPageSize(uint32_t pageSize)
		{ _pageSize = pageSize; return (*this); }

	std::unique_ptr<DescriptorPool>	build(void);

	private:
		Device								&_device;
		uint32_t							_pageSize{1000};
		std::unordered_map<VkDescriptorType,
							float>			_descriptorsRatio{};
		VkDescriptorPoolCreateFlags			_poolCreationFlags{0};
};

struct DescriptorSet {
	std::vector<VkDescriptorSet>	sets;
	DescriptorPool::Pool			*parentPool{nullptr};
	DescriptorPool					*manager{nullptr};

	void	free(void)	{ if (manager)	{ manager->freeSets(*this); } }
};


}
