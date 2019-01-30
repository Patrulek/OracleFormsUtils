#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "FAPIUtil.h"
#include "FAPILogger.h"

using namespace std;
using namespace CPPFAPIWrapper;

void startCmd(string _cmd);
string getType(string _filepath);

int main(int argc, char ** argv)
{
	if (argc < 5) {
		cout << "Run with arguments: -userid -filepath1 -filepath2 -win_name1 -win_name2\n\n"
			<< "Filepath: path to .fmb/.pll/.mmb/.rdf file to compare\n"
			<< "Win_name: name displayed in comparing program\n"
			<< "Userid: database connection string\n";
		return -1;
	}

	FAPILogger::changePath("Comparer.log");
	FAPILogger::setLevel(easylogger::LogLevel::LEVEL_DEBUG);

	vector<string> args = { argv[1], argv[2], argv[3], argv[4], "" };
	if (argc == 6)
		args[4] = argv[5];

	string cmd = "ifcmp60.exe module=" + args[0] + " module_type=" + getType(args[0])
		+ " batch=YES window_state=MINIMIZE forms_doc=YES";
	string cmd2 = "ifcmp60.exe module=" + args[1] + " module_type=" + getType(args[1])
		+ " batch=YES window_state=MINIMIZE forms_doc=YES";

	if (args[4] != "") {
		cmd += " logon=YES userid=" + args[4];
		cmd2 += " logon=YES userid=" + args[4];
	}
	else {
		cmd += " logon=NO";
		cmd2 += " logon=NO";
	}

	string cmd3 = "c:/\"program files\"/winmerge/winmergeu " + moduleNameFromPath(args[0]) + ".txt " 
		+ moduleNameFromPath(args[1]) + ".txt /dl \"" + args[2] + "\" /dr \"" + args[3] + "\"";

	thread t1(startCmd, cmd);
	startCmd(cmd2);
	t1.join();

	startCmd(cmd3);

    return 0;
}

string getType(string _filepath) {
	if (toUpper(_filepath).find(".FMB") != string::npos)
		return "FORM";
	else if (toUpper(_filepath).find(".PLL") != string::npos)
		return "LIBRARY";
	else if (toUpper(_filepath).find(".MMB") != string::npos)
		return "MENU";
	else if (toUpper(_filepath).find(".RDF") != string::npos)
		return "REPORT";

	throw std::runtime_error("Invalid file extension!");
}

void startCmd(string _cmd) {
	FAPILogger::debug(_cmd);
	system(_cmd.c_str());
}