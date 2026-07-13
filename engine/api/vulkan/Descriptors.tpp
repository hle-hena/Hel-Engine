/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Descriptors.tpp                                                     */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/13 13:02:10 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/13 13:03:03                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "api/vulkan/Descriptors.hpp"

namespace	hel {

template <typename T>
DescriptorWriter	&DescriptorWriter::writeBuffer(uint32_t setIndex,
												uint32_t binding,
												VkDescriptorType type,
												Buffer<T> &buffer,
												uint32_t offset) {
	VkDescriptorBufferInfo	bufferInfo = buffer.getDescriptorInfo();
	bufferInfo.offset = offset;
	_buffersInfo.push_back(bufferInfo);

	VkWriteDescriptorSet	write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = _handle->sets[setIndex];
	write.dstBinding = binding;
	write.descriptorType = type;
	write.descriptorCount = 1;
	write.pBufferInfo = &_buffersInfo.back();
	_writes.push_back(write);
	return (*this);
}

}
