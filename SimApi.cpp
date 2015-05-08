/*
 * LuaApi.cpp
 *
 *  Created on: 03-08-2013
 *      Author: Paweu
 */

#include "Simulation.h"
#define IGNORE_SIMAPI
#include "SimApi.h"

namespace SimApi {

int RandRange(int rmin, int rmax){
	int d = rmax - rmin;
	int r = (rand() % d) + rmin;
	return r;
}

bool BoolRand() {
	return rand() & 1;
}

//---------------------------------------------------

LuaConst_t BlenderModeConst[] = {
	{ "ValueTruth", (int)PhaseBlenderMode::ValueTruth, },
	{ "Normalized", (int)PhaseBlenderMode::NormalizedVariable, },
	{ "Index", (int)PhaseBlenderMode::IndexVariable, },
	{ "Range", (int)PhaseBlenderMode::Range, },
	{ 0, 0, },
};

LuaConst_t BlenderStdConst[] = {
	{ "StateTruth", (int)StdColorBlender::StateTruth, },
//	{ "Normalized", (int)PhaseBlenderMode::NormalizedVariable, },
//	{ "Index", (int)PhaseBlenderMode::IndexVariable, },
	{ 0, 0, },
};

LuaConstNamespace_t BlenderNamespace[] = {
	{ "Mode", 0, BlenderModeConst, },
	{ "Std", 0, BlenderStdConst, },
	{ 0, 0, 0, },
};

//---------------------------------------------------

LuaConst_t NeighborhoodModeConst[] = {
	{ "Neuman", (int)NeighborhoodMode::Neuman, },
	{ "Moore", (int)NeighborhoodMode::Moore, },
	{ "HexRandom", (int)NeighborhoodMode::HexRandom, },
	{ "HexLeft", (int)NeighborhoodMode::HexLeft, },
	{ "HexRight", (int)NeighborhoodMode::HexRight, },
	{ "Pentagonal", (int)NeighborhoodMode::Pentagonal, },
	{ 0, 0, },//
};

LuaConst_t NeighborhoodFuncConst[] = {
	{ "Basic", (int)NeighborhoodFuncType::Basic, },
	{ "Null", (int)NeighborhoodFuncType::Null, },
	{ 0, 0, },
};

LuaConstNamespace_t NeighborhoodNamespace[] = {
	{ "Mode", 0, NeighborhoodModeConst, },
	{ "Func", 0, NeighborhoodFuncConst, },
	{ 0, 0, 0, },
};

LuaConstNamespace_t FlagsNamespace[] = {
	{ 0, 0, 0, },
};

//---------------------------------------------------

LuaConst_t TypesConst[] = {
	{ "Int", (int)ParameterType::Int, },
	{ "Float", (int)ParameterType::Float, },
	{ "Byte", (int)ParameterType::Byte, },
	{ 0, 0, },
};
//---------------------------------------------------

LuaConst_t BoundaryConst[] = {
	{ "Cutted", (int)BoundaryMode::Cutted, },
	{ "Circled", (int)BoundaryMode::Circled, },
	{ "Bounced", (int)BoundaryMode::Bounced, },
	{ 0, 0, },
};

//---------------------------------------------------

LuaConstNamespace_t ConstNamespace[] = {
	{ "Neighborhood", NeighborhoodNamespace, 0, },
	{ "Blender", BlenderNamespace, 0, },
	{ "Types", 0, TypesConst, },
	{ "Boundary", 0, BoundaryConst, },
	{ 0, 0, 0, },
};

LuaConstNamespace_t RootNamespace[] = {
	{ "Const", ConstNamespace, 0, },
	{ 0, 0, 0, },
};

}//namespace SimApi
