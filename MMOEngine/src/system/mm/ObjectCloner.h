/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
 */

#ifndef OBJECTCLONER_H_
#define OBJECTCLONER_H_

namespace sys {
  namespace mm {

	template <class O> class ObjectCloner {
	public:
		static Object* clone(O* object) {
			return dynamic_cast<Object*>(new O(*object));
		}
	};

  } // namespace mm
} // namespace sys

#endif /* OBJECTCLONER_H_ */
