/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: EntityReference.hpp                                                 */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/24 18:44:55 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/24 19:10:00                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include "core/HelSystem.hpp"

namespace	hel::sys {
	
class EntityReference {
	public:
		static void			setReferenced(Entity::id id)
			{ _reference = id; }
		static Entity::id	getReferenced()
			{ return _reference; }

	private:
		static Entity::id	_reference;
};

}