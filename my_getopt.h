#include <iostream>
#include <getopt.h>
#include <sstream>
#include "my_space.h"

class Options{
    public:
	string my_address = "";

        Options() : Options(0, NULL){}
	Options(int argc, char* argv[]) {
	    parse_options(argc, argv);
	}
        
        void parse_options(int argc, char* argv[]) {
	    for(int i = 0 ; i < argc ; i++) {
		string buffer = string(argv[i]);
		if(buffer == "-h") {
		    cout << "Usage : ./exec -a [server_address] -h [help]" << endl;
		    return;
		}
		else if(buffer == "-a") {
		    if(i + 1 >= argc) {
			cout << "Must insert your server address with options -a" << endl;
			return;
		    }
		    else {
			buffer = string(argv[i + 1]);
			++i;
		    }
		    my_address = buffer;
		    break;
		}
	    }
        }

    private:
        string options;
};
