#pragma once

#include <unordered_map>

template <class T, typename STRING_TYPE, T DEFVAL = T(0)>
struct EnumConverter : protected std::unordered_map<STRING_TYPE, T> {
	typedef T type;
	typedef STRING_TYPE string_t;
	class ValueNotFoundExeption : public std::exception {};

	T FromString(const string_t &Value) const {
		auto it = this->find(Value);
		if(it == this->end()) throw ValueNotFoundExeption();
		return it->second;
	}

	T OrdinalString(const string_t &Value) const {
		auto it = this->find(Value);
		if(it == this->end()) return DEFVAL;
		return it->second;
	}

	T GetDefaultValue() const { return DEFVAL; }

	const string_t ToString(T Value) const {
		for(auto &it: *this) 
			if(it.second == Value)
				return it.first;
		throw ValueNotFoundExeption();
	}
	

	template <class INP>
	T Cast(INP v) const {
		for(auto &it: *this)
			if(it.second == static_cast<T>(v))
				return it.second;
		return GetDefaultValue();
	}
protected:
	void Add(const string_t &str, T value) {
		this->insert(std::make_pair(string_t(str), value));
	}
};

template <class T, typename STRING_TYPE, T DEFVAL = T(0)>
struct InstancedEnumConverter {
	typedef T type;
	typedef STRING_TYPE string_t;
	typedef typename EnumConverter<T, STRING_TYPE, DEFVAL>::ValueNotFoundExeption ValueNotFoundExeption;

	template <class INPUT>
	static T Cast(INPUT v) { return _Instance->Cast(v); }
	static T FromString(const string_t &Value) { return _Instance->FromString(Value); }
	static T OrdinalString(const string_t &Value) { return _Instance->OrdinalString(Value); }
	static T GetDefaultValue() { return DEFVAL; }
	static const string_t ToString(T Value) { return _Instance->ToString(Value); }

	InstancedEnumConverter() {
		m_IsOwner = _Instance == 0;
		if(!_Instance)
			_Instance = new EnumConverterInstance();
	}

	~InstancedEnumConverter() {
		if(m_IsOwner) {
			m_IsOwner = false;
			delete _Instance;
		}
	}
protected:
	struct EnumConverterInstance : public EnumConverter<T, STRING_TYPE, DEFVAL> {
		using EnumConverter<T, STRING_TYPE, DEFVAL>::Add;
	};
	static EnumConverterInstance *_Instance;

	void Add(const string_t &str, T value) {
		_Instance->Add(str, value);
	}
	bool m_IsOwner;
};

template <class T, typename STRING_TYPE, T DEFVAL>
typename InstancedEnumConverter<T, STRING_TYPE, DEFVAL>::EnumConverterInstance *InstancedEnumConverter<T, STRING_TYPE, DEFVAL>::_Instance = 0;

