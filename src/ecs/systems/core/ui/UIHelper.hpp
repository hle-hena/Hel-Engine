/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UIHelper.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/03 11:48:20 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/05 16:29:07                                        */
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
# include <string>
# include <math.h>

# include "api/ImGui/imgui.h"
# include "platform/window/Window.hpp"

#define	SETTER(name, type, member)	\
	auto	&set##name(type val)	{ member = val; return (*this); }

namespace	hel::sys {

class	Splitter {
	public:
		enum	Dir { Left, Right, Up, Down };

		Splitter(float *val);

		SETTER(Size, float, _size)
		SETTER(Hitbox, float, _hitbox)
		SETTER(Min, float, _min)
		SETTER(Max, float, _max)
		SETTER(Dir, Dir, _dir)
		SETTER(Label, const char *, _label)
		SETTER(Pos, ImVec2, _pos)
		void	build(void);

	private:
		static constexpr bool	isHorizontal(Dir a) { 
			return (static_cast<int>(a) & 2); 
		}
		
		static constexpr float	IsPositive(Dir a) { 
			return ((static_cast<int>(a) & 1) ? 1.f : -1.f); 
		}

		float		*_val;
		const char	*_label{"##splitter"};
		ImVec2		_pos{0.f, 0.f};
		Dir			_dir{Right};
		float		_min{0.f};
		float		_max{100.f};
		float		_size{0.f};
		float		_hitbox{6.f};
};



class	DragFloat {
	public:
		DragFloat(GLFWwindow *windowPtr, float *val);

		SETTER(Label, const char *, _label)
		SETTER(Format, const char *, _format)
		SETTER(Speed, float, _speed)
		SETTER(Min, float, _min)
		SETTER(Max, float, _max)
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



class	Table {
	public:
		Table(const char *name);

		bool	begin(uint32_t col);
		void	end(void);

		void	newRow(const char *rowName);
		template <typename Func>
		void	setNextCell(const char *label, Func&& drawAction);

	private:
		const char	*_name;
};

class	TableRow {
	public:
		enum	Type { VecDrag, DragRange };

		TableRow(Table &table, Window *window, const char *rowName);

		SETTER(Type, Type, _type)
		SETTER(Speed, float, _speed)
		SETTER(Min, float, _min)
		SETTER(Max, float, _max)
		SETTER(Range, uint32_t, _range)
		SETTER(Start, float *, _start)
		SETTER(ValueNames, std::initializer_list<const char *>, _valueNames)
		bool	build(void);

	private:
		bool	buildVecDrag(void);

		Table						&_table;
		Window						*_window;
		const char					*_rowName;
		Type						_type{VecDrag};
		float						_speed{1.f};
		float						_min{-INFINITY};
		float						_max{+INFINITY};
		uint32_t					_range{1};
		float						*_start{nullptr};
		std::vector<const char *>	_valueNames{};

		using BuildFunc = bool (TableRow::*)();
		static const std::unordered_map<Type, BuildFunc>	_buildFunctions;
};

}

#include "ecs/systems/core/ui/UIHelper.tpp"
