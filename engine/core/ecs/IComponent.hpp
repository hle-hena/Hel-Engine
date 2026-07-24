/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: IComponent.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/30 10:44:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 14:30:19                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "core/ecs/ValidComponent.hpp"

#include <string_view>
#include <optional>
#include <cstdint>

namespace	hel {

template <ValidComponent Comp>
struct	Pool;
struct	IPool;

template <typename Derived>
struct	IComponent {
	struct	POD {};
	struct	GPULayout {};
	struct	MetaData {
		static constexpr std::string_view	label = "Unnamed Component";
		static constexpr bool				gpuVisible = false;
	};
};

struct	OpaqueComponentHandle {
	private:
		IPool					*_pool{nullptr};
		std::optional<uint32_t>	_index;
		bool					_dismissed{false};

	public:
		~OpaqueComponentHandle(void);

		operator 					bool(void) const;
		template <ValidComponent Component>
		Component::POD				*get(void);
		void						*getRaw(void);
		void						dismiss(void);
		bool						isDirty(void);
		std::string_view			typeName(void);

	template <ValidComponent Comp>
	friend struct Pool;
	template <typename Include, typename Exclude>
	friend class View;
};

template <ValidComponent Component>
struct	ComponentHandle {
	protected:
		struct ModificationProxy;
		Pool<Component>			*_pool{nullptr};
		std::optional<uint32_t>	_index;

	public:
		operator 					bool(void) const;
		const Component::POD		*operator->(void);
		uint32_t					getDenseIndex(void) const;
		ModificationProxy			modify(void);
		bool						isDirty(void);

	friend class	Registry;
	template <typename Include, typename Exclude>
	friend class View;
};

template <ValidComponent Component>
struct	ComponentHandle<Component>::ModificationProxy: ComponentHandle<Component> {
	private:
		bool	_dismissed{false};

	public:
		ModificationProxy(const ComponentHandle<Component> &base);
		~ModificationProxy(void);

		Component::POD	*operator->(void);
		void			dismiss(void);
};

}

#include "core/ecs/IComponent.tpp"
