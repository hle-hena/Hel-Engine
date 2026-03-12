/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Setters.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/06 21:22:56 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/12 13:42:51                                        */
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
