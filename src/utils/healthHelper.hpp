/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: healthHelper.hpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2025/12/16 18:27:40 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2025/12/16 18:51:29                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2025 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

namespace	hel {

#define SET_UNHEALTHY(reason)	\
do {							\
	_healthy = false;			\
	_reason = reason;			\
} while (0);

#define RETURN_SET_UNHEALTHY(reason, ...)	\
do {										\
	_healthy = false;						\
	_reason = reason;						\
	return __VA_OPT__((__VA_ARGS__));		\
} while (0);

}
