/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ValidComponent.hpp                                                  */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/02 15:15:26 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/02 15:16:49                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <functional>
#include <string_view>

namespace	hel {

template <typename T>
concept	HasMetaData = requires {
	{ T::MetaData::label }		-> std::convertible_to<std::string_view>;
	{ T::MetaData::gpuVisible }	-> std::convertible_to<bool>;
};

template <typename T>
concept HasPOD = std::is_aggregate_v<typename T::POD>
	&& !std::is_polymorphic_v<typename T::POD>;

template <typename T>
concept IsFlatData = std::is_standard_layout_v<T> && std::is_trivial_v<T>;

template <typename T>
concept ValidateGPULayout = requires {
	requires (!T::MetaData::gpuVisible || (
		T::MetaData::gpuVisible && requires	{
			{ T::MetaData::toGPU }	-> std::convertible_to<
										std::function<typename T::GPULayout
											(const typename T::POD &)>>;
		})
	);
};

template <typename T>
concept ValidComponent = HasMetaData<T> && HasPOD<T> && ValidateGPULayout<T>;

}
