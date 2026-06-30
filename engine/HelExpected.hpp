/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Expected.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/13 16:38:21 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/26 14:56:30                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <string>

#if __cplusplus >= 202302L && __has_include(<expected>)
	#include <expected>
	namespace hel {
		template <typename T>
		using expected = std::expected<T, std::string>;
		using unexpected = std::unexpected<std::string>;
	}
#else
	#include <utils/expected.hpp>
	namespace hel {
		template <typename T>
		using expected = tl::expected<T, std::string>;
		using unexpected = tl::unexpected<std::string>;
	}
#endif
