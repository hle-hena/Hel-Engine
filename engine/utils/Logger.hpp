/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Logger.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/21 18:35:58 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/22 14:23:32                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <cstdio>
#include <cstdlib>
#include <format>
#include <source_location>
#include <string_view>

namespace	hel {

enum class	LogLevel {
	Message,
	Warning,
	Error,
	Fatal,
};

struct Log {
	std::string_view     fmt;
	std::source_location loc;

	template <typename T>
	consteval Log(const T &f,
				  std::source_location l = std::source_location::current())
		: fmt(f), loc(l)
	{}
};

struct	Logger {
	static void	message(Log text)
	{
		log(LogLevel::Message, text.fmt, text.loc);
	}

	static void	error(Log text)
	{
		log(LogLevel::Error, text.fmt, text.loc);
	}

	static void	fatal(Log text)
	{
		log(LogLevel::Fatal, text.fmt, text.loc);
	}

	template <typename... Args>
	static void	messagef(Log text, Args &&...args)
	{
		log(LogLevel::Message,
			std::vformat(text.fmt, std::make_format_args(args...)),
			text.loc);
	}

	template <typename... Args>
	static void	errorf(Log text, Args &&...args)
	{
		log(LogLevel::Error,
			std::vformat(text.fmt, std::make_format_args(args...)),
			text.loc);
	}

	template <typename... Args>
	static void	fatalf(Log text, Args &&...args)
	{
		log(LogLevel::Fatal,
			std::vformat(text.fmt, std::make_format_args(args...)),
			text.loc);
	}


private:
	static void	log(LogLevel level, std::string_view text,
					const std::source_location &loc)
	{
		const char	*prefix = nullptr;
		FILE		*stream = stdout;

		switch (level) {
			case LogLevel::Message:	prefix = "[MSG]   "; stream = stdout; break;
			case LogLevel::Warning:	prefix = "[WARN]  "; stream = stderr; break;
			case LogLevel::Error:	prefix = "[ERR]   "; stream = stderr; break;
			case LogLevel::Fatal:	prefix = "[FATAL] "; stream = stderr; break;
		}

		std::fprintf(stream, "%s %.*s (%s:%u in '%s')\n",
					prefix,
					static_cast<int>(text.size()), text.data(),
					loc.file_name(), loc.line(), loc.function_name());
	}
};


#define HEL_FATAL(...)				\
do {								\
	Logger::fatalf(__VA_ARGS__);	\
	std::abort();					\
} while (0)

#define HEL_ERROR(...)				\
do {								\
	Logger::errorf(__VA_ARGS__);	\
} while (0)

}
