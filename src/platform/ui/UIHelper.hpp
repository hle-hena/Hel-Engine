/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: UIHelper.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/03 11:48:20 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/20 20:45:11                                        */
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

#define	SETTER_INIT(name, type, member)	\
	auto	&set##name(type val)	{ member = {val}; return (*this); }

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
		SETTER(Normalizer, float, _normalizer)
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
		float		_normalizer{1.f};
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
		float		_min{-INFINITY};
		float		_max{+INFINITY};
};



class	Table {
	public:
		Table(const char *name);
		~Table(void);

		bool	newRow(const char *rowName, uint32_t nbCol);
		template <typename Func>
		void	setNextCell(const char *label, Func&& drawAction);

	private:
		bool	beginNewTable(void);
		void	endTable(void);

		const char	*_name;
		bool		_tableOpened{false};
		uint32_t	_nbTables{0};
		uint32_t	_nbCol{0};
};

class	TableRow {
	public:
		enum	Type { VecDrag, DragRange, SimpleText, InputText };

		TableRow(Table &table, Window *window, const char *rowName);

		SETTER(Type, Type, _type)
		SETTER_INIT(Speed, float, _speeds)
		SETTER_INIT(Min, float, _mins)
		SETTER_INIT(Max, float, _maxs)
		SETTER_INIT(Format, const char *, _fmts)
		SETTER_INIT(ValueName, const char *, _valueNames)
		SETTER(Speed, std::initializer_list<float>, _speeds)
		SETTER(Min, std::initializer_list<float>, _mins)
		SETTER(Max, std::initializer_list<float>, _maxs)
		SETTER(Format, std::initializer_list<const char *>, _fmts)
		SETTER(ValueName, std::initializer_list<const char *>, _valueNames)
		SETTER(Range, uint32_t, _range)
		SETTER(Start, float *, _startFloat)
		SETTER(Start, std::string *, _startString)
		bool	build(void);

	private:
		template <typename T>
		void	fillVec(std::vector<T> &vec, size_t wantedSize);

		bool	buildVecDrag(void);
		bool	buildDragRange(void);
		bool	buildSimpleText(void);
		bool	buildInputText(void);

		Table						&_table;
		Window						*_window;
		const char					*_rowName;
		Type						_type{VecDrag};
		std::vector<float>			_speeds{1.f};
		std::vector<float>			_mins{-INFINITY};
		std::vector<float>			_maxs{+INFINITY};
		std::vector<const char *>	_fmts{"%.3f"};
		std::vector<const char *>	_valueNames{nullptr};
		uint32_t					_range{1};
		float						*_startFloat{nullptr};
		std::string					*_startString{nullptr};
		

		using BuildFunc = bool (TableRow::*)();
		static const std::unordered_map<Type, BuildFunc>	_buildFunctions;
};

class	DropTarget {
	public:
		DropTarget(const char *type);

		SETTER(Size, const ImVec2 &, _size)
		SETTER(ResetPosition, bool, _setToEndPos)
		SETTER(Pos, const ImVec2 &, _startPos)
		DropTarget	&setEndPos(const ImVec2 &val);

		DropTarget	&addDummy(void);
		template <typename Func>
		void	build(Func &&dropAction);

	private:
		const char	*_type;
		ImVec2		_size;
		ImVec2		_startPos;
		ImVec2		_endPos;
		bool		_setToEndPos;
};

class	Button {
	public:
		Button(const char *label);

		SETTER(Size, ImVec2, _size)
		SETTER(EndPos, ImVec2, _endPos)
		SETTER(Pos, ImVec2, _pos)

		Button	&showOnHover(bool parentHover);

		bool	build(void);

	private:
		const char				*_label;
		ImVec2					_pos;
		std::optional<ImVec2>	_endPos;
		ImVec2					_size;
		bool					_hide;
};

class	Knob {
	public:
		Knob(float *val) : _val{val} {};

		SETTER(Label, const char *, _label)
		SETTER(Min, float, _min)
		SETTER(Max, float, _max)
		SETTER(Width, float, _radius)
		SETTER(Thickness, float, _thickness)
		bool	build(void);

	private:
		float		*_val;
		const char	*_label{"##knob"};
		float		_min{-INFINITY};
		float		_max{+INFINITY};
		float		_radius{10.f};
		float		_thickness{4.f};
};

class	ColoredDummy {
	public:
		ColoredDummy(void);

		SETTER(Size, ImVec2, _size)
		SETTER(EndPos, ImVec2, _endPos)
		SETTER(Pos, ImVec2, _pos)

		void	build(void);

	private:
		ImVec2	_pos;
		ImVec2	_endPos;
		ImVec2	_size;
};

}

#include "platform/ui/UIHelper.tpp"
