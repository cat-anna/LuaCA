#include "App.h"

int main(int argc, char** argv) {
	int res = 1;
	try {
		res = (new App::cApplication(argc, argv))->Run();
	}
	catch(bool){
		cerr << "boolean value thrown!\n";
	}
	catch (const char * Msg){
		cerr << "FATAL ERROR! '" << Msg << "'\n";
	}
	catch (string & Msg){
		cerr << "FATAL ERROR! '" << Msg << "'\n";
	}
	catch (std::exception &E){
		cerr << "FATAL ERROR! '" << E.what() << "'\n";
	}
	catch (...){
		cerr << "UNKNOWN FATAL ERROR!";
	}
	App::cApplication::DeleteInstance();
	return res;
}
