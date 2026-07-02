/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: IComponent.hpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/30 10:44:13 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/02 14:49:03                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <string_view>
#include <functional>
#include <optional>
#include <cstdint>

namespace	hel {

template <typename T>
concept	HasMetaData = requires {
	{ T::MetaData::label }		-> std::convertible_to<std::string_view>;
	{ T::MetaData::gpuVisible }	-> std::convertible_to<bool>;
};

template <typename T>
concept HasPOD = std::is_aggregate_v<typename T::POD>
	&& !std::is_polymorphic_v<typename T::POD>;

template <typename T>
concept IsFlatData = std::is_standard_layout_v<T> && std::is_trivial_v<T>;

template <typename T>
concept ValidateGPULayout = requires {
	requires (!T::MetaData::gpuVisible || (
		T::MetaData::gpuVisible && requires	{
			{ T::MetaData::toGPU }	-> std::convertible_to<
										std::function<typename T::GPULayout
											(const typename T::POD &)>>;
		})
	);
};

template <typename T>
concept ValidComponent = HasMetaData<T> && HasPOD<T> && ValidateGPULayout<T>;



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

	public:
		~OpaqueComponentHandle(void);

		operator 					bool(void) const;
		template <ValidComponent Component>
		const Component::POD		*get(void);
		bool						isDirty(void);

	friend class	Registry;
	template <ValidComponent Include, ValidComponent Exclude>
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
	template <ValidComponent Include, ValidComponent Exclude>
	friend class View;
};

template <ValidComponent Component>
struct	ComponentHandle<Component>::ModificationProxy: ComponentHandle<Component> {
	ModificationProxy(const ComponentHandle<Component> &base);
	~ModificationProxy(void);

	Component::POD	*operator->(void);
};

}

#include "ecs/IComponent.tpp"
