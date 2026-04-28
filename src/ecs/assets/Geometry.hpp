/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Geometry.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/10 16:01:55 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/27 20:52:12                                        */
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
	glm::vec2	uv{0.};
	glm::vec3	normal{0.};
	glm::vec3	color{0.};

	static std::vector<VkVertexInputBindingDescription>		getBindingDescriptions(void);
	static std::vector<VkVertexInputAttributeDescription>	getAttributeDescriptions(void);

	bool	operator==(const Vertex &other) const;
};

struct	Submesh {
	uint32_t	triFirstIndex;
	uint32_t	triIndexCount;
	uint32_t	lineFirstIndex;
	uint32_t	lineIndexCount;
	uint32_t	materialID;
};

struct	Geometry {
	std::string					modelName;

	std::unique_ptr<Buffer>		vertexBuffer;
	std::unique_ptr<Buffer>		triangleIndexBuffer;
	uint32_t					triangleVertexCount;

	std::vector<Submesh>		submeshes;
	std::vector<std::string>	materialPaths{};

	virtual bool	isLoadedFully(void) const;
	static std::shared_ptr<Geometry>	load(Device &device,
											const std::string &modelName);

	protected:
		struct	GeometryVectors {
			std::vector<Vertex>			vertices{};
			std::vector<uint32_t>		triangleIndices{};
			std::vector<uint32_t>		lineIndices{};

			std::vector<Submesh>		submeshes{};
			std::vector<std::string>	materialPaths{};
		};
		static GeometryVectors	loadFile(const std::string &modelName, bool fullLoad);

		template <typename T>
		static std::unique_ptr<Buffer>	createBuffer(Device &device,
													std::vector<T> data,
													VkBufferUsageFlags usage);
};

struct	FullGeometry : public Geometry {
	using AssetPool = Geometry;

	std::unique_ptr<Buffer>		lineIndexBuffer;
	uint32_t					lineVertexCount;

	bool	isLoadedFully(void) const override;
	static std::shared_ptr<FullGeometry> load(Device &device,
											  const std::string &modelName);
};

}

#include "ecs/assets/Geometry.tpp"
