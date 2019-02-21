#include "common\skyscript.h"
#include "common\obscript.h"
#include "common\types.h"
#include "common\enums.h"
#include "common\plugin.h"
#include <math.h>

#include <filesystem>
#include <fstream>
#include <streambuf>
namespace fs = std::filesystem;

#define CONFIG_FILE "dotate_pay_spawner.ini"
#define SCR_NAME "DotatePay spawner"


bool fileExists(const std::string& filename)
{
	struct stat buf;
	if (stat(filename.c_str(), &buf) != -1)
	{
		return true;
	}
	return false;
}


void PrintMsg(bool isEnabled)
{
	if (isEnabled)
	{
		PrintNote("[%s] enabled", SCR_NAME);
	}
	else
	{
		PrintNote("[%s] disabled", SCR_NAME);
	}
}

void main()
{
	bool isEnabled = false;
	BYTE key = IniReadInt(CONFIG_FILE, "main", "key", 0);
	int spawnInterval = IniReadInt(CONFIG_FILE, "main", "spawnInterval", 5);

	PrintNote("%s started", SCR_NAME);
	PrintNote("Press '%s' to enable/disable", GetKeyName(key).c_str());

	std::string path = fs::current_path().string() + "\\DonateSpawn\\";

	while (TRUE)
	{
		if (GetKeyPressed(key))
		{
			isEnabled = !isEnabled;
			PrintMsg(isEnabled);

			Wait(500);
		}

		if (isEnabled && fileExists(path))
		{
			for (const auto & entry : fs::directory_iterator(path))
			{
				if (GetKeyPressed(key))
				{
					isEnabled = !isEnabled;
					PrintMsg(isEnabled);
				}

				if (isEnabled && entry.path().extension() == ".spawn")
				{
					std::ifstream t(entry.path());
					std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
					t.close();

					char * writable = new char[str.size() + 1];
					std::copy(str.begin(), str.end(), writable);
					writable[str.size()] = '\0';

					ExecuteConsoleCommand(writable, NULL);
					PrintNote("[%s] spawned", writable);

					delete[] writable;

					fs::remove(entry.path());

					Wait(spawnInterval * 1000);
				}
			}
		}

		Wait(0); // In order to switch between script threads Wait() must be called anyway
	}
}




