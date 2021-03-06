#include <iostream>
#include <fstream>
#include <algorithm>

#include "D2FDEF.H"
#include "FAPIWrapper.h"
#include "FAPIModule.h"
#include "FAPIContext.h"
#include "FAPILogger.h"
#include "FormsObject.h"
#include "FAPIUtil.h"

using namespace CPPFAPIWrapper;
using namespace std;

int main(int argc, char ** argv)
{
	vector<string> fmbs;
	vector<string> args;
	string lib_name;
	bool compile = false;
	string conn = "";
	FAPILogger::changePath("LibAttacher.log");

	if (argc < 3) {
		cout << "Run with arguments: -filepath -lib_name -check_if_compile[N] -connstring[NULL]\n\n"
			<< "Filepath: .fmb file or .txt file with list of .fmb paths (in separated lines)\n"
			<< "Lib_name: name of .pll library\n"
			<< "Check_if_compile (default N): check compilation correctness after library attachment (Y/N) (together with connstring argument)\n"
			<< "Connstring (default NULL): database connection string, eg. scott/tiger@db\n";
		return -1;
	}

	for (int i = 0; i < argc; ++i)
		args.emplace_back(string(argv[i]));

	if ( toUpper(args[1]).find(".FMB") != string::npos)
		fmbs.emplace_back(args[1]);
	else if (toUpper(args[1]).find(".TXT") != string::npos) {
		ifstream file(args[1]);
		string line;

		while (getline(file, line))
			fmbs.emplace_back(line);

		file.close();
	}
	else {
		FAPILogger::error("Incorrect file extension!");
		return -1;
	}

	lib_name = args[2];

	if (args.size() > 3)
		compile = args[3] == "Y";

	if (compile && args.size() < 5) {
		FAPILogger::error("'Check_if_compile' argument needs to be set together with 'connstring'");
		return -1;
	}

	if (args.size() > 4)
		conn = args[4];

	auto ctx = createContext(conn);

	for (string fmb : fmbs) {
		FAPILogger::info("Attaching library " + lib_name + " to form module " + fmb);
		bool cont = false;
		bool connected = false;

		try {
			ctx->loadModule(fmb);
			auto module = ctx->getModule(fmb);

			if ( module->hasObject(D2FFO_ATT_LIB, lib_name)) {
				FAPILogger::warn("Library " + lib_name + " already attached");
				continue;
			}

			module->attachLib(lib_name);

			if (compile) {
				if (ctx->isConnected())
					module->compileModule();
				else
					FAPILogger::warn("Couldn't check compilation. No connecton with database!");
			}

			module->saveModule(fmb);
			FAPILogger::info("Library successfully attached");
		}
		catch (exception & ex) {
			FAPILogger::error(ex.what());
		}
	}

	return 0;
}
