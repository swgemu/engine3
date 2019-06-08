/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "Serializable.h"

#include "system/lang.h"

#include "engine/log/Logger.h"

Serializable::Serializable() : Object() {
	_variables.setInsertPlan(SortedVector<VectorMapEntry<VariableName, Variable*> >::NO_DUPLICATE);
	_variables.setNullValue(0);

	addSerializableVariable("_className", &_className);
}

/*void Serializable::writeObject(String& str) {
	StringBuffer buffer;

	buffer << "{" << "size=" << _variables.size();

	for (int i = 0; i < _variables.size(); ++i) {
		VectorMapEntry<VariableName, uint64>* entry = &_variables.SortedVector<VectorMapEntry<VariableName, uint64> >::get(i);

		VariableName varName = entry->getKey();
		void* variable = entry->getValue();

		int type = varName.getType();

		int version = varName.getVersion();
		String name = varName.getName();

		if (version != 0) {
			StringBuffer nameAndVersion;
			nameAndVersion << name << "[" << version << "]";
			nameAndVersion.toString(name);
		}

		String value;

		if (type == 0) {
			((Variable*)variable)->toString(value);
		} else {
			serializeAtomicType(variable, type, value);
		}

		buffer << "," << name << "=" << value;
	}

	buffer << "}";

	buffer.toString(str);
}*/

void Serializable::writeObject(ObjectOutputStream* stream) {
	int size = _variables.size();

	stream->writeShort((uint16)size);

	for (int i = 0; i < size; ++i) {
		VectorMapEntry<VariableName, uint64>* entry = &_variables.SortedVector<VectorMapEntry<VariableName, uint64> >::get(i);
		VariableName varName = entry->getKey();
		void* variable = (void*)(entry->getValue() + (uint64)this);

		int type = varName.getType();

		String name = varName.getName();

		name.toBinaryStream(stream);

		int offset = stream->getOffset();
		stream->writeInt(0);

		if (type == 0) {
			(static_cast<Variable*>(variable))->toBinaryStream(stream);
		} else {
			serializeAtomicType(variable, type, stream);
		}

		uint32 totalSize = (uint32) (stream->getOffset() - (offset + 4));

		stream->writeInt(offset, totalSize);
	}
}

int Serializable::getVariableDataOffset(const uint32& variableHashCode, ObjectInputStream* stream) {
	uint16 dataSize = stream->readShort();
	int offset;

	for (int i = 0; i < dataSize; ++i) {
		uint32 namehashCode = stream->readInt();

		uint32 varSize = stream->readInt();

		offset = stream->getOffset();

		if (namehashCode != variableHashCode)
			stream->shiftOffset(varSize);
		else {
			stream->reset();
			return offset;
		}
	}

	offset = -1;

	stream->reset();

	return offset;
}

int Serializable::getVariableDataOffset(const String& variableName, ObjectInputStream* stream) {
	uint16 dataSize = stream->readShort();
	int offset;

	for (int i = 0; i < dataSize; ++i) {
		String name;
		name.parseFromBinaryStream(stream);

		uint32 varSize = stream->readInt();

		offset = stream->getOffset();

		if (name != variableName)
			stream->shiftOffset(varSize);
		else {
			stream->reset();
			return offset;
		}
	}

	offset = -1;

	stream->reset();

	return offset;
}

int Serializable::getVariableNames(Vector<String>& variableNames, ObjectInputStream* stream) {
	uint16 dataSize = stream->readShort();

	for (int i = 0; i < dataSize; ++i) {
		String name;
		name.parseFromBinaryStream(stream);

		variableNames.add(name);

		uint32 varSize = stream->readInt();
		stream->shiftOffset(varSize);
	}

	stream->reset();

	return dataSize;
}

ObjectOutputStream* Serializable::convertToHashCodeNameMembers(ObjectInputStream* stream) {
	ObjectOutputStream* newData = new ObjectOutputStream(stream->size());

	uint16 dataSize = stream->readShort();
	newData->writeShort(dataSize);

	int offset;

	for (int i = 0; i < dataSize; ++i) {
		String name;
		name.parseFromBinaryStream(stream);

		newData->writeInt(name.hashCode());

		uint32 varSize = stream->readInt();

		newData->writeInt(varSize);

		Stream str;

		stream->readStream(&str, varSize);

		str.reset();

		newData->writeStream(&str);
	}

	return newData;
}

ObjectOutputStream* Serializable::changeVariableData(const uint32& variableHashCode, ObjectInputStream* object, Stream* newVariableData) {
	int offset = getVariableDataOffset(variableHashCode, object);

	if (offset == -1)
		return nullptr;

	ObjectOutputStream* newData = new ObjectOutputStream(object->size());
	object->copy(newData);

	object->reset();
	newData->reset();
	object->shiftOffset(offset - 4);
	uint32 dataSize = object->readInt();

	newData->shiftOffset(offset); //we go data length

	if (dataSize > 0)
		newData->removeRange(offset, offset + dataSize);

	newData->writeInt(offset - 4, newVariableData->size());
	newData->insertStream(newVariableData, newVariableData->size(), offset);

	object->reset();
	//newData->reset();

	return newData;
}

ObjectOutputStream* Serializable::changeVariableData(const String& variableName, ObjectInputStream* object, Stream* newVariableData) {
	int offset = getVariableDataOffset(variableName, object);

	if (offset == -1)
		return nullptr;

	ObjectOutputStream* newData = new ObjectOutputStream(object->size());
	object->copy(newData);

	object->reset();
	newData->reset();
	object->shiftOffset(offset - 4);
	uint32 dataSize = object->readInt();

	newData->shiftOffset(offset); //we go data length

	if (dataSize > 0)
		newData->removeRange(offset, offset + dataSize);

	newData->writeInt(offset - 4, newVariableData->size());
	newData->insertStream(newVariableData, newVariableData->size(), offset);

	object->reset();
	//newData->reset();

	return newData;
}

ObjectOutputStream* Serializable::addVariable(const String& variableName, ObjectInputStream* object, Stream* newVariableData) {
	object->reset();

	uint16 oldVariableCount = object->readShort();

	ObjectOutputStream* newData = new ObjectOutputStream(object->size());
	object->copy(newData, 0);

	newData->writeShort(0, oldVariableCount + 1);

	newData->setOffset(newData->size());

	const_cast<String*>(&variableName)->toBinaryStream(newData);

	newData->writeInt(newVariableData->size());
	newData->writeStream(newVariableData);

	return newData;
}

ObjectOutputStream* Serializable::deleteVariable(const String& variableName, ObjectInputStream* object) {
	object->reset();

	uint16 dataSize = object->readShort();

	int startOffset = 0, endOffset = 0;

	for (int i = 0; i < dataSize; ++i) {
		int currentOffset = object->getOffset();

		String name;
		name.parseFromBinaryStream(object);

		uint32 varSize = object->readInt();

		if (name == variableName) {
			//2 bytes size
			//ascii

			startOffset = currentOffset;
			endOffset = currentOffset + name.length() + varSize;
			break;
		}
	}

	object->reset();

	if (endOffset == 0)
		return nullptr;

	ObjectOutputStream* newData = new ObjectOutputStream(object->size());
	object->copy(newData);

	newData->removeRange(startOffset, endOffset);
	newData->reset();

	return newData;
}

ObjectOutputStream* Serializable::changeVariableName(const String& variableName, const String& newVariableName, ObjectInputStream* object) {
	object->reset();

	uint16 dataSize = object->readShort();

	int startOffset = 0, endOffset = 0;

	for (int i = 0; i < dataSize; ++i) {
		int currentOffset = object->getOffset();

		String name;
		name.parseFromBinaryStream(object);

		uint32 varSize = object->readInt();

		if (name == variableName) {
			//2 bytes size
			//ascii

			startOffset = currentOffset;
			endOffset = currentOffset + name.length() + 1;
			break;
		}
	}

	object->reset();

	if (endOffset == 0)
		return nullptr;

	ObjectOutputStream* newData = new ObjectOutputStream(object->size());
	object->copy(newData);

	newData->removeRange(startOffset, endOffset);
	ObjectOutputStream newName;

	String& nonconst = const_cast<String&>(newVariableName);
	nonconst.toBinaryStream(&newName);

	//newData->setOffset(startOffset);
	newData->insertStream(&newName, newName.size(), startOffset);

	return newData;
}

void Serializable::readObject(ObjectInputStream* stream) {
	int size = _variables.size();

	uint16 dataSize = stream->readShort();

	/*if (size != dataSize) {
		System::out << "WARNING data size and variables not equal in void Serializable::deSerialize(ObjectInputStream* stream)\n";

		return;
	}*/

	for (int i = 0; i < dataSize; ++i) {
		String name;
		name.parseFromBinaryStream(stream);

		uint32 varSize = stream->readInt();

		VariableName var;
		var.setName(name.toCharArray());

		int currentOffset = stream->getOffset();

		if (!_variables.contains(var)) {
			//System::out << "WARNING: variable " << name << " not found when deserializing [" << _className << "] \n";

			stream->shiftOffset(varSize);

			continue;
		}

		try {
			VectorMapEntry<VariableName, uint64> e(var);

			int pos = _variables.SortedVector<VectorMapEntry<VariableName, uint64> >::find(e);

			VectorMapEntry<VariableName, uint64>* entry = &_variables.SortedVector<VectorMapEntry<VariableName, uint64> >::get(pos);
			VariableName varName = entry->getKey();
			void* variable = (void*)(entry->getValue() + (uint64)this);

			int type = varName.getType();

			if (type == 0)
				(static_cast<Variable*>(variable))->parseFromBinaryStream(stream);
			else {
				deSerializeAtomicType(variable, type, stream);
			}
		} catch (...) {
			System::out << "WARNING: unreported exception caught variable " << name << " not found when deserializing [" << _className << "] \n";
		}

		stream->setOffset(currentOffset + varSize);
	}
}

/*void Serializable::readObject(const String& str) {
	String data;
	try {

		VectorMap<String, String> variableDataMap;

		int variableSize = getVariableDataMap(str, variableDataMap);

		for (int i = 0; i < variableSize; ++i) {
			VectorMapEntry<String, String>* entry = &variableDataMap.SortedVector<VectorMapEntry<String, String> >::get(i);

			String variableName = entry->getKey();
			String variableData = entry->getValue();

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
	var.setName(name.toCharArray());

	if (!_variables.contains(var)) {
		System::out << "WARNING: variable " << nameAndVersion << " not found when deserializing [" << _className << "] \n";

		return;
	}

	//System::out << "deserializing " << name << " version:" << version << "\n";

	VectorMapEntry<VariableName, uint64> e(var);

	int pos = _variables.SortedVector<VectorMapEntry<VariableName, uint64> >::find(e);

	VectorMapEntry<VariableName, uint64>* entry = &_variables.SortedVector<VectorMapEntry<VariableName, uint64> >::get(pos);
	VariableName varName = entry->getKey();
	void* variable = entry->getValue();

	int type = varName.getType();

	if (type == 0)
		((Variable*)variable)->parseFromString(varData, version);
	else {
		deSerializeAtomicType(variable, type, varData);
	}

}*/

void Serializable::addSerializableVariable(const char* name, Variable* variable, int version) {
	if (variable == nullptr)
		return;

	VariableName varName;
	varName.setName(name);
	varName.setVersion(version);
	varName.setType(0);

	if (_variables.put(varName, (uint64)variable - (uint64)this) == -1)
		Logger::console.warning("duplicate serializable variable: " + String(name));
}

void Serializable::addSerializableVariable(const char* name, uint8* variable, int version) {
	if (variable == nullptr)
		return;

	VariableName varName;
	varName.setName(name);
	varName.setVersion(version);
	varName.setType(TypeInfo<uint8>::type);

	if (_variables.put(varName, (uint64)variable - (uint64)this) == -1)
		Logger::console.warning("duplicate serializable variable: " + String(name));
}

void Serializable::addSerializableVariable(const char* name, int8* variable, int version) {
	if (variable == nullptr)
		return;

	VariableName varName;
	varName.setName(name);
	varName.setVersion(version);
	varName.setType(TypeInfo<int8>::type);

	if (_variables.put(varName, (uint64)variable - (uint64)this) == -1)
		Logger::console.warning("duplicate serializable variable: " + String(name));
}

void Serializable::addSerializableVariable(const char* name, uint16* variable, int version) {
	if (variable == nullptr)
		return;

	VariableName varName;
	varName.setName(name);
	varName.setVersion(version);
	varName.setType(TypeInfo<uint16>::type);

	if (_variables.put(varName, (uint64)variable - (uint64)this) == -1)
		Logger::console.warning("duplicate serializable variable: " + String(name));
}

void Serializable::addSerializableVariable(const char* name, int16* variable, int version) {
	if (variable == nullptr)
		return;

	VariableName varName;
	varName.setName(name);
	varName.setVersion(version);
	varName.setType(TypeInfo<int16>::type);

	if (_variables.put(varName, (uint64)variable - (uint64)this) == -1)
		Logger::console.warning("duplicate serializable variable: " + String(name));
}

void Serializable::addSerializableVariable(const char* name, uint32* variable, int version) {
	if (variable == nullptr)
		return;

	VariableName varName;
	varName.setName(name);
	varName.setVersion(version);
	varName.setType(TypeInfo<uint32>::type);

	if (_variables.put(varName, (uint64)variable - (uint64)this) == -1)
		Logger::console.warning("duplicate serializable variable: " + String(name));
}

void Serializable::addSerializableVariable(const char* name, int32* variable, int version) {
	if (variable == nullptr)
		return;

	VariableName varName;
	varName.setName(name);
	varName.setVersion(version);
	varName.setType(TypeInfo<int32>::type);

	if (_variables.put(varName, (uint64)variable - (uint64)this) == -1)
		Logger::console.warning("duplicate serializable variable: " + String(name));
}

void Serializable::addSerializableVariable(const char* name, uint64* variable, int version) {
	if (variable == nullptr)
		return;

	VariableName varName;
	varName.setName(name);
	varName.setVersion(version);
	varName.setType(TypeInfo<uint64>::type);

	if (_variables.put(varName, (uint64)variable - (uint64)this) == -1)
		Logger::console.warning("duplicate serializable variable: " + String(name));
}

void Serializable::addSerializableVariable(const char* name, int64* variable, int version) {
	if (variable == nullptr)
		return;

	VariableName varName;
	varName.setName(name);
	varName.setVersion(version);
	varName.setType(TypeInfo<int64>::type);

	if (_variables.put(varName, (uint64)variable - (uint64)this) == -1)
		Logger::console.warning("duplicate serializable variable: " + String(name));
}

void Serializable::addSerializableVariable(const char* name, float* variable, int version) {
	if (variable == nullptr)
		return;

	VariableName varName;
	varName.setName(name);
	varName.setVersion(version);
	varName.setType(TypeInfo<float>::type);

	if (_variables.put(varName, (uint64)variable - (uint64)this) == -1)
		Logger::console.warning("duplicate serializable variable: " + String(name));
}

void Serializable::addSerializableVariable(const char* name, double* variable, int version) {
	if (variable == nullptr)
		return;

	VariableName varName;
	varName.setName(name);
	varName.setVersion(version);
	varName.setType(TypeInfo<double>::type);

	if (_variables.put(varName, (uint64)variable - (uint64)this) == -1)
		Logger::console.warning("duplicate serializable variable: " + String(name));
}

void Serializable::addSerializableVariable(const char* name, bool* variable, int version) {
	if (variable == nullptr)
		return;

	VariableName varName;
	varName.setName(name);
	varName.setVersion(version);
	varName.setType(TypeInfo<bool>::type);

	if (_variables.put(varName, (uint64)variable - (uint64)this) == -1)
		Logger::console.warning("duplicate serializable variable: " + String(name));
}

Variable* Serializable::getSerializableVariable(const char* name) {
	VariableName var;
	var.setName(name);

	VectorMapEntry<VariableName, uint64> e(var);

	int pos = _variables.SortedVector<VectorMapEntry<VariableName, uint64> >::find(e);

	VectorMapEntry<VariableName, uint64>* entry = &_variables.SortedVector<VectorMapEntry<VariableName, uint64> >::get(pos);
	VariableName varName = entry->getKey();
	void* variable = (void*)(entry->getValue() + (uint64)this);

	if (varName.getType() == 0)
		return static_cast<Variable*>(variable);
	else
		return nullptr;
}

int Serializable::getObjectData(const String& str, String& obj) {
	return Vector<int>::getObjectData(str, obj);
}

int Serializable::serializeAtomicType(void* address, int type, String& value) {
	switch (type) {
	case TypeInfoAtomicBase<uint8>::UINT8:
		TypeInfo<uint8>::toString(address, value);
	break;

	case TypeInfoAtomicBase<uint16>::UINT16:
		TypeInfo<uint16>::toString(address, value);
	break;

	case TypeInfoAtomicBase<uint32>::UINT32:
		TypeInfo<uint32>::toString(address, value);
	break;

	case TypeInfoAtomicBase<uint64>::UINT64:
		TypeInfo<uint64>::toString(address, value);
	break;

	case TypeInfoAtomicBase<int8>::INT8:
		TypeInfo<int8>::toString(address, value);
	break;

	case TypeInfoAtomicBase<int16>::INT16:
		TypeInfo<int16>::toString(address, value);
	break;

	case TypeInfoAtomicBase<int32>::INT32:
		TypeInfo<int32>::toString(address, value);
	break;

	case TypeInfoAtomicBase<int64>::INT64:
		TypeInfo<int64>::toString(address, value);
	break;

	case TypeInfoAtomicBase<float>::FLOAT:
		TypeInfo<float>::toString(address, value);
	break;

	case TypeInfoAtomicBase<double>::DOUBLE:
		TypeInfo<double>::toString(address, value);
	break;

	case TypeInfoAtomicBase<bool>::BOOL:
		TypeInfo<bool>::toString(address, value);
	break;

	default:
		return 0;
	}

	return 1;
}

int Serializable::serializeAtomicType(void* address, int type, ObjectOutputStream* stream) {
	switch (type) {
	case TypeInfoAtomicBase<uint8>::UINT8:
		TypeInfo<uint8>::toBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<uint16>::UINT16:
		TypeInfo<uint16>::toBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<uint32>::UINT32:
		TypeInfo<uint32>::toBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<uint64>::UINT64:
		TypeInfo<uint64>::toBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<int8>::INT8:
		TypeInfo<int8>::toBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<int16>::INT16:
		TypeInfo<int16>::toBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<int32>::INT32:
		TypeInfo<int32>::toBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<int64>::INT64:
		TypeInfo<int64>::toBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<float>::FLOAT:
		TypeInfo<float>::toBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<double>::DOUBLE:
		TypeInfo<double>::toBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<bool>::BOOL:
		TypeInfo<bool>::toBinaryStream(address, stream);
	break;

	default:
		return 0;
	}

	return 1;
}

int Serializable::deSerializeAtomicType(void* address, int type, const String& value) {
	switch (type) {
	case TypeInfoAtomicBase<uint8>::UINT8:
		TypeInfo<uint8>::parseFromString(address, value);
	break;

	case TypeInfoAtomicBase<uint16>::UINT16:
		TypeInfo<uint16>::parseFromString(address, value);
	break;

	case TypeInfoAtomicBase<uint32>::UINT32:
		TypeInfo<uint32>::parseFromString(address, value);
	break;

	case TypeInfoAtomicBase<uint64>::UINT64:
		TypeInfo<uint64>::parseFromString(address, value);
	break;

	case TypeInfoAtomicBase<int8>::INT8:
		TypeInfo<int8>::parseFromString(address, value);
	break;

	case TypeInfoAtomicBase<int16>::INT16:
		TypeInfo<int16>::parseFromString(address, value);
	break;

	case TypeInfoAtomicBase<int32>::INT32:
		TypeInfo<int32>::parseFromString(address, value);
	break;

	case TypeInfoAtomicBase<int64>::INT64:
		TypeInfo<int64>::parseFromString(address, value);
	break;

	case TypeInfoAtomicBase<float>::FLOAT:
		TypeInfo<float>::parseFromString(address, value);
	break;

	case TypeInfoAtomicBase<double>::DOUBLE:
		TypeInfo<double>::parseFromString(address, value);
	break;

	case TypeInfoAtomicBase<bool>::BOOL:
		TypeInfo<bool>::parseFromString(address, value);
	break;

	default:
		return 0;
	}

	return 1;
}

int Serializable::deSerializeAtomicType(void* address, int type, ObjectInputStream* stream) {
	switch (type) {
	case TypeInfoAtomicBase<uint8>::UINT8:
		TypeInfo<uint8>::parseFromBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<uint16>::UINT16:
		TypeInfo<uint16>::parseFromBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<uint32>::UINT32:
		TypeInfo<uint32>::parseFromBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<uint64>::UINT64:
		TypeInfo<uint64>::parseFromBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<int8>::INT8:
		TypeInfo<int8>::parseFromBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<int16>::INT16:
		TypeInfo<int16>::parseFromBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<int32>::INT32:
		TypeInfo<int32>::parseFromBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<int64>::INT64:
		TypeInfo<int64>::parseFromBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<float>::FLOAT:
		TypeInfo<float>::parseFromBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<double>::DOUBLE:
		TypeInfo<double>::parseFromBinaryStream(address, stream);
	break;

	case TypeInfoAtomicBase<bool>::BOOL:
		TypeInfo<bool>::parseFromBinaryStream(address, stream);
	break;

	default:
		return 0;
	}

	return 1;
}

int Serializable::getVariableDataMap(const String& serializedData, VectorMap<String, String>& map) {
	String data;
	map.removeAll();

	try {
		if (!getObjectData(serializedData, data))
			return 0;

		int size = data.indexOf("=");

		int comma = data.indexOf(",");

		int variableSize;

		if (comma != -1)
			variableSize = Integer::valueOf(data.subString(size + 1, comma));
		else
			variableSize = Integer::valueOf(data.subString(size + 1, data.length() - 1));

		for (int i = 0; i < variableSize; ++i) {
			data = data.subString(comma + 1);

			int equal = data.indexOf("=");

			String variableName = data.subString(0, equal);
			String variableData;

			//System::out << data << "\n";

			if (data.subString(equal + 1, equal + 2).indexOf("{") != -1) {
				int lastSemiColon = getObjectData(data, variableData);

				comma = lastSemiColon;
			} else {
				comma = data.indexOf(",");

				if (comma != -1)
					variableData = data.subString(equal + 1, comma);
				else {
					variableData = data.subString(equal + 1, data.length() - 1);

					//System::out << variableData << "\n";
				}
			}

			map.put(variableName, variableData);
		}

	} catch (Exception& e) {
		System::out << e.getMessage();
		e.printStackTrace();
	} catch (...) {
		System::out << "Unreported exception caught in int Serializable::getVariableDataMap(const String& serializedData, VectorMap<String, String>& map)\n";
	}

	return map.size();
}

