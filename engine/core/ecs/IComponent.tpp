/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: IComponent.tpp                                                      */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/30 18:33:48 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/08 13:12:19                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/IComponent.hpp"
#include "ecs/Pool.hpp"

namespace	hel {

template <ValidComponent Component>
Component::POD	*OpaqueComponentHandle::get(void) {
	return static_cast<Component::POD *>(_pool->getRaw(_index.value()));
}

template <ValidComponent Comp>
ComponentHandle<Comp>::operator bool(void) const {
	return (_index.has_value());
}

template <ValidComponent Comp>
const typename Comp::POD	*ComponentHandle<Comp>::operator->(void) {
	return (&_pool->components[*_index]);
}

template <ValidComponent Comp>
uint32_t	ComponentHandle<Comp>::getDenseIndex(void) const {
	return (_index.value());
}

template <ValidComponent Comp>
ComponentHandle<Comp>::ModificationProxy	ComponentHandle<Comp>::modify(void)
{
	return ModificationProxy(*this);
}

template <ValidComponent Comp>
bool	ComponentHandle<Comp>::isDirty(void) {
	return _pool->isDirty(_index.value());
}

template <ValidComponent Comp>
ComponentHandle<Comp>::ModificationProxy
::ModificationProxy(const ComponentHandle<Comp> &base)
	: ComponentHandle<Comp>(base) {}

template <ValidComponent Comp>
ComponentHandle<Comp>::ModificationProxy
::~ModificationProxy(void) {
	if (!_dismissed && _index.has_value())
		_pool->markDirty(_index.value());
}

template <ValidComponent Comp>
typename Comp::POD	*ComponentHandle<Comp>::ModificationProxy::operator->(void) {
	return (&_pool->components[*_index]);
}

template <ValidComponent Comp>
void	ComponentHandle<Comp>::ModificationProxy::dismiss(void) {
	_dismissed = true;
}

}
