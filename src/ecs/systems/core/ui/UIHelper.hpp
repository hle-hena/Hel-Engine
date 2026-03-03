/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UIHelper.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/03 11:48:20 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/03 13:29:12                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include "api/ImGui/imgui.h"

# include <string>

namespace	hel::sys {

class	Splitter {
	public:
		enum	Dir { Vertical, Horizontal };

		Splitter(void) = default;
		~Splitter(void) = default;

		Splitter	&setSize(float size);
		Splitter	&setHitBox(float size);
		Splitter	&setPos(float x, float y);
		Splitter	&setLimits(float minSize, float maxSize);
		Splitter	&setDir(Dir dir);
		Splitter	&setVal(float *val);
		Splitter	&setId(const std::string &id);
		void		build(void);

	private:
		std::string	_id;
		ImVec2		_pos{0.f, 0.f};
		ImVec2		_limits{0.f, 0.f};
		Dir			_dir{Vertical};
		float		*_updateVal{nullptr};
		float		_size{0.f};
		float		_hitBox{4.f};
};

}
