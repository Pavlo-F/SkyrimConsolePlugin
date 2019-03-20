#include "common\skyscript.h"
#include "common\obscript.h"
#include "common\types.h"
#include "common\enums.h"
#include "common\plugin.h"
#include <math.h>

#include <filesystem>
#include <fstream>
#include <streambuf>
#include <thread>

namespace fs = std::filesystem;

#define CONFIG_FILE "dotate_pay_spawner.ini"
#define SCR_NAME "DotatePay spawner"

bool _isEnabled = true;

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


std::vector<std::string> Split(const std::string &text, std::string delimiter) {
	std::vector<std::string> tokens;
	std::size_t start = 0, end = 0;
	while ((end = text.find(delimiter, start)) != std::string::npos) {
		tokens.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
	return tokens;
}


void SpawnTask(std::string path, BYTE keyEnable, BYTE keyDisable, int spawnInterval)
{
		if (GetKeyPressed(keyEnable))
		{
			_isEnabled = true;
			PrintMsg(_isEnabled);
		} 
		else if (GetKeyPressed(keyDisable))
		{
			_isEnabled = false;
			PrintMsg(_isEnabled);
		}

		if (_isEnabled && fileExists(path))
		{
			CActor *player = Game::GetPlayer();
			if (Actor::IsDead(player))
			{
				return;
			}

			for (const auto & entry : fs::directory_iterator(path))
			{
				if (GetKeyPressed(keyEnable))
				{
					_isEnabled = true;
					PrintMsg(_isEnabled);
				}
				else if (GetKeyPressed(keyDisable))
				{
					_isEnabled = false;
					PrintMsg(_isEnabled);
				}

				if (_isEnabled && entry.path().extension() == ".spawn")
				{
					std::ifstream t(entry.path());
					std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
					t.close();

					if (str.find("save ") != std::string::npos)
					{
						Game::RequestSave();
					}
					else
					{
						std::vector<std::string> comandAndWhat = Split(str, ";");
						std::string comand = comandAndWhat[0];
						std::string what = "noname";

						if (comandAndWhat.capacity() > 1)
						{
							what = comandAndWhat[1];
						}

						char * exeCcomand = new char[comand.size() + 1];
						std::copy(comand.begin(), comand.end(), exeCcomand);
						exeCcomand[comand.size()] = '\0';

						ExecuteConsoleCommand(exeCcomand, NULL);
						delete[] exeCcomand;


						std::string msg = comand + " by " + what;

						char * print = new char[msg.size() + 1];
						std::copy(msg.begin(), msg.end(), print);
						print[msg.size()] = '\0';

						PrintNote("spawned %s", print);
						delete[] print;
					}

					fs::remove(entry.path()); 
					Wait(spawnInterval * 1000);
				}
			}
		}
}


void main()
{
	_isEnabled = true;

	BYTE keyEnable = IniReadInt(CONFIG_FILE, "main", "keyEnable", 36);
	BYTE keyDisable = IniReadInt(CONFIG_FILE, "main", "keyDisable", 35);
	int spawnInterval = IniReadInt(CONFIG_FILE, "main", "spawnInterval", 5);

	PrintNote("Press '%s' to ENABLE and '%s' to DISABLE", GetKeyName(keyEnable).c_str(), GetKeyName(keyDisable).c_str());


	if (_isEnabled)
	{
		PrintNote("%s ENABLED", SCR_NAME);
	}
	else
	{
		PrintNote("%s DISABLED", SCR_NAME);
	}

	std::string path = fs::current_path().string() + "\\DonateSpawn\\";

	while (TRUE)
	{
		try
		{
			SpawnTask(path, keyEnable, keyDisable, spawnInterval);
		}
		catch (...)
		{
			PrintNote("Error spawn");
		}

		Wait(0); // In order to switch between script threads Wait() must be called anyway
	}
	
}
