/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Setters.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/06 21:22:56 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/03/11 15:39:49                                        */
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
