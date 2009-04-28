/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "Serializable.h"

#include "../lang/StringBuffer.h"
#include "StringTokenizer.h"

#include "../lang.h"

Serializable::Serializable() {
	variables.setInsertPlan(SortedVector<VectorMapEntry<String, Variable*>*>::NO_DUPLICATE);
	variables.setNullValue(NULL);
}

void Serializable::serialize(String& str) {
	StringBuffer buffer;

	buffer << variables.size() << ";";

	for (int i = 0; i < variables.size(); ++i) {
		VectorMapEntry<String, Variable*>* entry = variables.SortedVector<VectorMapEntry<String, Variable*>*>::get(i);

		String nameAndVersion = entry->getKey();
		Variable* variable = entry->getValue();

		String value;
		variable->toString(&value);

		buffer << nameAndVersion << "=" << value << ";";
	}

	buffer.toString(str);
}

void Serializable::deSerialize(const String& str) {
	try {
		StringTokenizer mainTokens(str);
		mainTokens.setDelimeter(";");

		String name;

		int size = mainTokens.getIntToken();

		for (int i = 0; i < size; ++i) {
			String variable;
			mainTokens.getStringToken(variable);

			deSerializeVariable(variable);
		}

	} catch (Exception& e) {
		System::out << e.getMessage();
		e.printStackTrace();
	} catch (...) {
		System::out << "Unreported exception caught in void Serializable::deSerialize(const String& str)\n";
	}
}

void Serializable::deSerializeVariable(const String& var) {
	StringTokenizer tokenizer(var);
	tokenizer.setDelimeter("=");

	String nameAndVersion, data;
	tokenizer.getStringToken(nameAndVersion);

	Variable* variable = variables.get(nameAndVersion);

	if (variable == NULL) {
		System::out << "WARNING: variable " << nameAndVersion << " not found when deserializing [" << var << "] \n";

		return;
	}

	tokenizer.getStringToken(data);

	if (nameAndVersion.indexOf("|") != -1) {
		StringTokenizer ver(nameAndVersion);
		ver.setDelimeter("|");
		ver.shiftTokens(1);

		int version = ver.getIntToken();

		if (!variable->parseFromString(&data, version))
			System::out << "WARNING: could not deserialize variable " << nameAndVersion << "\n";
	} else {
		variable->parseFromString(&data);
	}
}
