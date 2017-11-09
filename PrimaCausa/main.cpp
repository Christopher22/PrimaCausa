#include "Controller/engine.h"

#include <crtdbg.h>

int main(int argc, char* argv[]) {
	// Use _CrtSetBreakAlloc( ... ); to find memory leaks!
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	if ( !Controller::Engine::Initialise(argc, argv, 800, 600, 60) ) {
		return 1;
	}

	Controller::Engine::GetInstance()->Run();
	Controller::Engine::DestroyInstance();
	return 0;
}