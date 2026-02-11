/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Geometry.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/10 16:03:26 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/11 15:46:31                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/assets/Geometry.hpp"
#include "api/vulkan/Buffer.hpp"
#include "utils/mathUtils.hpp"

#ifndef TINYOBJLOADER_IMPLEMENTATION
# define TINYOBJLOADER_IMPLEMENTATION
# include "api/objectLoader/tinyObjLoader.h"
#endif

#include <iostream>
#include <unordered_map>
#include <glm/gtx/hash.hpp>

namespace std
{
	template <>
	struct hash<hel::Vertex>
	{
		size_t	operator()(const hel::Vertex &vertex) const
		{
			size_t	seed = 0;
			hel::mathUtils::hashCombine(seed, vertex.position, vertex.color);
			return (seed);
		}	
	};
}

namespace	hel {

std::vector<VkVertexInputBindingDescription>	Vertex::getBindingDescriptions(void) {
	std::vector<VkVertexInputBindingDescription>	bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return (bindingDescriptions);
}

std::vector<VkVertexInputAttributeDescription>	Vertex::getAttributeDescriptions(void) {
	std::vector<VkVertexInputAttributeDescription>	attributeDescriptions{};

	attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
	attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});

	return (attributeDescriptions);
}

bool	Vertex::operator==(const Vertex &other) const {
	return (position == other.position && color == other.color);
}

std::shared_ptr<Geometry>	Geometry::load(Device &device, const std::string &path) {
	tinyobj::attrib_t					attrib;
	std::vector<tinyobj::shape_t>	 	shapes;
	std::vector<tinyobj::material_t>	materials;
	std::string							warn, error;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, path.c_str())) {
		std::cerr << (warn + error) << std::endl;
		return (nullptr);
	}
	std::vector<Vertex>						vertices;
	std::vector<uint32_t>					indices;
	std::unordered_map<Vertex, uint32_t>	uniqueVertices;
	for (auto &shape: shapes) {
		for (auto &index: shape.mesh.indices) {
			if (index.vertex_index >= 0) {
				Vertex	vertex{};

				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.color= {
					attrib.colors[3 * index.vertex_index + 0],
					attrib.colors[3 * index.vertex_index + 1],
					attrib.colors[3 * index.vertex_index + 2]
				};
				if (uniqueVertices.find(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}


	return (std::shared_ptr<Geometry>(new Geometry{path,
			createBuffer(device, vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT),
			createBuffer(device, indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
			static_cast<uint32_t>(indices.size())}));
}

}
