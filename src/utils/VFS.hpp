/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: VFS.hpp                                                             */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/04/27 22:19:38 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/27 23:35:30                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <string>
#include "core/Engine.hpp"
#include <unordered_map>

namespace	hel {

class	VFS {
	public:
		PASSKEY(EngineKey, Engine)
		static void			load(EngineKey);

		static std::string	getFilepath(std::string path);

	private:
		VFS(void);

		static std::string	normalizePath(std::string path);

		static std::unordered_map<std::string, std::string>	_pathCache;
};

}
