#ifndef TYPES_H_
#define TYPES_H_

template<typename T> class TypeInfoAtomicBase {
public:
	static const bool needConstructor = false;
};

template<typename T> class TypeInfoConstructedBase {
public:
	static const bool needConstructor = true;
};

template<typename T> class TypeInfo : public TypeInfoConstructedBase<T> {};

template<typename T> class TypeInfo<T*> : public TypeInfoAtomicBase<T*> {};

template<> class TypeInfo<uint8> : public TypeInfoAtomicBase<uint8> {};
template<> class TypeInfo<uint16> : public TypeInfoAtomicBase<uint16> {};
template<> class TypeInfo<uint32> : public TypeInfoAtomicBase<uint32> {};
template<> class TypeInfo<int8> : public TypeInfoAtomicBase<int8> {};
template<> class TypeInfo<int16> : public TypeInfoAtomicBase<int16> {};
template<> class TypeInfo<int32> : public TypeInfoAtomicBase<int32> {};
template<> class TypeInfo<uint64> : public TypeInfoAtomicBase<uint64> {};
template<> class TypeInfo<int64> : public TypeInfoAtomicBase<int64> {};
template<> class TypeInfo<float> : public TypeInfoAtomicBase<float> {};
template<> class TypeInfo<double> : public TypeInfoAtomicBase<double> {};

#endif /*TYPES_H_*/
