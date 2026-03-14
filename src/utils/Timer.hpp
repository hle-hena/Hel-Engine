/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Timer.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/17 16:36:44 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/13 22:13:23                                        */
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

		template <typename Ratio = std::ratio<1>>
		float	lap(void) {
			auto	now = clock::now();
			float	dt = std::chrono::duration<float, Ratio>(now - _lapStartTime).count();
			_lapStartTime = now;
			return (dt);
		}

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
