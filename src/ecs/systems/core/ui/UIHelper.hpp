/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UIHelper.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/03 11:48:20 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/05 12:59:29                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# define GLFW_INCLUDE_VULKAN
# include <GLFW/glfw3.h>
# include "api/ImGui/imgui.h"

# include <string>

#define SETTER(type, name, member) \
    type& set##name(auto val) { member = val; return *this; }

namespace	hel::sys {

class	Splitter {
	public:
		enum	Dir { Left, Right, Up, Down };

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
		static constexpr bool	isHorizontal(Dir a) { 
			return (static_cast<int>(a) & 2); 
		}
		
		static constexpr float	IsPositive(Dir a) { 
			return ((static_cast<int>(a) & 1) ? 1.f : -1.f); 
		}

		std::string	_id;
		ImVec2		_pos{0.f, 0.f};
		ImVec2		_limits{0.f, 0.f};
		Dir			_dir{Right};
		float		*_updateVal{nullptr};
		float		_size{0.f};
		float		_hitBox{6.f};
};

class	DragFloat {
	public:
		DragFloat(GLFWwindow *windowPtr, float *val);

		SETTER(DragFloat, Label, _label)
		SETTER(DragFloat, Format, _format)
		SETTER(DragFloat, Speed, _speed)
		SETTER(DragFloat, Min, _min)
		SETTER(DragFloat, Max, _max)
		bool	build(void);

	private:
		GLFWwindow	*_windowPtr;
		float		*_val;
		const char	*_label{"##v"};
		const char	*_format{"%.3f"};
		float		_speed{1.f};
		float		_min{+(1./0.)};
		float		_max{-(1./0.)};
};

}
