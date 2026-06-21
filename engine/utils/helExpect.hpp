/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Expected.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/13 16:38:21 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/02 18:02:42                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#if __cplusplus >= 202302L && __has_include(<expected>)
	#include <expected>
	namespace hel {
		using std::expected;
		using std::unexpected;
	}
#else
	#include <utils/expected.hpp>
	namespace hel {
		using tl::expected;
		using tl::unexpected;
	}
#endif