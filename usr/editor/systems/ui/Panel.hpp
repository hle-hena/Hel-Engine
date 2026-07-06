/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Panel.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/16 10:19:48 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/06 10:37:42                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <limits>
#include <ui/ImGui/imgui.h>

#include "HelExpected.hpp"

namespace	hel {

class	Window;
class	Registry;
class	ImagePool;

}

namespace	hel::sys {

class	Dock;

class	IPanel {
	public:
		IPanel(void) = default;
		virtual ~IPanel(void) = default;

		virtual expected<void>	setup(Registry *registry,
												ImagePool *imagePool) final;
		virtual void	shouldClose(bool closing) final
							{ _shouldClose = closing; }
		virtual bool	shouldClose(void) const final
							{ return (_shouldClose); }

		virtual void	changeOwner(Dock *newOwner, size_t insertIdx =
							std::numeric_limits<size_t>::max()) final;
		virtual void	setOwner(Dock *newOwner) final;
		virtual expected<void>	onInit(void) { return {}; }

		virtual const char	*getLabel(void) const = 0;
		virtual Dock		*getOwner(void) const final { return (_owner); }
		virtual void		render(Window *window, const ImVec2 &size) = 0;

	protected:
		Registry	*_registry;
		ImagePool	*_imagePool;

		Dock		*_owner;
		bool		_shouldClose{false};
};

template <typename Derived>
class	Panel : public IPanel {
	public:
		const char	*getLabel() const override {
			if constexpr (requires { Derived::label; })
				return Derived::label;
			else
				return typeid(Derived).name();
		}
};

}
