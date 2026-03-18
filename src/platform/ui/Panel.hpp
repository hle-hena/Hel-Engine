/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Panel.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/16 10:19:48 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/18 17:14:54                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <string>

# include "utils/Expected.hpp"
# include "api/ImGui/imgui.h"

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

		virtual void	setup(Registry *registry, ImagePool *imagePool) final;
		virtual void	changeOwner(Dock *newOwner, size_t insertIdx = -1) final;
		virtual void	setOwner(Dock *newOwner) final;
		virtual expected<void, std::string>	onInit(void) { return {}; }

		virtual const char	*getLabel(void) const = 0;
		virtual Dock		*getOwner(void) const final { return (_owner); }
		virtual void	render(Window *window, const ImVec2 &size) = 0;

	protected:
		Registry	*_registry;
		ImagePool	*_imagePool;

		Dock		*_owner;
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
