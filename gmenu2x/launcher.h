#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <string>
#include <vector>


class Launcher
{
public:
	Launcher(std::vector<std::string> const& commandLine,
			 bool consoleApp = true);
	Launcher(std::vector<std::string> && commandLine,
			 bool consoleApp = true);

	void exec();

private:
	std::vector<std::string> commandLine;
	bool consoleApp;
};

#endif // LAUNCHER_H
