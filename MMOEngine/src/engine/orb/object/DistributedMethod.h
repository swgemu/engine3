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

	class InvokeMethodMessage;

	class DistributedMethod {
		const DistributedObject* object;

		sys::uint32 methodID;
		sys::uint32 invocationID;

		DistributedObjectBroker* orb;

		InvokeMethodMessage* invocationMessgage;
		Packet* response;

	public:
		DistributedMethod(const DistributedObject* obj, sys::uint32 methid);
		DistributedMethod(DistributedObjectBroker* broker, InvokeMethodMessage* invmsg);

		~DistributedMethod();

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
		double executeWithDoubleReturn();

		short executeWithSignedShortReturn();
		unsigned short executeWithUnsignedShortReturn();

		void executeWithAsciiReturn(String& value);
		void executeWithUnicodeReturn(UnicodeString& value);

		DistributedObject* executeWithObjectReturn();

		// parameter insertion methods
		void addBooleanParameter(bool val);

		void addByteParameter(byte val);

		void addSignedCharParameter(char val);
		void addUnsignedCharParameter(unsigned char val);

		void addSignedShortParameter(short val);
		void addUnsignedShortParameter(unsigned short val);

		void addSignedIntParameter(int val);
		void addUnsignedIntParameter(unsigned int val);

		void addSignedLongParameter(long long val);
		void addUnsignedLongParameter(unsigned long long val);

		void addFloatParameter(float val);
		void addDoubleParameter(double val);

		void addAsciiParameter(const String& ascii);
		void addUnicodeParameter(const UnicodeString& str);

		void addObjectParameter(DistributedObject* obj);

		// parameter reader methods
		bool getBooleanParameter();

		byte getByteParameter();

		int getSignedIntParameter();
		unsigned int getUnsignedIntParameter();

		short getSignedShortParameter();
		unsigned short getUnsignedShortParameter();

		char getSignedCharParameter();
		unsigned char getUnsignedCharParameter();

		long long getSignedLongParameter();
		unsigned long long getUnsignedLongParameter();

		float getFloatParameter();
		double getDoubleParameter();

		String& getAsciiParameter(String& ascii);
		UnicodeString& getUnicodeParameter(UnicodeString& str);

		DistributedObject* getObjectParameter();

		const DistributedObject* getObject() const {
			return object;
		}

		InvokeMethodMessage* getInvocationMessage() const {
			return invocationMessgage;
		}

		Packet* getResponseMessage() const {
			return response;
		}

	private:
		void execute();

	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDMETHOD_H_*/
