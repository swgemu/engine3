/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DISTRIBUTEDMETHOD_H_
#define DISTRIBUTEDMETHOD_H_

#include "system/lang.h"

namespace engine {
  namespace ORB {

	class DistributedObjectBroker;
	class DistributedObject;
	
	class DistributedMethod {
		DistributedObject* object;
		
		sys::uint32 methodID;
		sys::uint32 invocationID;
		
		DistributedObjectBroker* orb;
		Packet* invmsg;
		
	public:
		DistributedMethod(DistributedObject* obj, sys::uint32 methid);
		DistributedMethod(DistributedObjectBroker* broker, Packet* pack);
		
		// exeuctor methods
		void executeWithVoidReturn();

		bool executeWithBooleanReturn();
		
		char executeWithSignedCharReturn();
		unsigned char executeWithUnsignedCharReturn();

		byte executeWithByteReturn();

		int executeWithSignedIntReturn();
		unsigned int executeWithUnsignedIntReturn();
		
		long long executeWithSignedLongReturn();
		unsigned long long executeWithUnsignedLongReturn();

		float executeWithFloatReturn();

		void executeWithAsciiReturn(string& value);
		void executeWithUnicodeReturn(unicode& value);
		
		DistributedObject* executeWithObjectReturn();
		
		// parameter insertion methods
		void addBooleanParameter(bool val);

		void addByteParameter(byte val);

		void addSignedCharParameter(char val);
		void addUnsignedCharParameter(unsigned char val);

		void addSignedIntParameter(int val);
		void addUnsignedIntParameter(unsigned int val);
		
		void addSignedLongParameter(long long val);
		void addUnsignedLongParameter(unsigned long long val);

		void addFloatParameter(float val);

		void addAsciiParameter(const string& ascii);
		void addUnicodeParameter(const unicode& str);

		void addObjectParameter(DistributedObject* obj);
	
		// parameter reader methods
		bool getBooleanParameter();

		byte getByteParameter();

		int getSignedIntParameter();
		unsigned int getUnsignedIntParameter();

		char getSignedCharParameter();
		unsigned char getUnsignedCharParameter();
		
		long long getSignedLongParameter();
		unsigned long long getUnsignedLongParameter();

		float getFloatParameter();

		string& getAsciiParameter(string& ascii);
		unicode& getUnicodeParameter(unicode& str);
		
		DistributedObject* getObjectParameter();
	
	private:
		void execute(Packet* response);
		
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDMETHOD_H_*/
