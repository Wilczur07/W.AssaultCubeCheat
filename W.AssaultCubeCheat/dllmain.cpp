

#include <algorithm>
#include "Includes.h"

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hdc);

twglSwapBuffers owglSwapBuffers;

BYTE originalBytes[5] = { 0 };
BYTE* targetSwapBuffers = nullptr;  // the real address of wglSwapBuffers


HMODULE ourModule;
WNDPROC oWndProc;
static HWND Window = NULL;

volatile bool g_UnloadRequested = false;
volatile bool g_Unhooked = false;
volatile bool g_UnloadThreadStarted = false;


int init = false;
bool show = false;

// MENU STATE
static int selectedTab = 0;    
static int selectedOption = 0; 
static bool inOptions = false;


const char* tabs[] = { "ESP", "Aimbot", "Triggerbot [TODO]", "Misc" };

const char* espOpts[] = { "Enable ESP", "Show Distance", "Show Health", "Show Box", "Show friendlies" };

const char* aimbotOpts[] = { "Enable Aimbot", "FOV", "Smooth" };
const char* trigOpts[] = { "Enable Triggerbot", "Delay" };
const char* miscOpts[] = { "Bunnyhop", "No Recoil" };

// PATCH: clean ImGui safely (only if initialized)
static void CleanupImGuiIfAny()
{
    if (init)
    {
        // nie zaczynaj nowych frame'ów, po prostu sprzątnij backends i context
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        init = FALSE;
    }
}

// PATCH: restore original 5 bytes + flush icache
static void RestoreOriginalSwap()
{
    if (!g_Unhooked && targetSwapBuffers)
    {
        DWORD oldProtect;
        VirtualProtect(targetSwapBuffers, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy(targetSwapBuffers, originalBytes, 5);
        FlushInstructionCache(GetCurrentProcess(), targetSwapBuffers, 5);
        VirtualProtect(targetSwapBuffers, 5, oldProtect, &oldProtect);
        g_Unhooked = true;
    }
}

// PATCH: delayed unload from a separate thread (not the render/window thread)
static DWORD WINAPI DelayedFreeThread(LPVOID)
{
    // poczekaj aż aktualna klatka się domknie i nic już nie siedzi w naszej DLL
    Sleep(150);
    FreeLibraryAndExitThread(ourModule, 0);
    return 0;
}

#define UPKEY VK_OEM_6
#define DOWNKEY VK_OEM_4
#define LEFTKEY VK_OEM_1
#define RIGHTKEY VK_OEM_7

#define IM_COL_RED ImColor(IM_COL32(255,0,0,255))
#define IM_COL_WHITE ImColor(IM_COL32(255,255,255,255))


BOOL __stdcall hkwglSwapBuffers(HDC hdc)
{
    // init once
    if (init == FALSE)
    {
        glewExperimental = GL_TRUE;
        if (glewInit() == GLEW_OK)
        {
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();

            //Doesnt work
            //io.KeyMap[ImGuiKey_DownArrow] = VK_OEM_4;
            //io.KeyMap[ImGuiKey_UpArrow] = VK_OEM_6;
            //io.KeyMap[ImGuiKey_RightArrow] = VK_OEM_1;
            //io.KeyMap[ImGuiKey_LeftArrow] = VK_OEM_7;

            io.Fonts->AddFontDefault();

            io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
            ImGui_ImplWin32_Init(Window);
            ImGui_ImplOpenGL3_Init();
            init = TRUE;
        }

        
    }

    // toggle ui
    if (GetAsyncKeyState(VK_INSERT) & 1)
        show = !show;

    // request unload (tylko ustaw flagę)
    if (GetAsyncKeyState(VK_END) & 1)
        g_UnloadRequested = true;

    // jeśli prosimy o unload a jeszcze nie odpięliśmy hooka -> zrób "miękkie" odpięcie
    if (g_UnloadRequested && !g_Unhooked)
    {
        // 1) przywróć WndProc
        if (Window && oWndProc)
            SetWindowLongPtr(Window, GWL_WNDPROC, (LONG_PTR)oWndProc);

        // 2) przestań rysować i sprzątnij ImGui
        CleanupImGuiIfAny();

        // 3) przywróć prolog wglSwapBuffers
        RestoreOriginalSwap();

        // 4) uruchom opóźnione FreeLibrary w osobnym wątku (tylko raz)
        if (!g_UnloadThreadStarted)
        {
            g_UnloadThreadStarted = true;
            CreateThread(nullptr, 0, DelayedFreeThread, nullptr, 0, nullptr);
        }
    }

    // rysowanie tylko jeśli nadal jesteśmy zahookowani i mamy aktywny ImGui
    if (!g_UnloadRequested && init)
    {


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        CheatLoop();

        if (show)
        {
            ImGui::SetNextWindowSize(ImVec2(300, 300));
            ImGui::Begin("W | Assault Cube Hack v0.3", 0, ImGuiWindowFlags_AlwaysAutoResize);
            {
                // Left side = Tabs
                ImGui::BeginChild("LeftMenu", ImVec2(100, 0), true);
                for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
                {
                    bool isSelected = (selectedTab == i);
                    if (ImGui::Selectable(tabs[i], isSelected))
                    {
                        selectedTab = i;
                        inOptions = true;   // when clicked with mouse
                        selectedOption = 0; // reset option index
                    }

                    // Draw highlight if we’re navigating with arrows
                    if (!inOptions && isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndChild();

                ImGui::SameLine();

                // Right side = Options
                ImGui::BeginChild("RightContent", ImVec2(0, 0), true);

                if (inOptions)
                {
                    switch (selectedTab)
                    {
	                    case 0: //ESP

                            ImGui::TextColored(
                                (selectedOption == 0 ? IM_COL_RED : IM_COL_WHITE),
                                "%s %s: %s",
                                (selectedOption == 0 ? ">" : " "),
                                espOpts[0],
                                c_Esp_Enabled ? "ON" : "OFF"
                            );

                            ImGui::TextColored(
                                (selectedOption == 1 ? IM_COL_RED : IM_COL_WHITE),
                                "%s %s: %s",
                                (selectedOption == 1 ? ">" : " "),
                                espOpts[1],
                                c_Esp_Distance ? "ON" : "OFF"
                            );

                            ImGui::TextColored(
                                (selectedOption == 2 ? IM_COL_RED : IM_COL_WHITE),
                                "%s %s: %s",
                                (selectedOption == 2 ? ">" : " "),
                                espOpts[2],
                                c_Esp_Health ? "ON" : "OFF"
                            );


                            ImGui::TextColored(
                                (selectedOption == 3 ? IM_COL_RED : IM_COL_WHITE),
                                "%s %s: %s",
                                (selectedOption == 3 ? ">" : " "),
                                espOpts[3],
                                c_Esp_Box ? "ON" : "OFF"
                            );


                            ImGui::TextColored(
                                (selectedOption == 4 ? IM_COL_RED : IM_COL_WHITE),
                                "%s %s: %s",
                                (selectedOption == 4 ? ">" : " "),
                                espOpts[4],
                                c_Esp_ShowFriendlies ? "ON" : "OFF"
                            );

                            if (selectedOption == 0 && ImGui::IsKeyPressed(RIGHTKEY)) c_Esp_Enabled = !c_Esp_Enabled;
                            if (selectedOption == 1 && ImGui::IsKeyPressed(RIGHTKEY)) c_Esp_Distance = !c_Esp_Distance;
                            if (selectedOption == 2 && ImGui::IsKeyPressed(RIGHTKEY)) c_Esp_Health = !c_Esp_Health;
                            if (selectedOption == 3 && ImGui::IsKeyPressed(RIGHTKEY)) c_Esp_Box = !c_Esp_Box;
                            if (selectedOption == 4 && ImGui::IsKeyPressed(RIGHTKEY)) c_Esp_ShowFriendlies = !c_Esp_ShowFriendlies;


                            break;

	                    case 1: //Aimbot

                            ImGui::TextColored(
                                (selectedOption == 0 ? IM_COL_RED : IM_COL_WHITE),
                                "%s %s: %s",
                                (selectedOption == 0 ? ">" : " "),
                                "Aimbot",
                                c_Aimbot_Enabled ? "ON" : "OFF"
                            );

                            ImGui::TextColored(
                                (selectedOption == 1 ? IM_COL_RED : IM_COL_WHITE),
                                "%s %s: %d",
                                (selectedOption == 1 ? ">" : " "),
                                "FOV",
                                c_Aimbot_Fov
                            );

                            if (selectedOption == 0 && ImGui::IsKeyPressed(RIGHTKEY)) c_Aimbot_Enabled = !c_Aimbot_Enabled;
                            if (selectedOption == 1 && ImGui::IsKeyPressed(RIGHTKEY)) c_Aimbot_Fov = min(c_Aimbot_Fov + 1, 180); 
                            if (selectedOption == 1 && ImGui::IsKeyPressed(LEFTKEY))  c_Aimbot_Fov = max(c_Aimbot_Fov - 1, 1);  


							break;
                        case 2: //Triggerbot

                            ImGui::TextColored(
                                (selectedOption == 0 ? IM_COL_RED : IM_COL_WHITE),
                                "%s %s: %s",
                                (selectedOption == 0 ? ">" : " "),
                                "Trigger bot",
                                c_TriggerBot_Enabled ? "ON" : "OFF"
                            );

                            ImGui::TextColored(
                                (selectedOption == 1 ? IM_COL_RED : IM_COL_WHITE),
                                "%s %s: %d ms",
                                (selectedOption == 1 ? ">" : " "),
                                "Delay",
                                c_TriggerBot_Delay
                            );

                            //TODO implement c_triggerbot_delay int

                            if (selectedOption == 0 && ImGui::IsKeyPressed(RIGHTKEY)) c_TriggerBot_Enabled = !c_TriggerBot_Enabled;
                            if (selectedOption == 1 && ImGui::IsKeyPressed(RIGHTKEY)) c_TriggerBot_Delay = min(c_TriggerBot_Delay + 10, 1000);
                            if (selectedOption == 1 && ImGui::IsKeyPressed(LEFTKEY))  c_TriggerBot_Delay = max(c_TriggerBot_Delay - 10, 0);

                            break;
                        case 3: //Misc

                            ImGui::TextColored(
                                (selectedOption == 0 ? IM_COL_RED : IM_COL_WHITE),
                                "%s %s: %s",
                                (selectedOption == 0 ? ">" : " "),
                                "INF HEALTH",
                                c_InfHealth ? "ON" : "OFF"
                            );

                            ImGui::TextColored(
                                (selectedOption == 1 ? IM_COL_RED : IM_COL_WHITE),
                                "%s %s: %s",
                                (selectedOption ==  1 ? ">" : " "),
                                "NO RECOIL",
                                c_NoRecoil ? "ON" : "OFF"
                            );


                            if (selectedOption == 0 && ImGui::IsKeyPressed(RIGHTKEY)) c_InfHealth = !c_InfHealth;
                            if (selectedOption == 1 && ImGui::IsKeyPressed(RIGHTKEY)) c_NoRecoil = !c_NoRecoil;

                            break;
                    }
                }

                ImGui::EndChild();
            }
            ImGui::End();

            // Handle navigation keys
            if (inOptions)
            {
                if (ImGui::IsKeyPressed(UPKEY))
                {
                    INT32 max = IM_ARRAYSIZE(espOpts);
                    if (selectedOption - 1 > -1)
                    {
                        selectedOption--;
                    }
                }
                if (ImGui::IsKeyPressed(DOWNKEY)) {
                    INT32 max = IM_ARRAYSIZE(espOpts);

                    selectedOption++;

                    if (selectedOption >= max)
                    {
                        selectedOption--;
                    }
                }
                if (ImGui::IsKeyPressed(VK_ESCAPE)) inOptions = false; // go back
            }
            else
            {
                if (ImGui::IsKeyPressed(UPKEY))   selectedTab--;
                if (ImGui::IsKeyPressed(DOWNKEY))
                {
                    selectedTab++;
                }
                if (ImGui::IsKeyPressed(RIGHTKEY)) { inOptions = true; selectedOption = 0; }
            }

            // clamp selection
            selectedTab = std::clamp(selectedTab, 0, IM_ARRAYSIZE(tabs) - 1);
        }

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    // WYWOŁANIE ORYGINAŁU:
    // - jeśli hook nadal aktywny -> użyj trampoliny (owglSwapBuffers)
    // - jeśli prolog przywrócony -> dzwoń bezpośrednio pod oryginalny adres (targetSwapBuffers)
    if (!g_Unhooked)
        return owglSwapBuffers(hdc);
    else
        return ((twglSwapBuffers)targetSwapBuffers)(hdc);
}


//BOOL __stdcall hkwglSwapBuffers(HDC hdc)
//{
//    if (init == FALSE)
//    {
//        glewExperimental = GL_TRUE;
//        if (glewInit() == GLEW_OK)
//        {
//            ImGui::CreateContext();
//            ImGuiIO& io = ImGui::GetIO();
//            io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
//            ImGui_ImplWin32_Init(Window);
//            ImGui_ImplOpenGL3_Init();
//            init = TRUE;
//        }
//    }
//
//    if (GetAsyncKeyState(VK_INSERT) & 1)
//        show = !show;
//
//    if (GetAsyncKeyState(VK_END) & 1) // Unload
//    {
//        MH_DisableHook(MH_ALL_HOOKS);
//        SetWindowLongPtr(Window, GWL_WNDPROC, (LONG_PTR)oWndProc); // Reset WndProc
//    }
//
//    ImGui_ImplOpenGL3_NewFrame();
//    ImGui_ImplWin32_NewFrame();
//    ImGui::NewFrame();
//
//    if (show)
//    {
//        ImGui::Begin("ImGui Window");
//        ImGui::End();
//    }
//
//    ImGui::EndFrame();
//    ImGui::Render();
//    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//    return owglSwapBuffers(hdc);
//}


LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI HackThread(HMODULE hModule)
{
    // ustaw niestandardowy WndProc i zapamiętaj oryginał
    oWndProc = (WNDPROC)SetWindowLongPtr(Window, GWL_WNDPROC, (LONG_PTR)WndProc);

    // resolve oryginalny wglSwapBuffers i backup prologu
    targetSwapBuffers = (BYTE*)GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
    memcpy(originalBytes, targetSwapBuffers, 5);

    // odpal trampolinę (zwraca wskaźnik na oryginał)
    owglSwapBuffers = (twglSwapBuffers)mem::TrampHook32(
        targetSwapBuffers,
        (BYTE*)hkwglSwapBuffers,
        5
    );

    return 0;
}

DWORD WINAPI Initalization(__in  LPVOID lpParameter)
{
    while (GetModuleHandle("opengl32.dll") == NULL) { Sleep(100); }
    Sleep(100);

    HMODULE hMod = GetModuleHandle("opengl32.dll");
    if (hMod)
    {
        do
            Window = GetProcessWindow();
        while (Window == NULL);

        RECT rect;
        if (GetClientRect(Window, &rect))
        {
            windowWidth = rect.right - rect.left;
            windowHeight = rect.bottom - rect.top;
        }

        InitModules();
        HackThread(ourModule); // SetWindowLongPtr wykonywany jest w HackThread
        return true;
    }
    else
        return false;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    ourModule = hModule;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, Initalization, 0, 0, 0);
        break; // PATCH: break here
    }
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

//
//BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
//    if (reason == DLL_PROCESS_ATTACH) {
//        MessageBoxA(0, "Injected!", "Test", MB_OK);
//    }
//    return TRUE;
//}