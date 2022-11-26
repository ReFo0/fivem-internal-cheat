#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <vector>
#include "imgui/imgui_internal.h"
#include <string>
#include "vec.h"
#include "bones.h"
#include "xor.h"
#include "safe.h"
#include <stdlib.h>

void show_console() {
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
}

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

bool aa = false;

uint64_t ScanSignature(uint64_t pModuleBaseAddress, const char* szSignature, size_t nSelectResultIndex = NULL) {
	auto PatternToBytes = [](const char* szpattern) {
		auto       m_iBytes = std::vector<int>{};
		const auto szStartAddr = const_cast<char*>(szpattern);
		const auto szEndAddr = const_cast<char*>(szpattern) + strlen(szpattern);

		for (auto szCurrentAddr = szStartAddr; szCurrentAddr < szEndAddr; ++szCurrentAddr) {
			if (*szCurrentAddr == '?') {
				++szCurrentAddr;
				if (*szCurrentAddr == '?') ++szCurrentAddr;
				m_iBytes.push_back(-1);
			}
			else m_iBytes.push_back(strtoul(szCurrentAddr, &szCurrentAddr, 16));
		}
		return m_iBytes;
	};

	const auto pDosHeader = (PIMAGE_DOS_HEADER)pModuleBaseAddress;
	const auto pNTHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)pModuleBaseAddress + pDosHeader->e_lfanew);
	const auto dwSizeOfImage = pNTHeaders->OptionalHeader.SizeOfImage;
	auto m_iPatternBytes = PatternToBytes(szSignature);
	const auto pScanBytes = reinterpret_cast<std::uint8_t*>(pModuleBaseAddress);
	const auto m_iPatternBytesSize = m_iPatternBytes.size();
	const auto m_iPatternBytesData = m_iPatternBytes.data();
	size_t nFoundResults = 0;

	for (auto i = 0ul; i < dwSizeOfImage - m_iPatternBytesSize; ++i) {
		bool bFound = true;

		for (auto j = 0ul; j < m_iPatternBytesSize; ++j) {
			if (pScanBytes[i + j] != m_iPatternBytesData[j] && m_iPatternBytesData[j] != -1) {
				bFound = false;
				break;
			}
		}

		if (bFound) {
			if (nSelectResultIndex != 0) {
				if (nFoundResults < nSelectResultIndex) {
					nFoundResults++;
					bFound = false;
				}
				else return reinterpret_cast<uint64_t>(&pScanBytes[i]);
			}
			else return reinterpret_cast<uint64_t>(&pScanBytes[i]);
		}
	}
	return NULL;
}


template <typename T> T read(uint64_t addr)
{
	T Novalue = {};
	if (!IsBadReadPtr((const void*)addr, sizeof(T)))
	{
		return *(T*)(addr);
	}
	else
	{
		return Novalue;
	}
}

template <typename T> void write(uint64_t addr, T buffer)
{
	T Novalue = {};
	if (!IsBadWritePtr((const void*)addr, sizeof(T)))
	{
		return *(T*)(addr) = buffer;
	}
	else
	{
		return Novalue;
	}
}


void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(_("C:\\Windows\\Fonts\\Tahoma.ttf"), 13.0f);
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;


float color_red = 1.;
float color_green = 0;
float color_blue = 0;
float color_random = 0.0;
float color_speed = -10.0;
uintptr_t localplayer;
uintptr_t weaponmanager;
DWORD recoil;
DWORD spread;
DWORD armor;
DWORD playerinfo;
DWORD playerid;
DWORD armortype;
DWORD ammotype;
DWORD weaponname;
DWORD range;
bool noclip = false;
bool norecoil = false;
bool nospread = false;
bool mouseaim = false;
bool aimfov = false;
float fovvalu = 20.f;
bool godmodes = false;
bool superpunch = false;
bool ammo2 = false;
bool noreload = false;
bool ignoreselfEsp = false;
bool infiniteammo = false;
bool aimbota = false;
float distancelimite = 1500;
bool box = false;
bool iskelet = false;
bool hpesp = false;
bool Visual = false;
bool PedEsp = true;

class scrNativeCallContext
{
protected:
	void* m_pReturn;
	uint32_t m_nArgCount;
	void* m_pArgs;

	uint32_t m_nDataCount;

public:
	template<typename T>
	inline T GetArgument(int idx)
	{
		intptr_t* arguments = (intptr_t*)m_pArgs;

		return *(T*)&arguments[idx];
	}

	inline void* GetArgumentBuffer()
	{
		return m_pArgs;
	}

	template<typename T>
	inline void SetResult(int idx, T value)
	{
		intptr_t* returnValues = (intptr_t*)m_pReturn;

		*(T*)&returnValues[idx] = value;
	}

	inline int GetArgumentCount()
	{
		return m_nArgCount;
	}

	template<typename T>
	inline T GetResult(int idx)
	{
		intptr_t* returnValues = (intptr_t*)m_pReturn;

		return *(T*)&returnValues[idx];
	}

	inline void SetVectorResults() { }
};

class NativeContext :
	public scrNativeCallContext
{
private:
	// Configuration
	enum
	{
		MaxNativeParams = 16,
		ArgSize = 4,
	};

	uint8_t m_TempStack[MaxNativeParams * ArgSize];

public:
	inline NativeContext()
	{
		m_pArgs = &m_TempStack;
		m_pReturn = &m_TempStack;
		m_nArgCount = 0;
		m_nDataCount = 0;
	}

	template <typename T>
	inline void Push(T value)
	{
		if (sizeof(T) > ArgSize)
		{
			throw "Argument has an invalid size";
		}
		else if (sizeof(T) < ArgSize)
		{
			*reinterpret_cast<uintptr_t*>(m_TempStack + ArgSize * m_nArgCount) = 0;
		}

		*reinterpret_cast<T*>(m_TempStack + ArgSize * m_nArgCount) = value;
		m_nArgCount++;
	}

	inline void Reverse()
	{
		uintptr_t tempValues[MaxNativeParams];
		uintptr_t* args = (uintptr_t*)m_pArgs;

		for (int i = 0; i < m_nArgCount; i++)
		{
			int target = m_nArgCount - i - 1;

			tempValues[target] = args[i];
		}

		memcpy(m_TempStack, tempValues, sizeof(m_TempStack));
	}

	template <typename T>
	inline T GetResult()
	{
		return *reinterpret_cast<T*>(m_TempStack);
	}
};

typedef void(__cdecl* NativeHandler)(scrNativeCallContext* context);

using GetNativeHandler_t = NativeHandler(__fastcall*)(__int64 hash);
using CallNativeHandler_t = void(__fastcall*)(NativeContext*);

struct pass
{
	template<typename ...T> pass(T...) {}
};

class NativeInvoke
{
private:

	static inline void Invoke(NativeContext* cxt, __int64 hash)
	{
		auto mod = SAFE_CALL(GetModuleHandleA)(_("rage-scripting-five.dll"));
		auto inset = SAFE_CALL(GetProcAddress)(mod, _("?GetNativeHandler@scrEngine@rage@@SAP6AXPEAVscrNativeCallContext@2@@Z_K@Z"));

		NativeHandler fn = ((GetNativeHandler_t)(inset))(hash);

		if (fn != 0)
		{
			(fn)(cxt);
		}
	}

public:

	template<__int64 Hash, typename R, typename... Args>
	static inline R Invoke(Args... args)
	{
		NativeContext cxt;

		pass{ ([&]()
		{
			cxt.Push(args);
		}(), 1)... };

		Invoke(&cxt, Hash);

		return cxt.GetResult<R>();
	}

};

uintptr_t gworldptr = ScanSignature((uintptr_t)(GetModuleHandle(nullptr)), _("48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52 8B 81"));
uintptr_t world = *(uint64_t*)(gworldptr + *(int*)(gworldptr + 0x3) + 0x7);
uintptr_t replayptr = ScanSignature((uintptr_t)(GetModuleHandle(nullptr)), _("48 8D 0D ?? ?? ?? ?? 89 44 24 30 E8 ?? ?? ?? ?? 48 83 C4 28 C3 48 8B 05"));
uintptr_t w2sptr = ScanSignature((uintptr_t)(GetModuleHandle(nullptr)), _("48 89 5C 24 ?? 55 56 57 48 83 EC 70 65 4C 8B 0C 25"));
uintptr_t boneptr = ScanSignature((uintptr_t)(GetModuleHandle(nullptr)), _("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC 60 48 8B 01 41 8B E8 48 8B F2 48 8B F9 33 DB"));


ImVec2 GetWindowMiddle()
{
	RECT rect;
	HWND hwnd = GetActiveWindow();
	if (GetWindowRect(hwnd, &rect))
	{
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		return ImVec2(width / 2, height / 2);
	}
	return ImVec2(0, 0);
}

ImVec2 WorldToScreen(Vec3 pos)
{
	auto& io = ImGui::GetIO();
	ImVec2 tempVec2;
	reinterpret_cast<bool(__fastcall*)(Vec3*, float*, float*)>(w2sptr)(&pos, &tempVec2.x, &tempVec2.y);
	tempVec2.x *= io.DisplaySize.x;
	tempVec2.y *= io.DisplaySize.y;
	return tempVec2;
}
ImVec2 GetBonePosW2S(const int64_t cPed, const int32_t wMask)
{
	__m128 tempVec4;
	reinterpret_cast<void* (__fastcall*)(int64_t, __m128*, int32_t)>(boneptr)(cPed, &tempVec4, wMask);
	return WorldToScreen(Vec3::FromM128(tempVec4));
}

Vec3 GetBonePos(const int64_t cPed, const int32_t wMask)
{
	__m128 tempVec4;
	reinterpret_cast<void* (__fastcall*)(int64_t, __m128*, int32_t)>(boneptr)(cPed, &tempVec4, wMask);
	return Vec3::FromM128(tempVec4);
}

float best_fov = 25.f;

float pythag(ImVec2 src, ImVec2 dst)
{
	return sqrtf(pow(src.x - dst.x, 2) + pow(src.y - dst.y, 2));
}

float pythagVec3(Vec3 src, Vec3 dst)
{
	return sqrtf(pow(src.x - dst.x, 2) + pow(src.y - dst.y, 2) + pow(src.z - dst.z, 2));
}


int roll(int min, int max)
{
	double x = rand() / static_cast<double>(RAND_MAX + 1);
	int that = min + static_cast<int>(x * (max - min));

	return that;
}

void ColorChange()
{
	static float Color[3];
	static DWORD Tickcount = 0;
	static DWORD Tickcheck = 0;
	ImGui::ColorConvertRGBtoHSV(color_red, color_green, color_blue, Color[0], Color[1], Color[2]);
	if (GetTickCount() - Tickcount >= 1)
	{
		if (Tickcheck != Tickcount)
		{
			Color[0] += 0.001f * color_speed;
			Tickcheck = Tickcount;
		}
		Tickcount = GetTickCount();
	}
	if (Color[0] < 0.0f) Color[0] += 1.0f;
	ImGui::ColorConvertHSVtoRGB(Color[0], Color[1], Color[2], color_red, color_green, color_blue);
}

typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;



static void __fastcall DrawCircleImGui(int x, int y, int radius, RGBA color, int segments)
{
	ImGui::GetOverlayDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), segments);
}

void DrawHealthBar(ImVec2 pos, ImVec2 dim, ImColor col)
{
	ImGui::GetBackgroundDrawList()->AddLine(pos, ImVec2(pos.x, pos.y - dim.y), col, dim.x);
}


void DrawWaterMark(std::string str, ImVec2 loc)
{
	ImVec2 size = { 0,0 };
	float minx = 0;
	float miny = 0;
	ImU32 RGB = ImGui::GetColorU32({ color_red, color_green, color_blue, 255 });

	ImDrawList* Renderer = ImGui::GetOverlayDrawList();
	Renderer->AddText(ImVec2((loc.x - 1) - minx, (loc.y - 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
	Renderer->AddText(ImVec2((loc.x + 1) - minx, (loc.y + 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
	Renderer->AddText(ImVec2((loc.x + 1) - minx, (loc.y - 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
	Renderer->AddText(ImVec2((loc.x - 1) - minx, (loc.y + 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
	Renderer->AddText(ImVec2(loc.x - minx, loc.y - miny), RGB, str.c_str());
}

 bool OnScreen(ImVec2 coords)
 {
	 if (coords.x < 0.1f || coords.y < 0.1 || coords.x > GetSystemMetrics(SM_CXSCREEN) || coords.y > GetSystemMetrics(SM_CYSCREEN))
	 {
		 return false;
	 }
	 else {
		 return true;
	 }
 }


 

