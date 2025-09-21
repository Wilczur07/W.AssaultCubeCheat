#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <cstdint>
#include <initializer_list>
#include <vector>
#include <cstring>
#include <type_traits>

namespace mem {

    // Sprawdzenie, czy adres wskazuje na zmapowaną, czytalną stronę
    inline bool is_readable(const void* addr) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (!VirtualQuery(addr, &mbi, sizeof(mbi))) return false;
        if (mbi.State != MEM_COMMIT) return false;
        DWORD p = mbi.Protect & 0xFF;
        if (p == PAGE_NOACCESS || p == PAGE_GUARD) return false;
        return true;
    }

    // RAII do tymczasowej zmiany ochrony
    struct protect_guard {
        void* addr{};
        size_t size{};
        DWORD  oldProt{};
        bool   ok{};
        protect_guard(void* a, size_t s, DWORD newProt = PAGE_EXECUTE_READWRITE)
            : addr(a), size(s) {
            ok = VirtualProtect(addr, size, newProt, &oldProt) != 0;
        }
        ~protect_guard() {
            if (ok) {
                DWORD dummy;
                VirtualProtect(addr, size, oldProt, &dummy);
            }
        }
    };

    // Read/Write (lokalny proces)
    template<typename T>
    inline bool Read2(uintptr_t address, T& out) {
        if (!is_readable(reinterpret_cast<void*>(address))) return false;
        std::memcpy(&out, reinterpret_cast<const void*>(address), sizeof(T));
        return true;
    }

    template<typename T>
    inline T Read(uintptr_t address, T def = T{}) {
        T v{};
        return Read2<T>(address, v) ? v : def;
    }

    template<typename T>
    inline bool Write(uintptr_t address, const T& value) {
        protect_guard pg(reinterpret_cast<void*>(address), sizeof(T));
        if (!pg.ok) return false;
        std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(T));
        return true;
    }

    inline bool write_bytes(uintptr_t address, const void* src, size_t size) {
        protect_guard pg(reinterpret_cast<void*>(address), size);
        if (!pg.ok) return false;
        std::memcpy(reinterpret_cast<void*>(address), src, size);
        return true;
    }

    // Łańcuch wskaźników: base -> [base] + off1 -> [...] + off2 -> ...
    inline uintptr_t read_ptr_chain(uintptr_t base, std::initializer_list<uintptr_t> offsets) {
        uintptr_t ptr = base;
        for (auto off : offsets) {
            ptr = mem::Read<uintptr_t>(ptr);
            if (!ptr) return 0;
            ptr += off;
        }
        return ptr;
    }

    template<typename T>
    T ReadOffset(void* base, uintptr_t offset)
    {
        return *reinterpret_cast<T*>((uintptr_t)base + offset);
    }

    template<typename T>
    void WriteOffset(void* base, uintptr_t offset, const T& value)
    {
        *reinterpret_cast<T*>((uintptr_t)base + offset) = value;
    }

    inline HMODULE hmodule(const WCHAR* name = nullptr)
    {
        // nullptr => main module (the game exe)
        return GetModuleHandleW(name);
    }

    inline uintptr_t module_base(const WCHAR* name = nullptr) {
        return reinterpret_cast<uintptr_t>(hmodule(name));
    }
    inline size_t module_size(HMODULE mod) {
        MODULEINFO mi{};
        if (GetModuleInformation(GetCurrentProcess(), mod, &mi, sizeof(mi)))
            return mi.SizeOfImage;
        return 0;
    }

    // Prosty pattern scan "xx?x" w module
    inline uintptr_t find_pattern(const wchar_t* modName, const char* pat, const char* mask) {
        HMODULE mod = hmodule(modName);
        if (!mod) return 0;
        auto base = reinterpret_cast<uint8_t*>(mod);
        size_t size = module_size(mod);
        size_t mlen = std::strlen(mask);

        for (size_t i = 0; i + mlen <= size; ++i) {
            bool ok = true;
            for (size_t j = 0; j < mlen; ++j) {
                if (mask[j] == 'x' && pat[j] != *(char*)(base + i + j)) { ok = false; break; }
            }
            if (ok) return reinterpret_cast<uintptr_t>(base + i);
        }
        return 0;
    }


    //Hooking
    inline BOOL Detour32(BYTE* src, BYTE* dst, const UINT32 len)
    {
	    if (len < 5)
	    {
            return false;
	    }

        DWORD curProtection;
        VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

        uintptr_t relAddress = dst - src - 5;

        //JMP
        *src = 0xE9;

        //SET JMP TO REL ADRRESS
        *(uintptr_t*)(src + 1) = relAddress;

        VirtualProtect(src, len ,curProtection, &curProtection);
        return TRUE;
    }

    inline BYTE* TrampHook32(BYTE* src, BYTE* dst, const UINT32 len)
    {
        if (len < 5)
        {
            return 0;
        }

        //GATEWAY
        BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

        //WRITE STOLEN TO GATEWAY
        memcpy_s(gateway, len, src, len);

        uintptr_t gatewayRelAddress = src - gateway - 5;

        *(gateway + len) = 0xE9;
        //*(uintptr_t*)(gateway + len) = 0xE9;
        //*(uintptr_t*)(gateway + len + 1) = gatewayRelAddress;
        *(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelAddress;

        Detour32(src, dst, len);

        return gateway;
    }
}
