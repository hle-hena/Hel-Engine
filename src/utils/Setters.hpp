/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Setters.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/06 21:22:56 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/09 09:11:24                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#define	SETTER(name, type, member)	\
	auto	&set##name(type val)	{ member = val; return (*this); }

#define	SETTER_VERBOSE(name, type, member)	\
	auto	&name(type val)	{ member = val; return (*this); }

#define PROXY_SETTER(name, type, member, parentName)	\
	auto	&name(type val)	{ member = val; return (parentName); }

#define	SETTER_INIT(name, type, member)	\
	auto	&set##name(type val)	{ member = {val}; return (*this); }

#define SETTER_OR(name, type, member)	\
	auto	&set##name(type val)	{ member |= val; return (*this); }

#define PASSKEY(keyName, friendClass)	\
	struct keyName {					\
		private:						\
			keyName(void) = default;	\
			friend class friendClass;	\
	};
