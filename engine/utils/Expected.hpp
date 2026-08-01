/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Expected.hpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/13 16:38:21 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/08/01 13:18:13                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <string>
#include <format>

namespace	hel {

template <typename UnexpectedT>
struct unexpected_wrapper : public UnexpectedT {
	using UnexpectedT::UnexpectedT;

	template <typename... Args>
	explicit unexpected_wrapper(std::format_string<Args...> fmt, Args&&... args)
		: UnexpectedT(std::format(fmt, std::forward<Args>(args)...)) {}
};

}

#if __cplusplus >= 202302L && __has_include(<expected>)
	#include <expected>
	namespace hel {
		template <typename T>
		using expected = std::expected<T, std::string>;
		using unexpected = unexpected_wrapper<std::unexpected<std::string>>;
	}
#else
	#include <utils/expected.hpp>
	namespace hel {
		template <typename T>
		using expected = tl::expected<T, std::string>;
		using unexpected = unexpected_wrapper<tl::unexpected<std::string>>;
	}
#endif
