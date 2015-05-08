/*
 * PhaseBlender.h
 *
 *  Created on: 10 maj 2014
 *      Author: Paweu
 */

#ifndef PHASEBLENDER_H_
#define PHASEBLENDER_H_

typedef std::vector<glm::vec4> ColorVector;
typedef std::shared_ptr<ColorVector> SharedColorVector;

struct PhaseBlenderInstance {
	PhaseBlenderInstance(SimulationParameters *sp, unsigned ByteOffset, SharedColorVector ColorVector, PhaseBlender *Parent);
	virtual ~PhaseBlenderInstance();
 	virtual void ProcessColor(glm::vec3 &color, const CellContext *cell) const = 0;
 	virtual void SetParameters(LuaApi::TableDispatcher &);
 	virtual void InitStep();
 	SimulationParameters* GetSimulationParameters() const { return m_Parametrs; }
 	DefineREADAccesPTR(Parent, PhaseBlender)
protected:
 	SimulationParameters *m_Parametrs;//??
	unsigned m_ByteOffset;
	SharedColorVector m_ColorVector;
	PhaseBlender *m_Parent;
private:
};

typedef std::shared_ptr<PhaseBlenderInstance> SharedPhaseBlenderInstance;

class PhaseBlender {
public:
	PhaseBlender(Simulation *Owner, const char* Name = 0);
	~PhaseBlender();

	SharedPhaseBlenderInstance& GetInstance();

	static std::shared_ptr<PhaseBlender> GetStdColorBlender(Simulation *Owner, StdColorBlender which, const std::string &Name);

	DefineREADAcces(Name, std::string)

//for scripts
	void SetParameter(const char* Name);
	void AddColor(float r, float g, float b) { AddColor4(r, g, b, 1.0f); }
	void AddColor4(float r, float g, float b, float a);
	void GenerateColorsByBits(unsigned BitsPerColor);
	void GenerateColors(int count);
	void SetMode(int Mode);
	void SetParameters(LuaApi::TableDispatcher &);
protected:
	std::string m_ParamName;
	std::string m_Name;
	Simulation *m_Sim;
	PhaseBlenderMode m_Mode;
	SharedColorVector m_ColorVector;
	PhaseBlenderInstance *ConstructInstance();
	SharedPhaseBlenderInstance m_Instance;
};
typedef std::shared_ptr<PhaseBlender> SharedPhaseBlender;

#endif //PHASEBLENDER_H_
