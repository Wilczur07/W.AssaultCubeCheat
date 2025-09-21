#include "Cheat.h"

//Vars
Player* localPlayer = nullptr;
Player** entList = nullptr;


//MISC
bool c_InfHealth = FALSE;
bool c_NoRecoil = TRUE;

//AIMBOT
bool c_Aimbot_Enabled = TRUE;
int c_Aimbot_Fov = 15;

//TRIGGERBOT
bool c_TriggerBot_Enabled = FALSE;
int c_TriggerBot_Delay = 100;


//ESP
bool c_Esp_Enabled = TRUE;
bool c_Esp_Name = TRUE;
bool c_Esp_Weapon = TRUE;
bool c_Esp_Distance = TRUE;
bool c_Esp_Health = TRUE;
bool c_Esp_Box = TRUE;
bool c_Esp_Skeleton = TRUE;
bool c_Esp_ShowFriendlies = TRUE;


int windowHeight = 0;
int windowWidth = 0;

float viewMatrix[16] = {};

// ===== HELPERS =====
BOOL IsReadable(void* ptr)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (!VirtualQuery(ptr, &mbi, sizeof(mbi))) return FALSE;
	if (mbi.State != MEM_COMMIT) return FALSE;
	if (!(mbi.Protect & (PAGE_READWRITE | PAGE_READONLY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)))
		return FALSE;
	return TRUE;
}


Player* GetLocalPlayer()
{
	if (Modules::GameBase == 0x0)
	{
		return nullptr;
	}

	return *reinterpret_cast<Player**>(
		Modules::GameBase + Offsets::LocalPlayer
		);
}

BOOL IsPlayerPtrValid(Player* player)
{
	if (!player) return FALSE;

	int* hpPtr = reinterpret_cast<int*>((uintptr_t)player + Offsets::Player::Health);

	if (!IsReadable(hpPtr))
		return FALSE;

	int hp = *hpPtr; // safe to deref now
	return (hp > 0 && hp < 500);
}

Player** GetEntList()
{
	if (Modules::GameBase == 0x0)
		return nullptr;

	return *reinterpret_cast<Player***>(
		Modules::GameBase + Offsets::EntityList
		);
}


BOOL IsEntListValid(Player** entList)
{
	if (!entList) return FALSE;

	for (int i = 0; i < 32; i++)
	{
		Player* p = entList[i];
		if (!IsPlayerPtrValid(p)) continue;

		if (p->health > 0 && p->health < 200)
			return TRUE; // at least one valid player found
	}

	return FALSE;
}

// ===== VIEW MATRIX =====
BOOL ReadViewMatrix()
{
	if (Modules::GameBase == 0x0)
		return FALSE;

	uintptr_t addr = Modules::GameBase + Offsets::ViewMatrix;
	if (!IsReadable(reinterpret_cast<void*>(addr)))
		return FALSE;

	memcpy(viewMatrix, reinterpret_cast<void*>(addr), sizeof(viewMatrix));
	return TRUE;
}