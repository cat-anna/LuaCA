/*
 * PhaseBlender.cpp
 *
 *  Created on: 10 maj 2014
 *      Author: Paweu
 */

#include "Simulation.h"

//----------------------------------------------------
struct StateTruthStdBlender: public PhaseBlender {
	StateTruthStdBlender(Simulation *Owner) :
			PhaseBlender(Owner) {
		m_Mode = PhaseBlenderMode::ValueTruth;
		m_ParamName = "state";
		m_ColorVector->push_back(glm::vec4(0, 0, 0, 1));
		m_ColorVector->push_back(glm::vec4(1, 1, 1, 1));
	}
};

//----------------------------------------------------

static void BlendColor(glm::vec3 &out, const glm::vec4 &in4) {
	glm::vec3 in3(in4);
	float a = in4[3];
	in3 *= a;
	out = out * (1.0f - a) + in3;
}

template<class T>
struct ValueTruthBlender: public PhaseBlenderInstance {
	ValueTruthBlender(SimulationParameters *sp, unsigned ByteOffset,
			SharedColorVector ColorVector, PhaseBlender *Parent) :
			PhaseBlenderInstance(sp, ByteOffset, ColorVector, Parent) {
		if (m_ColorVector->size() < 2) {
			//TODO: ??
		}
	}
	virtual void ProcessColor(glm::vec3 &color, const CellContext *cell) const {
		T value = cell->GetValue<T>(m_ByteOffset);
		glm::vec4 &colt4 = m_ColorVector->at(value != 0 ? 1 : 0);
		BlendColor(color, colt4);
	}
};

template<class T>
struct IndexedBlender: public PhaseBlenderInstance {
	IndexedBlender(SimulationParameters *sp, unsigned ByteOffset,
			SharedColorVector ColorVector, PhaseBlender *Parent) :
			PhaseBlenderInstance(sp, ByteOffset, ColorVector, Parent) {
	}
	virtual void ProcessColor(glm::vec3 &color, const CellContext *cell) const {
		int value = static_cast<int>(cell->GetValue<T>(m_ByteOffset));
		glm::vec4 &colt4 = m_ColorVector->at(value > 0 ? value : 0);
		BlendColor(color, colt4);
	}
};

template<class T>
struct NormalizedBlender: public PhaseBlenderInstance {
	NormalizedBlender(SimulationParameters *sp, unsigned ByteOffset,
			SharedColorVector ColorVector, PhaseBlender *Parent) :
			PhaseBlenderInstance(sp, ByteOffset, ColorVector, Parent) {
	}
	virtual void ProcessColor(glm::vec3 &color, const CellContext *cell) const {
		double value = static_cast<double>(cell->GetValue<T>(m_ByteOffset));
		double pos = (m_ColorVector->size() - 1) * value;
		int index = static_cast<int>(pos);
		if (value == 1)
			--index;
		pos -= index;
		glm::vec4 &col1 = m_ColorVector->at(index);
		glm::vec4 &col2 = m_ColorVector->at(index + 1);
		glm::vec4 col = col1 * (1 - pos);
		col += col2 * (pos);
		BlendColor(color, col);
	}
};

template<class T>
struct RangeBlender: public PhaseBlenderInstance {
	RangeBlender(SimulationParameters *sp, unsigned ByteOffset,
			SharedColorVector ColorVector, PhaseBlender *Parent) :
			PhaseBlenderInstance(sp, ByteOffset, ColorVector, Parent), m_min(), m_max(
					100), m_delta(100) {
	}
	virtual void ProcessColor(glm::vec3 &color, const CellContext *cell) const {
		T input = static_cast<T>(cell->GetValue<T>(m_ByteOffset));
		if (m_Adaptive) {
			if (input > m_AdaptiveMax)
				m_AdaptiveMax = input;
			else if (input < m_AdaptiveMin)
				m_AdaptiveMin = input;
		}

		if (input > m_max)
			input = m_max;
		else if (input < m_min)
			input = m_min;

		input -= m_min;
		float value = input / m_delta;
		float pos = (m_ColorVector->size() - 1) * value;
		int index = static_cast<int>(pos);
		if (value == 1)
			--index;
		pos -= index;
//		try {
			glm::vec4 &col1 = m_ColorVector->at(index);
			glm::vec4 &col2 = m_ColorVector->at(index + 1);
			glm::vec4 col = col1 * (1 - pos);
			col += col2 * (pos);
			BlendColor(color, col);
//		} catch (...) {
//			return;
//		}
	}
	void SetParameters(LuaApi::TableDispatcher &t) {
		m_AdaptiveMin = m_min = t.get<T>("min", m_min);
		m_AdaptiveMax = m_max = t.get<T>("max", m_max);
		m_Adaptive = t.getBoolean("Adaptive",  false);
		m_delta = m_max - m_min;
	}
	void InitStep() {
		if (m_Adaptive) {

			if(abs(100.0f / (float) m_AdaptiveMin) < 0.1f) {
				if (m_AdaptiveMin < 0) m_min = m_AdaptiveMin + 100;
				else m_min = m_AdaptiveMin - 100;
			} else {
				if (m_AdaptiveMin < 0) m_min = m_AdaptiveMin * 1.1;
				else m_min = m_AdaptiveMin * 0.9;
			}

			if(abs(100.0f / (float) m_AdaptiveMax) < 0.1f) {
				if (m_AdaptiveMax < 0) m_max = m_AdaptiveMax + 100;
				else m_max = m_AdaptiveMax - 100;
			} else {
				if (m_AdaptiveMax < 0) m_max = m_AdaptiveMax * 1.1;
				else m_max = m_AdaptiveMax * 0.9;
			}

			m_delta = m_max - m_min;
			if(m_delta == 0) m_delta = 1;
			m_AdaptiveMin = std::numeric_limits<T>::max();
			m_AdaptiveMax = std::numeric_limits<T>::min();
		}
	}
private:
	T m_min, m_max;
	mutable T m_AdaptiveMin, m_AdaptiveMax;
	bool m_Adaptive;
	float m_delta;
};

//----------------------------------------------------

PhaseBlenderInstance::PhaseBlenderInstance(SimulationParameters *sp,
		unsigned ByteOffset, SharedColorVector ColorVector,
		PhaseBlender *Parent) :
		m_Parametrs(sp), m_ByteOffset(ByteOffset), m_ColorVector(ColorVector), m_Parent(
				Parent) {
}

PhaseBlenderInstance::~PhaseBlenderInstance() {
}

void PhaseBlenderInstance::SetParameters(LuaApi::TableDispatcher &) {
}

void PhaseBlenderInstance::InitStep() {

}
//----------------------------------------------------

PhaseBlender::PhaseBlender(Simulation *Owner, const char* Name) :
		m_ParamName(""), m_Name("?"), m_Sim(Owner), m_Mode(), m_ColorVector(
				new ColorVector()), m_Instance() {
	if (Name)
		m_Name = Name;
}

PhaseBlender::~PhaseBlender() {
}

template<template<typename > class OUT, typename ... Args>
PhaseBlenderInstance* CreateInstance(ParameterType pt, Args ... args) {
	switch (pt) {
	case ParameterType::Byte:
		return new OUT<LuaTypes::Byte>(args...);
	case ParameterType::Int:
		return new OUT<LuaTypes::Int>(args...);
	case ParameterType::Float:
		return new OUT<LuaTypes::Float>(args...);
	default:
		cerr
				<< "Error creating blender instance: parameter has unknown type!\n";
	}
	return 0;
}

SharedPhaseBlenderInstance& PhaseBlender::GetInstance() {
	if (!m_Instance)
		m_Instance = std::shared_ptr<PhaseBlenderInstance>(ConstructInstance());
	return m_Instance;
}

PhaseBlenderInstance* PhaseBlender::ConstructInstance() {
	SimulationParameters *sp = &m_Sim->GetParameters();
	const std::shared_ptr<SimParameter> Parameter = sp->GetParameter(
			m_ParamName.c_str());
	if (!Parameter) {
		cerr << "Cannot create blender instance for non existing parameter: '"
				<< m_ParamName << "'\n";
		throw "Cannot create blender instance for non existing parameter!";
	}
	ParameterType type = Parameter->GetType();
	unsigned offset = Parameter->GetByteOffset();
	switch (m_Mode) {
	case PhaseBlenderMode::ValueTruth: {
		return CreateInstance<ValueTruthBlender>(type, sp, offset,
				m_ColorVector, this);
	}
	case PhaseBlenderMode::NormalizedVariable: {
		return CreateInstance<NormalizedBlender>(type, sp, offset,
				m_ColorVector, this);
	}
	case PhaseBlenderMode::IndexVariable: {
		return CreateInstance<IndexedBlender>(type, sp, offset, m_ColorVector,
				this);
	}
	case PhaseBlenderMode::Range: {
		return CreateInstance<RangeBlender>(type, sp, offset, m_ColorVector,
				this);
	}
	default:
		return 0;
	}
}

void PhaseBlender::SetParameters(LuaApi::TableDispatcher &t) {
	GetInstance()->SetParameters(t);
}

void PhaseBlender::SetParameter(const char* Name) {
	cout << "PhaseBlender '" << m_Name << "': Setting parameter to '" << Name
			<< "'\n";
	m_ParamName = Name;
}

void PhaseBlender::AddColor4(float r, float g, float b, float a) {
//	char buf[128];
//	sprintf(buf, "(%.2f, %.2f, %.2f, %.2f)", r, g, b, a);
//	cout << "PhaseBlender: Adding color: " << buf << "\n";
	m_ColorVector->push_back(glm::vec4(r, g, b, a));
}

void PhaseBlender::SetMode(int Mode) {
	PhaseBlenderMode type = PhaseBlenderModeMap::Cast(Mode);
	if (type == PhaseBlenderModeMap::GetDefaultValue()) {
		type = PhaseBlenderMode::Default;
		cout << "PhaseBlender '" << m_Name << "': Unknown mode, defaulting to '"
				<< PhaseBlenderModeMap::ToString(type) << "'\n";
	}
	cout << "PhaseBlender '" << m_Name << "': Changing mode to: '"
			<< PhaseBlenderModeMap::ToString(type) << "'\n";
	m_Mode = type;
}

std::shared_ptr<PhaseBlender> PhaseBlender::GetStdColorBlender(
		Simulation *Owner, StdColorBlender which, const std::string &Name) {
	static std::shared_ptr<PhaseBlender> StateTruth(
			new StateTruthStdBlender(Owner));
	StateTruth->m_Name = Name;
	switch (which) {
	case StdColorBlender::StateTruth:
		return StateTruth;
	default:
		break;
	}
	return std::shared_ptr<PhaseBlender>(0);
}

void PhaseBlender::GenerateColors(int /*count*/) {

}

void PhaseBlender::GenerateColorsByBits(unsigned BitsPerColor) {
	int size = 1 << (3 * BitsPerColor);
	cout << "PhaseBlender '" << m_Name << "': Generating colors from "
			<< BitsPerColor << " bits, got " << size << " colors\n";
	m_ColorVector->reserve(size);
	for (unsigned i = 0, j = size; i < j; ++i) {
		unsigned mask = (1 << BitsPerColor) - 1;
		unsigned delta = 256 / (1 << BitsPerColor);
		unsigned g = (i & mask) * delta;
		unsigned b = ((i >> BitsPerColor) & mask) * delta;
		unsigned r = ((i >> (BitsPerColor * 2)) & mask) * delta;
		glm::vec4 c;
		c[0] = r / 255.0f;
		c[1] = g / 255.0f;
		c[2] = b / 255.0f;
		c[3] = 1.0f;
		m_ColorVector->push_back(c);
	}
}
