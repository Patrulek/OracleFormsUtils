#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_set>

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
	bool compile = false;
	string conn = "";
	FAPILogger::changePath("LibCleaner.log");
	FAPILogger::setLevel(easylogger::LogLevel::LEVEL_TRACE);

	if (argc < 2) {
		cout << "Run with arguments: -filepath -check_if_compile[N] -connstring[NULL]\n\n"
			<< "Filepath: .fmb file or .txt file with list of .fmb paths (in separated lines)\n"
			<< "Check_if_compile (default N): check compilation correctness after library attachment (Y/N) (together with connstring argument)\n"
			<< "Connstring (default NULL): database connection string, eg. scott/tiger@db\n";
		return -1;
	}

	for (int i = 0; i < argc; ++i)
		args.emplace_back(string(argv[i]));

	if (toUpper(args[1]).find(".FMB") != string::npos)
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

	if (args.size() > 2)
		compile = args[2] == "Y";

	if (compile && args.size() < 4) {
		FAPILogger::error("'Check_if_compile' argument needs to be set together with 'connstring'");
		return -1;
	}

	if (args.size() > 3)
		conn = args[3];

	auto ctx = createContext(conn);

	for (string fmb : fmbs) {
		unordered_set<string> lib_names;
		FAPILogger::info("Cleaning libraries in form module " + fmb);
		bool cont = false;
		bool connected = false;

		try {
			ctx->loadModule(fmb);
			auto module = ctx->getModule(fmb);
			auto libs = module->getAttachedLibraries();
			
			for (auto lib : libs)
				lib_names.insert(lib->getName());

			for (auto lib : lib_names) {
				module->detachLib(lib);
				module->attachLib(lib);
			}

			if (compile) {
				if (ctx->isConnected())
					module->compileModule();
				else
					FAPILogger::warn("Couldn't check compilation. No connecton with database!");
			}

			module->saveModule(fmb);
			FAPILogger::info("Libraries successfully cleaned!");
		}
		catch (exception & ex) {
			FAPILogger::error(ex.what());
		}
	}

	return 0;
}
