/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Descriptors.hpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/22 18:47:53 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/27 18:38:20                                        */
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
# include <deque>
# include <unordered_map>
# include <vulkan/vulkan.h>

namespace	hel {

class	Device;
class	Buffer;
class	Image;
struct	DescriptorSet;

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

		bool				allocateSets(DescriptorSet &handle, uint32_t setCount);
		void				resetPools(void);
		void				freeSets(DescriptorSet &handle);

		VkDescriptorPool	getActivePool(void) const
			{ return (_activeHandle->_pool); }

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

struct	DescriptorBindings {
	std::vector<VkDescriptorSetLayoutBinding>	_bindings{};
	std::deque<VkSampler>						_samplers{};

	bool	operator==(const DescriptorBindings &other) const;
};

class	DescriptorFactory {
	public:
		DescriptorFactory(Device &device);
		~DescriptorFactory(void) = default;

		DescriptorFactory	&setSetCount(uint32_t setCount)
			{ _setCount = setCount; return (*this); }

		DescriptorFactory	&addBinding(uint32_t binding, VkDescriptorType type,
										VkShaderStageFlags stages,
										VkSampler sampler = VK_NULL_HANDLE,
										uint32_t descriptorCount = 1);
		std::unique_ptr<DescriptorSet>	build(DescriptorPool &buildPool);
		VkDescriptorSetLayout			getSetLayout(void);

		static void	deleteLayoutCache(Device &device);

	private:
		Device				&_device;
		DescriptorBindings	_bindings;
		uint32_t			_setCount{1};

		static std::unordered_map<DescriptorBindings,
								VkDescriptorSetLayout>	_descriptorSetLayouts;
};

class	DescriptorWriter {
	public:
		DescriptorWriter(Device &device, DescriptorSet *sets);
		~DescriptorWriter(void)	= default;

		DescriptorWriter	&writeBuffer(uint32_t setIndex, uint32_t binding,
										VkDescriptorType type, Buffer &buffer,
										uint32_t offset = 0);
		DescriptorWriter	&writeImage(uint32_t setIndex, uint32_t binding,
									VkDescriptorType type, VkImageView view,
									VkImageLayout layout, VkSampler sampler);
		void				update(void);

	private:
		Device								&_device;
		DescriptorSet						*_handle;
		std::vector<VkWriteDescriptorSet>	_writes{};
		std::deque<VkDescriptorBufferInfo>	_buffersInfo{};
		std::deque<VkDescriptorImageInfo>	_imagesInfo{};
};

struct DescriptorSet {
	using ptr = std::unique_ptr<DescriptorSet>;
	~DescriptorSet(void)	{ free(); }

	VkDescriptorSetLayout			setLayout;
	std::vector<VkDescriptorSet>	sets;
	DescriptorPool::Pool			*parentPool{nullptr};
	DescriptorPool					*manager{nullptr};

	void		free(void)	{ if (manager)	{ manager->freeSets(*this); } }
};


}
