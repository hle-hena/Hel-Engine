/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Geometry.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/10 16:01:55 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/11 16:07:19                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>
# include <glm/glm.hpp>
# include <vector>
# include <memory>
# include <string>

namespace	hel {

class	Buffer;
class	Device;

struct	Vertex {
	glm::vec3	position{0.};
	glm::vec3	color{1.};
	glm::vec3	normal{0.};

	static std::vector<VkVertexInputBindingDescription>		getBindingDescriptions(void);
	static std::vector<VkVertexInputAttributeDescription>	getAttributeDescriptions(void);
	
	bool	operator==(const Vertex &other) const;
};

struct	Geometry {
	std::string					filePath;

	std::unique_ptr<Buffer>		vertexBuffer;
	std::unique_ptr<Buffer>		indexBuffer;
	uint32_t					vertexCount;

	static std::shared_ptr<Geometry>	load(Device &device,
											const std::string &path);

	private:
		template <typename T>
		static std::unique_ptr<Buffer>	createBuffer(Device &device,
													std::vector<T> data,
													VkBufferUsageFlags usage);
};

}

#include "ecs/assets/Geometry.tpp"
