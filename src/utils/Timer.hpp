/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Timer.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/17 16:36:44 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/17 17:11:17                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <chrono>

namespace	hel {

class	Timer {
	public:
		void	start(void)	 {_startTime = clock::now(); }

		template <typename Ratio = std::ratio<1>>
		float	elapsedTime(void) {
			return (std::chrono::duration<float, Ratio>(clock::now() - _startTime).count());
		}

	private:
		using clock = std::chrono::steady_clock;
		std::chrono::time_point<clock>	_startTime;
};

}
