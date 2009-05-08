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
	variables.setInsertPlan(SortedVector<VectorMapEntry<VariableName, Variable*>*>::NO_DUPLICATE);
	variables.setNullValue(NULL);
}

void Serializable::serialize(String& str) {
	StringBuffer buffer;

	buffer << "{" << "size=" << variables.size();

	for (int i = 0; i < variables.size(); ++i) {
		VectorMapEntry<VariableName, Variable*>* entry = variables.SortedVector<VectorMapEntry<VariableName, Variable*>*>::get(i);

		VariableName varName = entry->getKey();
		Variable* variable = entry->getValue();

		int version = varName.getVersion();
		String name = varName.getName();

		if (version != 0) {
			StringBuffer nameAndVersion;
			nameAndVersion << name << "[" << version << "]";
			nameAndVersion.toString(name);
		}

		String value;
		variable->toString(value);

		buffer << "," << name << "=" << value;
	}

	buffer << "}";

	buffer.toString(str);
}

void Serializable::serialize(ObjectOutputStream* stream) {
	int size = variables.size();

	stream->writeShort((uint16)size);

	for (int i = 0; i < size; ++i) {
		/*VectorMapEntry<String, Variable*>* entry = variables.SortedVector<VectorMapEntry<String, Variable*>*>::get(i);
		Variable* variable = entry->getValue();
		String name = entry->getKey();

		name.toBinaryStream(stream);*/
		Variable* variable = variables.get(i);
		variable->toBinaryStream(stream);
	}
}

void Serializable::deSerialize(ObjectInputStream* stream) {
	int size = variables.size();

	uint16 dataSize = stream->readShort();

	if (size != dataSize) {
		System::out << "WARNING data size and variables not equal in void Serializable::deSerialize(ObjectInputStream* stream)\n";

		return;
	}

	for (int i = 0; i < size; ++i) {
		/*String variableName;
		variableName.parseFromBinaryStream(stream);

		Variable* variable = variables.get(variableName);*/

		Variable* variable = variables.get(i);

		variable->parseFromBinaryStream(stream);
	}
}

void Serializable::deSerialize(const String& str) {
	String data;
	try {
		if (!getObjectData(str, data))
			return;

		int size = data.indexOf("=");

		int comma = data.indexOf(",");

		int variableSize = Integer::valueOf(data.subString(size + 1, comma));

		for (int i = 0; i < variableSize; ++i) {
			data = data.subString(comma + 1);

			int equal = data.indexOf("=");

			String variableName = data.subString(0, equal);
			String variableData;

			System::out << data << "\n";

			if (data.subString(equal + 1, equal + 2).indexOf("{") != -1) {
				int lastSemiColon = getObjectData(data, variableData);

				comma = lastSemiColon;
			} else {
				comma = data.indexOf(",");
				if (comma != -1)
					variableData = data.subString(equal + 1, comma);
				else
					variableData = data.subString(equal + 1, data.length());
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
	String name = nameAndVersion;
	int version = 0;

	int verIdx = nameAndVersion.indexOf('[');

	if (verIdx != -1) {
		name = nameAndVersion.subString(0, verIdx);
		version = Integer::valueOf(nameAndVersion.subString(verIdx + 1, nameAndVersion.length()));
	}

	VariableName var;
	var.setName(name);

	Variable* variable = variables.get(var);

	if (variable == NULL) {
		System::out << "WARNING: variable " << nameAndVersion << " not found when deserializing [" << varData << "] \n";

		return;
	}

	//System::out << "deserializing " << name << " version:" << version << "\n";

	variable->parseFromString(varData, version);
}

void Serializable::addSerializableVariable(const String& name, Variable* variable, int version) {
	VariableName varName;
	varName.setName(name);
	varName.setVersion(version);

	variables.put(varName, variable);
}

Variable* Serializable::getSerializableVariable(const String& name) {
	VariableName var;
	var.setName(name);
	return variables.get(var);
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
