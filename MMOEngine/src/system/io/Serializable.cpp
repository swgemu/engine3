/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "Serializable.h"

#include "../lang/StringBuffer.h"
#include "StringTokenizer.h"

#include "../lang.h"

#include "ObjectOutputStream.h"
#include "ObjectInputStream.h"

//VectorMap<uint32, String> Serializable::variableNames;
//ReadWriteLock Serializable::variableNameMutex;

Serializable::Serializable() : Object() {
	variables.setInsertPlan(SortedVector<VectorMapEntry<String, Variable*>*>::NO_DUPLICATE);
	variables.setNullValue(NULL);
}

void Serializable::serialize(String& str) {
	StringBuffer buffer;

	buffer << "{" << "size=" << variables.size();

	for (int i = 0; i < variables.size(); ++i) {
		VectorMapEntry<String, Variable*>* entry = variables.SortedVector<VectorMapEntry<String, Variable*>*>::get(i);

		String nameAndVersion = entry->getKey();
		Variable* variable = entry->getValue();

		String value;
		variable->toString(value);

		buffer << "," << nameAndVersion << "=" << value;
	}

	buffer << "}";

	buffer.toString(str);
}

void Serializable::serialize(ObjectOutputStream* stream) {
	int size = variables.size();

	stream->writeInt(size);

	for (int i = 0; i < size; ++i) {
		Variable* variable = variables.get(i);

		variable->toBinaryStream(stream);
	}
}

void Serializable::deSerialize(ObjectInputStream* stream) {
	int size = variables.size();

	int dataSize = stream->readInt();

	if (size != dataSize) {
		System::out << "WARNING data size and variables not equal in void Serializable::deSerialize(ObjectInputStream* stream)\n";

		return;
	}

	for (int i = 0; i < size; ++i) {
		Variable* variable = variables.get(i);

		variable->parseFromBinaryStream(stream);
	}
}

void Serializable::deSerialize(const String& str) {
	String data;
	try {
		if (!getObjectData(str, data))
			return;

		int comma = data.indexOf("=");

		int variableSize = Integer::valueOf(data.subString(1, comma));

		comma = data.indexOf(",");

		for (int i = 0; i < variableSize; ++i) {
			data = data.subString(comma + 1);

			int equal = data.indexOf("=");

			String variableName = data.subString(0, equal);
			String variableData;

			if (data.subString(equal + 1, equal + 2).indexOf("{") != -1) {
				int lastSemiColon = getObjectData(data, variableData);

				comma = lastSemiColon;
			} else {
				comma = data.indexOf(",");
				variableData = data.subString(equal + 1, comma);
			}

			deSerializeVariable(variableName, variableData);
		}

	} catch (Exception& e) {
		System::out << e.getMessage();
		e.printStackTrace();
	} catch (...) {
		System::out << "Unreported exception caught in void Serializable::deSerialize(const String& str)\n";
	}
}

void Serializable::deSerializeVariable(const String& nameAndVersion, const String& varData) {
	Variable* variable = variables.get(nameAndVersion);

	if (variable == NULL) {
		System::out << "WARNING: variable " << nameAndVersion << " not found when deserializing [" << varData << "] \n";

		return;
	}

	int hasVersion = nameAndVersion.indexOf("|");

	if (hasVersion != -1) {
		int version = Integer::valueOf(nameAndVersion.subString(hasVersion + 1));

		variable->parseFromString(varData, version);
	} else {
		variable->parseFromString(varData);
	}
}

void Serializable::addSerializableVariable(const String& nameAndVersion, Variable* variable) {
	variables.put(nameAndVersion, variable);
}

Variable* Serializable::getSerializableVariable(const String& nameAndVersion) {
	return variables.get(nameAndVersion);
}

int Serializable::getObjectData(const String& str, String& obj) {
	int opening = str.indexOf("{");

	if (opening == -1)
		return 0;

	int i, subObjects = 0;
	bool subString = false;


	for (i = opening + 1; i < str.length(); ++i) {
		char opening = str.charAt(i);

		if (subString && opening == '\\') {
			++i;
			continue;
		}

		if (opening == '"') {
			subString = !subString;
			continue;
		}

		if (!subString) {
			if (opening == '{') {
				++subObjects;
			} else if (opening == '}') {
				--subObjects;
			}
		}

		if (subObjects < 0) {
			break;
		}
	}

	if (subObjects < 0) {
		obj = str.subString(opening, i + 1);

		//System::out << obj << "\n";

		return i + 1;
	} else
		System::out << "WARNING expecting \"}\" in " << str << " at " << i;

	return 0;
}
