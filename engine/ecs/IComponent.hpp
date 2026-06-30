/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: IComponent.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/30 10:44:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/30 13:36:34                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <string_view>

namespace	hel {

template <typename T>
concept	HasMetaData = requires {
	{ T::MetaData::label }		-> std::convertible_to<std::string_view>;
	{ T::MetaData::gpuVisible }	-> std::convertible_to<bool>;
};

template <typename T>
concept HasPOD = std::is_standard_layout_v<typename T::POD>
				&& std::is_trivial_v<typename T::POD>;

template <typename T>
concept ValidComponent = HasMetaData<T> && HasPOD<T>;

template <typename Derived>
struct	IComponent {
	struct	POD {};
	struct	MetaData {
		static constexpr std::string_view	label = "Unnamed Component";
		static constexpr bool				gpuVisible = false;
	};
};

}
