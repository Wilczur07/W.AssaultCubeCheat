#pragma once
#include <cstdint>
#include "Utils/IMem.hpp"
#include "Utils/Geom.hpp"

#define PAD(size) char _pad_##__COUNTER__[size]
#define PAD_AT(from, to) char _pad_##from[(to) - (from)]

//Game specific vars for rendering esp etc..

//
// Base Addresses (modules)
//
namespace Modules
{
    // These are filled at runtime (use GetModuleHandle or GetModuleBase)
    inline uintptr_t GameBase = 0;
}

//
// Offsets
//
namespace Offsets
{
    //ac_client.exe+195404
    //ac_client.exe+18AC00
    //ac_client.exe+17E254
    //ac_client.exe+17E0A8
    constexpr uintptr_t LocalPlayer = 0x17E0A8;

    // It containts everything but our player from what i seem ac_client.exe+18AC04
    constexpr uintptr_t EntityList = 0x18AC04;

    constexpr uintptr_t ViewMatrix = 0x17DFD0;

    // Example: multi-level pointer chain
    // CE: game.exe + 0x1234 → 0x20 → 0x14 → health
    // You’d resolve it like: base + 0x1234 -> deref -> +0x20 -> deref -> +0x14
    constexpr uintptr_t ExampleChainBase = 0x1234;
    constexpr uintptr_t ExampleChainOffsets[2] = { 0x20, 0x14 };
    // You’ll use a function to walk this chain.

    // Player struct relative offsets
    namespace Player
    {
        constexpr uintptr_t Position = 0x4;
        constexpr uintptr_t Position2 = 0x28;
        //0-360 values
        constexpr uintptr_t Yaw = 0x34;
        //(-90)-90 values
        constexpr uintptr_t Pitch = 0x38;
        constexpr uintptr_t SomethingRelatedToShooting = 0x40;
        constexpr uintptr_t ActualJumpHeight = 0x48;
        //Y Is related to shift like crouching
        constexpr uintptr_t SomeVec3 = 0x4C;
        //one byte value 0/1 player on the ground
        constexpr uintptr_t IsGrounded = 0x5D;
        constexpr uintptr_t Health = 0xEC;
        constexpr uintptr_t HoldedWeapon = 0x1EC;
        //QWORD long nickname
        constexpr uintptr_t Nickname = 0x204;
        constexpr uintptr_t Team = 0x30C;
        //constexpr uintptr_t Armor = 0xFC;
        //constexpr uintptr_t Position = 0x34;
        //constexpr uintptr_t Team = 0x32C;
    }
}

//
// Structs
//

struct Player
{
    PAD_AT(0x00, 0x04);
    Vec3 position;          // 0x04

    // 0x28 - Position2
    PAD_AT(0x10, 0x28);
    Vec3 position2;         // 0x28

    float yaw;              // 0x34
    float pitch;            // 0x38
    PAD_AT(0x3C, 0x40);
    int somethingShoot;     // 0x40
    PAD_AT(0x44, 0x48);
    float jumpHeight;       // 0x48
    Vec3 someVec3;          // 0x4C
    PAD_AT(0x58, 0x5C);
    // 0x5D - IsGrounded (byte)
    int isGrounded;     // 0x5D

    // 0xEC - Health
    PAD_AT(0x62, 0xEC);
    int health;             // 0xEC

    // 0x1EC - HoldedWeapon
    PAD_AT(0xF0, 0x1EC);
    int holdedWeapon; 

    PAD_AT(0x1F0, 0x205);
    char nickname[15];      // 0x204 (assuming 16 chars)

    // 0x30C - Team
    PAD_AT(0x214, 0x30C);
    int team;               // 0x30C
};

static_assert(offsetof(Player, position) == 0x04, "bad offset: position");
static_assert(offsetof(Player, position2) == 0x28, "bad offset: position2");
static_assert(offsetof(Player, yaw) == 0x34, "bad offset: yaw");
static_assert(offsetof(Player, pitch) == 0x38, "bad offset: yaw");
static_assert(offsetof(Player, somethingShoot) == 0x40, "bad offset: yaw");
static_assert(offsetof(Player, jumpHeight) == 0x48, "bad offset: yaw");
static_assert(offsetof(Player, someVec3) == 0x4C, "bad offset: someVec3");
static_assert(offsetof(Player, isGrounded) == 0x5C, "bad offset: isGrounded");
static_assert(offsetof(Player, health) == 0xEC, "bad offset: health");
static_assert(offsetof(Player, holdedWeapon) == 0x1EC, "bad offset: holdedWeapon");
static_assert(offsetof(Player, nickname) == 0x205, "bad offset: nickname");
static_assert(offsetof(Player, team) == 0x30C, "bad offset: team");