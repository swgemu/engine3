/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DISTRIBUTEDOBJECT_H_
#define DISTRIBUTEDOBJECT_H_

#include "system/lang.h"

namespace engine {
  namespace ORB {

	class NamingDirectoryService;
	
	class DistributedObjectAdapter;
	class DistributedObjectStub;
	
	class DistributedObject : public ReadWriteLock, public Object {
	protected:
		string _className;
		string _name;
		
		uint64 _objectID;
		
		NamingDirectoryService* _namingDirectory;
			
	public:
		DistributedObject();
		
		virtual ~DistributedObject();
	
		// setters
		inline void _setClassName(const string& n) {
			_className = n;
		}
		
		inline void _setName(const string& n) {
			_name = n;
		}
	
		inline void _setObjectID(uint64 id) {
			_objectID = id;	
		}
	
		inline void _setNamingDirectory(NamingDirectoryService* service) {
			_namingDirectory = service;
		}
	
		// getters
		inline const string& _getClassName() {
			return _className;
		}
	
		inline const string& _getName() {
			return _name;
		}	
		
		inline uint64 _getObjectID() {
			return _objectID;
		}
		
		inline NamingDirectoryService* _getNamingDirectory() {
			return _namingDirectory;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECT_H_*/
