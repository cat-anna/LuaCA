/*
 * ApiGenerator.cpp
 *
 *  Created on: 12 maj 2014
 *      Author: Immethis
 */

#include "ApiGenerator.h"

#include "ApiDefAutoGen.h"
typedef ApiDefAutoGen::Namespace ApiInitializer;
#include "Simulation.h"
#include "SimApi.h"

namespace ApiGenerator {

void PrintApi(std::ostream &) {

	ApiDefAutoGen::Namespace::Initialize("ApiDef.xml");
	{
		ApiDefAutoGen::Namespace::SetRoot("Primary");
		SimApi::InitSimApi(ApiDefAutoGen::Namespace::Begin(0));
	}
	ApiDefAutoGen::Namespace::Finalize();
//	cout << "Generating 'ApiOutCall.xml'" << endl;
//	ApiDefAutoGen::OutCallsPicker::WriteOutCalls("ApiOutCall.xml");

}

} /* namespace ApiGenerator */
