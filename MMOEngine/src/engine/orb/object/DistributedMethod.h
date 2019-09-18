/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DISTRIBUTEDMETHOD_H_
#define DISTRIBUTEDMETHOD_H_

#include "system/lang.h"

#include "engine/orb/messages/InvokeMethodMessage.h"

namespace engine {
  namespace ORB {

	class DistributedObjectBroker;
	class DistributedObject;

	class DistributedMethod {
		const DistributedObject* object = nullptr;

		sys::uint32 methodID = 0;
		sys::uint32 invocationID = 0;

		DistributedObjectBroker* orb = nullptr;

		InvokeMethodMessage* invocationMessage = nullptr;
		Packet* response = nullptr;

	public:
		DistributedMethod(const DistributedObject* obj, sys::uint32 methid, bool async = false);
		DistributedMethod(DistributedObjectBroker* broker, InvokeMethodMessage* invmsg);

		~DistributedMethod();

		// exeuctor methods
		void executeWithVoidReturn(bool async = false);

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

		template<typename T>
		void addDereferencedSerializableParameter(const T& array) {
			TypeInfo<T>::toBinaryStream(&(const_cast<T&>(array)), invocationMessage);
		}

		template<typename T>
		void addDereferencedSerializableParameter(T& array) {
			TypeInfo<T>::toBinaryStream(&array, invocationMessage);
		}

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

		template<typename T>
		T getDereferencedSerializableParameter() {
			T object;

			TypeInfo<T>::parseFromBinaryStream(&object, invocationMessage->getIncomingPacket());

			return object;
		}

		const DistributedObject* getObject() const {
			return object;
		}

		InvokeMethodMessage* getInvocationMessage() const {
			return invocationMessage;
		}

		Packet* getResponseMessage() const {
			return response;
		}

	private:
		void execute(bool asyncMethod = false);

	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDMETHOD_H_*/
