/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Pipeline.tpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/29 12:21:24 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/29 12:23:54                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "Pipeline.hpp"

namespace	hel {

template <typename VertexType>
void	Pipeline::setVertexInputDescriptions(PipelineConfigInfo &configInfo) {
	configInfo.attributeDescription = VertexType::getAttributeDescriptions();
	configInfo.bindingDescription = VertexType::getBindingDescriptions();
}

}
