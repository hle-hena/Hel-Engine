/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Timer.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/17 16:36:44 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/18 11:21:35                                        */
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
		using clock = std::chrono::steady_clock;

		void	start(void)	{	_startTime = clock::now();
								_lapStartTime = _startTime;	}

		void	lap(void)	{	_lapStartTime = clock::now();	}

		template <typename Ratio = std::ratio<1>>
		float	elapsedTime(void) {
			return (std::chrono::duration<float, Ratio>(clock::now() - _startTime).count());
		}

		template <typename Ratio = std::ratio<1>>
		float	lapTime(void) {
			return (std::chrono::duration<float, Ratio>(clock::now() - _lapStartTime).count());
		}

	private:
		std::chrono::time_point<clock>	_startTime;
		std::chrono::time_point<clock>	_lapStartTime;
};

}
