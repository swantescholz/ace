
 

//##########################################################
//*
#include "ace/ace.h"
#include "ace/Ipc.h"
#include "ace/StackTrace.h"
using namespace ace;

void init(int argc, char **argv);
int main( int argc, char **argv) {
	cout << "START" << endl;
	init(argc,argv);
	cout << "END" << endl;
	return 0;
}

void init(int argc, char **argv) {
	
	//stacktrace.init();
	
	if(argc > 1) {
		String arg1 = std::string(argv[1]);
		if(arg1.startsWith("--self")) {
			int num = util.lex<int>(arg1.substr(6));
			ipc.selfCall(num);
			return;
		}
	}
	
	cout << "aceExecName: " <<  argv[0] << endl;
	Gtk::Main main_obj(argc, argv);
	ace::Application *win = nullptr;
	win = new ace::Application(argv[0]);
	try {
		main_obj.run(*win);
	}
	catch (shared_ptr<AssertionException> ex) {
		std::cout << ex->toString() << std::endl;
	}
	delete win; win = nullptr;
}//*/
