/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Expected.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/13 16:38:21 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/13 18:15:45                                        */
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
	#include "api/tl/expected.hpp"
	namespace hel {
		using tl::expected;
		using tl::unexpected;
	}
#endif