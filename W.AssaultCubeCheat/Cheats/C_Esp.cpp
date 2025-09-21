#include <algorithm>

#include "../Cheat.h"

//consts
const INT32 GUM = 300;

const float PLAYER_HEIGHT = 1.25f;
const float PLAYER_WIDTH = 0.6f;
const float EYE_HEIGHT = 4.5f;

const float PLAYER_AR = PLAYER_HEIGHT / PLAYER_WIDTH;

const int ESP_FONT_HEIGHT = 15;
const int ESP_FONT_WIDTH = 9;

VOID Cesp(std::vector<PlayerInfo> tPlayers)
{
	if (tPlayers.size() == 0)
	{
		return;
	}

	for (auto pTarget : tPlayers)
	{
		float dist = pTarget.position.Distance(localPlayer->position);
		//float fontSize = std::clamp(24.0f - (dist * 0.05f), 6.0f, 14.0f);

		Vec3 head = pTarget.position;
		//head.z += 0.5f;

		Vec3 screenHead;
		if (!WorldToScreen(head, screenHead, viewMatrix, windowWidth, windowHeight))
			continue;

		pTarget.position2.z -= 1.5f;

		Vec3 screenFeet;
		if (!WorldToScreen(pTarget.position2, screenFeet, viewMatrix, windowWidth, windowHeight))
			continue;

		FLOAT scale = (GUM / dist) * 1.6f;
		float height = screenFeet.y - screenHead.y;
		float width = height * 0.3f; // adjust ratio for your model

		float boxHeight = screenFeet.y - screenHead.y;
		float fontSize = std::clamp(boxHeight * 0.2f, 10.0f, 20.0f);

		if (c_Esp_Name)
		{
			float xCoord = screenHead.x - width * 0.5f;
			float yCoord = screenHead.y - height - 5.0f;
			yCoord += height * 0.55f;

			Render::Text(Vec2(xCoord, yCoord), pTarget.isEnemy ? COLOR_RED : COLOR_GREEN, pTarget.nickname, fontSize);
		}

		if (c_Esp_Box)
		{
			float x = screenHead.x - width * 0.5f;
			float y = screenHead.y - height * 0.5f; y += height * 0.40f;

			Render::Box(
				Vec2(x, y),
				Vec2(width, height),
				pTarget.isEnemy ? COLOR_RED : COLOR_GREEN,
				1);
		}

		if (c_Esp_Distance)
		{
			float xCoord = screenFeet.x - width * 0.5f;
			float yCoord = screenFeet.y + height * 0.5f; 
			yCoord -= height * 0.45f;

			Render::Text(Vec2(xCoord, yCoord), pTarget.isEnemy ? COLOR_RED : COLOR_GREEN, std::to_string((int)std::round(dist)) + " M", fontSize);
		}

		if (c_Esp_Health)
		{
			// full bar height = player box height
			float barHeight = height;
			float barWidth = width * 0.1f; // thin vertical bar

			// position bar to the left of the box
			float x = screenHead.x - width * 0.6f;
			float y = screenHead.y - height * 0.5f; y += height * 0.40f;

			// clamp health percentage
			float healthPercent = std::clamp(pTarget.health / 100.0f, 0.0f, 1.0f);

			// background (grey full bar)
			Render::BoxFilled(
				Vec2(x, y),
				Vec2(barWidth, barHeight),
				COLOR_DARKGREY,
				1);


			// filled health (from bottom up)
			Render::BoxFilled(
				Vec2(x, y + barHeight * (1.0f - healthPercent)),
				Vec2(barWidth, barHeight * healthPercent),
				COLOR_MAGENTA,
				1);
		}

	}
}