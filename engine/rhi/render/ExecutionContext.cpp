/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ExecutionContext.cpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/29 14:28:35 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/29 14:28:56                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "rhi/render/ExecutionContext.hpp"

namespace	hel {

#define RETURN_ERROR(error) do {	\
	_error = error;					\
	return this;					\
} while (0)

GlobalData	*GlobalData::addData(const std::string &key,
								std::shared_ptr<void> data)
{
	if (_error)
		return this;
	if (_locked)
		RETURN_ERROR("Trying to call addData for key \"" + key
					+ "\" when data have already been passed to the engine.");
	if (_engineGlobals.contains(key) || _shaderGlobals.contains(key))
		RETURN_ERROR("Trying to set two data with the key \""
					+ key + "\".");
	if (data == nullptr)
		RETURN_ERROR("Trying to set data as nullptr for \""
					+ key + "\"");
	_engineGlobals.emplace(key, data);
	return this;
}

GlobalData	*GlobalData::addData(const std::string &key,
								std::shared_ptr<void> data,
								Ref<Buffer> buffer, uint32_t bindingIndex)
{
	if (_error)
		return this;
	if (_locked)
		RETURN_ERROR("Trying to call addData for key \"" + key
					+ "\" when data have already been passed to the engine.");
	if (_engineGlobals.contains(key) || _shaderGlobals.contains(key))
		RETURN_ERROR("Trying to set two data with the key \""
					+ key + "\".");
	if (data == nullptr)
		RETURN_ERROR("Trying to set data as nullptr for \""
					+ key + "\"");
	if (buffer == nullptr)
		RETURN_ERROR("Trying to set buffer as nullptr for \""
					+ key + "\"");
	if (std::find_if(_shaderGlobals.begin(), _shaderGlobals.end(),
				[bindingIndex](const std::pair<std::string, ShaderData> &pair) {
					return pair.second.bindingIndex == bindingIndex;
				}) != _shaderGlobals.end())
	{
		RETURN_ERROR("Trying to set the binding "
					+ std::to_string(bindingIndex) + " two times.");
	}
	_shaderGlobals.emplace(key, ShaderData{data, buffer, bindingIndex});
	return this;
}

std::unordered_map<std::string,
				GlobalData::ShaderData>	&GlobalData::list() {
	return _shaderGlobals;
}

expected<void>	GlobalData::lock() {
	if (_locked)
		return unexpected("Error: setUserData's engine function called twice.");
	if (_error.has_value())
		return unexpected(_error.value());
	_locked = true;
	return {};
}

}
