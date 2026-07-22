/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Texture.hpp                                                         */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/24 15:06:28 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/22 14:28:40                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <memory>
#include <string>

#include "utils/Ref.hpp"

namespace	hel {

class	Device;
class	Image;

struct	Texture {
	std::string	filePath;
	Ref<Image>	image;

	static std::shared_ptr<Texture>	load(Device *device,
											const std::string &path);

	protected:
		struct	RawTexture {
			unsigned char	*pixels{nullptr};
			int				width, height, channels;
		};

		static RawTexture	loadFile(const std::string &path);
};

}
