/*
 * Types.h
 *
 *  Created on: 6 cze 2014
 *      Author: Paweu
 */

#ifndef TYPES_H_
#define TYPES_H_

namespace Types {

enum class PhaseBlenderMode {
	Undefined,
	ValueTruth,
	NormalizedVariable,
	IndexVariable,
	Range,

	Default = ValueTruth,
};
struct PhaseBlenderModeMap : public InstancedEnumConverter<PhaseBlenderMode, std::string, PhaseBlenderMode::Undefined> {
	PhaseBlenderModeMap() {
		Add("ValueTruth", 				type::ValueTruth);
		Add("NormalizedVariable", 		type::NormalizedVariable);
		Add("IndexVariable", 			type::IndexVariable);
		Add("Range", 					type::Range);
	}
};

//------------------------

enum class StdColorBlender {
	Undefined,
	StateTruth,

	Default = StateTruth,
};
struct StdColorBlenderMap : public InstancedEnumConverter<StdColorBlender, std::string, StdColorBlender::Undefined> {
	StdColorBlenderMap() {
		Add("StateTruth", 				type::StateTruth);
		//Add("NormalizedVariable", 		type::NormalizedVariable);
	//	Add("IndexVariable", 			type::IndexVariable);
	}
};

//------------------------

enum class NeighborhoodMode {
	Unknown,
	Moore,
	Neuman,
	HexRandom, HexLeft, HexRight, Pentagonal,
	Default = Moore,
};
struct NeighborhoodModeMap : public InstancedEnumConverter<NeighborhoodMode, std::string, NeighborhoodMode::Unknown> {
	NeighborhoodModeMap(){
		Add("Neuman", type::Neuman);
		Add("Moore", type::Moore);
		Add("HexRandom", type::HexRandom);
		Add("HexLeft", type::HexLeft);
		Add("HexRight", type::HexRight);
		Add("Pentagonal", type::Pentagonal);
	}
};

//------------------------

enum class NeighborhoodFuncType {
	Unknown,
	Null,
	Basic,

	Default = Basic,
};
struct NeighborhoodFuncTypeMap : public InstancedEnumConverter<NeighborhoodFuncType, std::string, NeighborhoodFuncType::Unknown> {
	NeighborhoodFuncTypeMap() {
		Add("Null", type::Null);
		Add("Basic", type::Basic);
		Add("Extended", type::Basic);
	}
};

//------------------------

enum class BoundaryMode {
	Unknown, Circled, Cutted, Bounced,
	Default = Circled,
};
struct BoundaryModeMap : public InstancedEnumConverter<BoundaryMode, std::string, BoundaryMode::Unknown> {
	BoundaryModeMap() {
		Add("Circled", type::Circled);
		Add("Cutted", type::Cutted);
		Add("Bounced", type::Bounced);
	}
};

} // namespace Types

using namespace Types;

#endif /* TYPES_H_ */
