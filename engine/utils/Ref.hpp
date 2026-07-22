/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Ref.hpp                                                             */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/13 11:45:51 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/22 14:03:56                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <atomic>
#include <cstdint>
#include <cstddef>
#include <utility>

namespace	hel {

template <typename T>
class	Ref;

class	RefCounted {
	public:
		RefCounted(void) = default;
		RefCounted(const RefCounted&) = delete;
		RefCounted	&operator=(const RefCounted&) = delete;

		uint32_t	refCount() const noexcept {
			return _refCount.load(std::memory_order_relaxed);
		}

	protected:
		virtual ~RefCounted() = default;

	private:
		template <typename T>
		friend class	Ref;

		void	addRef(void) const noexcept {
			_refCount.fetch_add(1, std::memory_order_relaxed);
		}

		void	release(void) const noexcept {
			if (_refCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
				delete this;
			}
		}

		mutable std::atomic<uint32_t>	_refCount{0};
};

template <typename T>
class	Ref {
	public:
		Ref(void) noexcept = default;
		Ref(std::nullptr_t) noexcept {}

		Ref(T *p) noexcept : _ptr(p)
			{ if (_ptr) _ptr->addRef(); }

		Ref(const Ref &other) noexcept : _ptr(other._ptr)
			{ if (_ptr) _ptr->addRef(); }

		template <typename U>
		Ref(const Ref<U> &other) noexcept : _ptr(other.get())
			{ if (_ptr) _ptr->addRef(); }

		Ref(Ref &&other) noexcept : _ptr(other._ptr)
			{ other._ptr = nullptr; }

		template <typename U>
		Ref(Ref<U> &&other) noexcept : _ptr(other._ptr)
			{ other._ptr = nullptr; }

		~Ref()
			{ if (_ptr) _ptr->release(); }

		Ref	&operator=(Ref other) noexcept {
			std::swap(_ptr, other._ptr);
			return *this;
		}

		void	reset(void) noexcept {
			if (_ptr) _ptr->release();
			_ptr = nullptr;
		}
		uint32_t	refCount(void) noexcept { return _ptr->refCount(); }

		T	*get() const noexcept { return _ptr; }
		T	*operator->() const noexcept { return _ptr; }
		T	&operator*() const noexcept { return *_ptr; }

		explicit operator bool() const noexcept { return _ptr != nullptr; }

		friend bool operator==(const Ref& a, const Ref& b) noexcept { return a._ptr == b._ptr; }
		friend bool operator!=(const Ref& a, const Ref& b) noexcept { return a._ptr != b._ptr; }
		friend bool operator==(const Ref& a, std::nullptr_t) noexcept { return a._ptr == nullptr; }
		friend bool operator!=(const Ref& a, std::nullptr_t) noexcept { return a._ptr != nullptr; }

	private:
		template <typename U> friend class Ref;
		T	*_ptr = nullptr;
};

template <typename T, typename... Args>
Ref<T>	makeRef(Args&&... args) {
	return Ref<T>(new T(std::forward<Args>(args)...));
}

}

namespace	std {

template <typename T>
struct	hash<hel::Ref<T>> {
	size_t	operator()(const hel::Ref<T>& r) const noexcept {
		return std::hash<T*>()(r.get());
	}
};

}
