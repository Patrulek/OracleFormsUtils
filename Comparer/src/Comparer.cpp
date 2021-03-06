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
string findWinMerge();
string findTortoiseMerge();

int main(int argc, char ** argv)
{
	if (argc < 5) {
		cout << "Run with arguments: -filepath1 -filepath2 -win_name1 -win_name2 -userid\n\n"
			<< "Filepath: path to .fmb/.pll/.mmb/.rdf file to compare\n"
			<< "Win_name: name displayed in comparing program\n"
			<< "Userid: database connection string\n";
		return -1;
	}

	FAPILogger::changePath("Comparer.log");
	FAPILogger::setLevel(easylogger::LogLevel::LEVEL_DEBUG);

	vector<string> args = { argv[1], argv[2], argv[3], argv[4], "", findWinMerge() };
	if (argc >= 6)
		args[4] = argv[5];

	string merge_args = "\"" + moduleNameFromPath(args[0]) + ".txt\" \""
		+ moduleNameFromPath(args[1]) + ".txt\" /dl \"" + args[2] + "\" /dr \"" + args[3] + "\"";

	if (args[5] == "") {
		args[5] = findTortoiseMerge();
		merge_args = "/base:\"" + moduleNameFromPath(args[0]) + ".txt\" /mine:\"" + moduleNameFromPath(args[1]) + "\"";

		if( args[5] == "" )
			throw std::runtime_error("No Win/Tortoise Merge installed!");
	}

	string type = getType(args[0]);

	string cmd = "ifcmp60.exe module=\"" + args[0] + "\" module_type=" + type
		+ " batch=YES window_state=MINIMIZE forms_doc=YES";
	string cmd2 = "ifcmp60.exe module=\"" + args[1] + "\" module_type=" + type
		+ " batch=YES window_state=MINIMIZE forms_doc=YES";

	if (type == "REPORT") {
		cmd = "rwcon60.exe DTYPE=rexfile STYPE=rdffile OVERWRITE=YES BATCH=YES WINDOW_STATE=MINIMIZE SOURCE=\"" + args[0] + "\"";
		cmd2 = "rwcon60.exe DTYPE=rexfile STYPE=rdffile OVERWRITE=YES BATCH=YES WINDOW_STATE=MINIMIZE SOURCE=\"" + args[1] + "\"";
	}
	else {
		if (args[4] != "") {
			cmd += " logon=YES userid=" + args[4];
			cmd2 += " logon=YES userid=" + args[4];
		}
		else {
			cmd += " logon=NO";
			cmd2 += " logon=NO";
		}
	}

	string cmd3 = args[5] + merge_args;
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


string findWinMerge() {
	if (fileExists("c:/program files/winmerge/winmergeu.exe"))
		return "c:/\"program files\"/winmerge/winmergeu.exe ";

	if (fileExists("c:/program files (x86)/winmerge/winmergeu.exe"))
		return "c:/\"program files (x86)\"/winmerge/winmergeu.exe ";

	string appdata = getenv("APPDATA");
	FAPILogger::debug("Appdata: " + appdata);

	if (fileExists(appdata + "winmerge/winmergeu.exe"))
		return appdata + "winmerge/winmergeu.exe";

	string path = getenv("PATH");
	FAPILogger::debug("Path: " + path);

	vector<string> strs = splitString(path);

	for (string str : strs)
		if (fileExists(str + "winmerge/winmergeu.exe"))
			return str + "winmerge/winmergeu.exe";

	return "";
}

string findTortoiseMerge() {
	if (fileExists("c:/program files/tortoisesvn/bin/tortoisemerge.exe"))
		return "c:/\"program files\"/tortoisesvn/bin/tortoisemerge.exe ";

	if (fileExists("c:/program files (x86)/tortoisesvn/bin/tortoisemerge.exe"))
		return "c:/\"program files (x86)\"/tortoisesvn/bin/tortoisemerge.exe ";

	string appdata = getenv("APPDATA");
	FAPILogger::debug("Appdata: " + appdata);

	if (fileExists(appdata + "tortoisesvn/bin/tortoisemerge.exe"))
		return appdata + "tortoisesvn/bin/tortoisemerge.exe";

	string path = getenv("PATH");
	FAPILogger::debug("Path: " + path);

	vector<string> strs = splitString(path);

	for (string str : strs)
		if (fileExists(str + "tortoisesvn/bin/tortoisemerge.exe"))
			return str + "tortoisesvn/bin/tortoisemerge.exe";

	return "";
}