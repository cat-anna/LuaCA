#pragma once

namespace Neighborhood {

struct HexagonalLeft : public IVec2Vector {
	HexagonalLeft() {
		push_back(glm::ivec2(-1, -1));
		push_back(glm::ivec2( 0, -1));
		push_back(glm::ivec2(-1,  0));
		push_back(glm::ivec2( 1,  1));
		push_back(glm::ivec2( 0,  1));
		push_back(glm::ivec2( 1,  0));
	}
};
const HexagonalLeft HexagonalLeftInstance;

struct HexagonalRight : public IVec2Vector {
	HexagonalRight() {
		push_back(glm::ivec2(-1,  1));
		push_back(glm::ivec2( 0,  1));
		push_back(glm::ivec2(-1,  0));
		push_back(glm::ivec2( 1, -1));
		push_back(glm::ivec2( 0, -1));
		push_back(glm::ivec2( 1,  0));
	}
};
const HexagonalRight HexagonalRightInstance;

struct Moore : public IVec2Vector {
	Moore() {
		push_back(glm::ivec2(-1, -1));
		push_back(glm::ivec2( 0, -1));
		push_back(glm::ivec2( 1, -1));
		push_back(glm::ivec2(-1,  0));
		push_back(glm::ivec2( 1,  0));
		push_back(glm::ivec2(-1,  1));
		push_back(glm::ivec2( 0,  1));
		push_back(glm::ivec2( 1,  1));
	}
};
const Moore MooreInstance;

struct Neuman : public IVec2Vector {
	Neuman() {
		push_back(glm::ivec2( 0, -1));
		push_back(glm::ivec2(-1,  0));
		push_back(glm::ivec2( 1,  0));
		push_back(glm::ivec2( 0,  1));
	}
};
const Neuman NeumanInstance;

struct PentagonalTop : public IVec2Vector {
	PentagonalTop() {
		push_back(glm::ivec2(-1, -1));
		push_back(glm::ivec2( 0, -1));
		push_back(glm::ivec2( 1, -1));
		push_back(glm::ivec2(-1,  0));
		push_back(glm::ivec2( 1,  0));
	}
};

struct PentagonalBottom : public IVec2Vector {
	PentagonalBottom() {
		push_back(glm::ivec2(-1,  1));
		push_back(glm::ivec2( 0,  1));
		push_back(glm::ivec2( 1,  1));
		push_back(glm::ivec2(-1,  0));
		push_back(glm::ivec2( 1,  0));
	}
};

struct PentagonalLeft : public IVec2Vector {
	PentagonalLeft() {
		push_back(glm::ivec2( 0, -1));
		push_back(glm::ivec2( 0,  1));
		push_back(glm::ivec2(-1,  1));
		push_back(glm::ivec2(-1,  0));
		push_back(glm::ivec2(-1, -1));
	}
};

struct PentagonalRight : public IVec2Vector {
	PentagonalRight() {
		push_back(glm::ivec2( 0, -1));
		push_back(glm::ivec2( 0,  1));
		push_back(glm::ivec2( 1,  1));
		push_back(glm::ivec2( 1,  0));
		push_back(glm::ivec2( 1, -1));
	}
};

const IVec2Vector *Pentagonal[] = {
	new PentagonalTop(), new PentagonalLeft(), new PentagonalBottom(), new PentagonalRight(),
};

} //namespace Neighborhood
