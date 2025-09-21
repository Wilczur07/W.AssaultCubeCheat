#pragma once
#include "AssCube.h"
#include "Utils/IMem.hpp"
#include "Utils/Render.hpp"


struct PlayerInfo {
	uintptr_t base;
	int health;
	int team;
	Vec3 position;
	Vec3 position2;
	bool isDormant;
	bool isEnemy;
	std::string nickname;
};

// Global cheat state (declarations only)
extern Player* localPlayer;
extern Player** entList;

//MISC
extern bool c_InfHealth;
extern bool c_NoRecoil;

//AIMBOT
extern bool c_Aimbot_Enabled;
extern int c_Aimbot_Fov;

//TRIGGERBOT
extern bool c_TriggerBot_Enabled;
extern int c_TriggerBot_Delay;

//ESP
extern bool c_Esp_Enabled;
extern bool c_Esp_Name;
extern bool c_Esp_Distance;
extern bool c_Esp_Health;
extern bool c_Esp_Box;
extern bool c_Esp_ShowFriendlies;

//
extern int windowHeight;
extern int windowWidth;


extern float viewMatrix[16];

//HELPERS

Player* GetLocalPlayer();
Player** GetEntList();
BOOL IsPlayerPtrValid(Player* player);
BOOL IsEntListValid(Player** entList);

BOOL ReadViewMatrix();

//CHEATS
VOID CinfiniteHealth();
VOID Cesp(std::vector<PlayerInfo> tPlayers);
VOID CtriggerBot(std::vector<PlayerInfo> tPlayers);
VOID CAimBot(std::vector<PlayerInfo> tPlayers);


//BASE

inline VOID CheatLoop()
{
	if (!IsPlayerPtrValid(localPlayer))
	{
		localPlayer = GetLocalPlayer();
		return;
	}

	if (!IsEntListValid(entList))
	{
		entList = GetEntList();
		return;
	}

	if (!ReadViewMatrix())
		return;


	//MAIN ENT LOOP
	INT32 maxPlayers = 32; //todo find
	std::vector<PlayerInfo> players;

	for (int i = 0; i < maxPlayers; i++)
	{
		Player* p = entList[i]; 
		if (!IsPlayerPtrValid(p))
			continue;

		PlayerInfo info{};
		info.base = reinterpret_cast<uintptr_t>(p);
		info.health = p->health;
		info.team = p->team;
		info.position = p->position;
		info.position2 = p->position2;
		info.isEnemy = (localPlayer && p->team != localPlayer->team);
		info.nickname = std::string(p->nickname, strnlen(p->nickname, 15));

		players.push_back(info);
	}

	if (c_Esp_Enabled)
	{
		Cesp(players);
	}

	if (c_TriggerBot_Enabled)
	{
		CtriggerBot(players);
	}

	if (c_Aimbot_Enabled)
	{
		CAimBot(players);
	}

}

inline VOID InitModules()
{
	// get base address of game.exe once
	Modules::GameBase = reinterpret_cast<uintptr_t>(
		GetModuleHandleA("ac_client.exe")
		);
}


