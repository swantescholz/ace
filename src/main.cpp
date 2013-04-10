
 

//##########################################################
//*
#include "Ace.h"

void init(int argc, char **argv);
int main( int argc, char **argv) {
	cout << "START" << endl;
	init(argc,argv);
	cout << "END" << endl;
	return 0;
}


void init(int argc, char **argv) {
	if(argc > 1) {
		std::string arg1 = std::string(argv[1]);
		if(util::stringBeginsWith(arg1, "--self")) {
			int num = util::lex<int>(arg1.substr(6));
			selfCall(num);
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
	catch (const ace::Exception& ex) {
		std::cout << "BLUB" << std::endl;
		std::cout << "An error ocurred:\n" << ex.toString() << std::endl;
	}
	delete win; win = nullptr;
}//*/
