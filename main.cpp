#include "includes.h"
#include <thread>
#include <iostream>


DWORD AmmoExplosiveType;
int selectedaim = 0;
float smooth = 2.f;





uintptr_t player_() {
	uintptr_t ClosestPlayer = 0;
	float closestdist = FLT_MAX;
	float dist;

	uintptr_t ReplayInterfacePatternFixed = read<uintptr_t>(replayptr + *(int*)(replayptr + 0x3) + 0x7);


	uintptr_t PedReplayInterface = read<uintptr_t>(ReplayInterfacePatternFixed + 0x18);

	if (PedReplayInterface) {
		int MaxPeds = read<int>(PedReplayInterface + 0x108);

		for (int i = 0; i < MaxPeds; i++) {
			uintptr_t PedList = read<uintptr_t>(PedReplayInterface + 0x100);
			if (!PedList) continue;
			uintptr_t pCPed = read<uintptr_t>(PedList + (i * 0x10));
			if (!pCPed) continue;

			if (pCPed == localplayer) continue;

			Vec3 EntPos = *(Vec3*)(pCPed + 0x90);


			float Health = read<float>(pCPed + 0x280);

			if (Health < 0 || Health == 0) continue;

			ImVec2 Ent2DPos = WorldToScreen(EntPos);
			if (selectedaim == 1)
			{

				auto ped_type = read<uint32_t>(pCPed + 0x10A8);
				ped_type = ped_type << 11 >> 25;


				if (ped_type != 2) {
					continue;
				}
			}
			if (selectedaim == 2)
			{
				auto ped_type = read<uint32_t>(pCPed + 0x10A8);
				ped_type = ped_type << 11 >> 25;


				if (ped_type == 2) {
					continue;
				}

			}
			ImVec2 Head = GetBonePosW2S(pCPed, SKEL_Head);
			ImVec2 middle = GetWindowMiddle();
			dist = pythag(middle, Head);

			if (dist < closestdist)
			{
				closestdist = dist;
				ClosestPlayer = pCPed;
			}
		}
	}

	return ClosestPlayer;
}

void _aimbot(uintptr_t entity) {

	auto get_distance = [](double x1, double y1, double x2, double y2) {
		return sqrtf(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0));
	};

	auto bone_pos = GetBonePosW2S(entity, SKEL_Head);
	Vec3 bone_postest = GetBonePos(entity, SKEL_Head);
		float aimspeedon = smooth + 1;
		ImVec2 screen = bone_pos;

		auto center_x = GetSystemMetrics(SM_CXSCREEN) / 2;
		auto center_y = GetSystemMetrics(SM_CYSCREEN) / 2;
		auto fov = get_distance(center_x, center_y, screen.x, screen.y);
		float TargetX = 0;
		float TargetY = 0;

		if (screen.x != 0) {
			if (screen.x > center_x) {
				TargetX = -(center_x - screen.x);
				TargetX /= aimspeedon;
				if (TargetX + center_x > center_x * 2) TargetX = 0;
			}

			if (screen.x < center_x) {
				TargetX = screen.x - center_x;
				TargetX /= aimspeedon;
				if (TargetX + center_x < 0) TargetX = 0;
			}
		}

		if (screen.y != 0) {
			if (screen.y > center_y) {
				TargetY = -(center_y - screen.y);
				TargetY /= aimspeedon;
				if (TargetY + center_y > center_y * 2) TargetY = 0;
			}

			if (screen.y < center_y) {
				TargetY = screen.y - center_y;
				TargetY /= aimspeedon;
				if (TargetY + center_y < 0) TargetY = 0;
			}
		}





		if (fov < fovvalu)
		{

			if (SAFE_CALL(GetAsyncKeyState)(VK_RBUTTON) & 0x8000) {

				mouse_event(MOUSEEVENTF_MOVE, float(TargetX), float(TargetY / 1.2), 0, 0);


			}
		}
	}


void DrawCornerBox(ImVec2 pos, ImVec2 dim, ImColor col, int thickness)
{
	if (OnScreen(pos)) {
		float cornersizex = dim.x / 3;
		float cornersizey = dim.y / 4;
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(pos), ImVec2(pos.x - cornersizex, pos.y), col, thickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(pos.x - dim.x, pos.y), ImVec2((pos.x - dim.x) + cornersizex, pos.y), col, thickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(pos.x - dim.x, pos.y), ImVec2(pos.x - dim.x, pos.y - cornersizey), col, thickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(pos.x - dim.x, (pos.y - dim.y)), ImVec2(pos.x - dim.x, (pos.y - dim.y) + cornersizey), col, thickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(pos.x - dim.x, (pos.y - dim.y)), ImVec2((pos.x - dim.x) + cornersizex, pos.y - dim.y), col, thickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(pos.x, (pos.y - dim.y)), ImVec2(pos.x - cornersizex, pos.y - dim.y), col, thickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(pos.x, (pos.y - dim.y)), ImVec2(pos.x, (pos.y - dim.y) + cornersizey), col, thickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(pos.x, pos.y), ImVec2(pos.x, pos.y - cornersizey), col, thickness);

	}
}

void aimbot_()
{
	if (localplayer) {

		_aimbot(player_());
	}
}

void esps() {
	if (localplayer) {
		uintptr_t ReplayInterfacePatternFixed = read<uint64_t>(replayptr + *(int*)(replayptr + 0x3) + 0x7);
		if (!ReplayInterfacePatternFixed) return;
		uintptr_t Pedrpin = read<uint64_t>(ReplayInterfacePatternFixed + 0x18);
		if (Pedrpin)
		{
			int MaxPeds = read<int>(Pedrpin + 0x108);

			for (int i = 0; i < MaxPeds; i++)
			{
				if (!Pedrpin) continue;
				uintptr_t PedList = read<uint64_t>(Pedrpin + 0x100);
				if (!PedList) continue;
				uintptr_t CPed = read<uint64_t>(PedList + (i * 0x10));

				if (!CPed) continue;
				if (ignoreselfEsp)
				{
					if (CPed == localplayer)
						continue;
				}

				if (PedEsp == false)
				{
					auto ped_type = read<uint32_t>(CPed + 0x10A8);
					ped_type = ped_type << 11 >> 25;

					if (ped_type != 2) {
						continue;
					}
				}


				Vec3 EntPos = read<Vec3>(CPed + 0x90);
				Vec3 LocPos = read<Vec3>(localplayer + 0x90);
				Vec3 DistanceCalculation = (LocPos - EntPos);
				double Distance = sqrtf(DistanceCalculation.x * DistanceCalculation.x + DistanceCalculation.y * DistanceCalculation.y + DistanceCalculation.z * DistanceCalculation.z);
				if (Distance > distancelimite)
				{
					continue;
				}
				float ArmorPed;
				ImVec2 Ped2DPos = WorldToScreen(EntPos);
				ImVec2 LocPos2D = WorldToScreen(LocPos);
				ImVec2 Head = GetBonePosW2S(CPed, SKEL_Head);
				ImVec2 LeftFoot = GetBonePosW2S(CPed, SKEL_L_Foot);
				ImVec2 RightFoot = GetBonePosW2S(CPed, SKEL_R_Foot);
				ImVec2 Leftarm = GetBonePosW2S(CPed, MH_L_Elbow);
				ImVec2 Rightarm = GetBonePosW2S(CPed, MH_R_Elbow);
				ImColor ESPSkeleton = ImColor(255, 255, 255);
				float FootAdv = (LeftFoot.y + RightFoot.y) / 2;
				float height = fabs(Head.y - (FootAdv)) * 1.2;
				float width = height / 2.5;
				float font_size = 18;
				if (box) {
					ImGui::GetBackgroundDrawList()->AddRect(ImVec2(Ped2DPos.x - width / 1.5, Ped2DPos.y - height / 1.5), ImVec2(Ped2DPos.x + width / 2, Ped2DPos.y + height / 1.6), ImColor(255, 255, 255, 255), 1, 1, 3);
					ImGui::GetBackgroundDrawList()->AddRect(ImVec2(Ped2DPos.x - width / 1.5, Ped2DPos.y - height / 1.5), ImVec2(Ped2DPos.x + width / 2, Ped2DPos.y + height / 1.6), ImColor(255, 255, 255, 255), 1, 1, 1);
				}
				if (iskelet) {
					ImVec2 Back = GetBonePosW2S(CPed, SKEL_Neck_1);

					ImVec2 LeftClavicle = GetBonePosW2S(CPed, SKEL_L_Clavicle);
					ImVec2 LeftElbow = GetBonePosW2S(CPed, MH_L_Elbow);
					ImVec2 LeftUpperArm = GetBonePosW2S(CPed, SKEL_L_UpperArm);
					ImVec2 LeftForearm = GetBonePosW2S(CPed, SKEL_L_Forearm);
					ImVec2 LeftHand = GetBonePosW2S(CPed, SKEL_L_Hand);
					ImVec2 RightClavicle = GetBonePosW2S(CPed, SKEL_R_Clavicle);
					ImVec2 RightElbow = GetBonePosW2S(CPed, MH_R_Elbow);
					ImVec2 RightUpperArm = GetBonePosW2S(CPed, SKEL_R_UpperArm);
					ImVec2 RightForearm = GetBonePosW2S(CPed, SKEL_R_Forearm);
					ImVec2 RightHand = GetBonePosW2S(CPed, SKEL_R_Hand);
					ImVec2 Pelvis = GetBonePosW2S(CPed, SKEL_Pelvis);
					ImVec2 LeftThigh = GetBonePosW2S(CPed, SKEL_L_Thigh);
					ImVec2 LeftCalf = GetBonePosW2S(CPed, SKEL_L_Calf);
					ImVec2 RightThigh = GetBonePosW2S(CPed, SKEL_R_Thigh);
					ImVec2 RightCalf = GetBonePosW2S(CPed, SKEL_R_Calf);

					ImGui::GetBackgroundDrawList()->AddLine(Head, Back, ESPSkeleton, 1);
					ImGui::GetBackgroundDrawList()->AddLine(Back, LeftUpperArm, ESPSkeleton, 1);
					ImGui::GetBackgroundDrawList()->AddLine(LeftUpperArm, LeftForearm, ESPSkeleton, 1);
					ImGui::GetBackgroundDrawList()->AddLine(LeftForearm, LeftHand, ESPSkeleton, 1);
					ImGui::GetBackgroundDrawList()->AddLine(Back, RightUpperArm, ESPSkeleton, 1);
					ImGui::GetBackgroundDrawList()->AddLine(RightUpperArm, RightForearm, ESPSkeleton, 1);
					ImGui::GetBackgroundDrawList()->AddLine(RightForearm, RightHand, ESPSkeleton, 1);
					ImGui::GetBackgroundDrawList()->AddLine(Back, Pelvis, ESPSkeleton, 1);
					ImGui::GetBackgroundDrawList()->AddLine(Pelvis, LeftThigh, ESPSkeleton, 1);
					ImGui::GetBackgroundDrawList()->AddLine(LeftThigh, LeftCalf, ESPSkeleton, 1);
					ImGui::GetBackgroundDrawList()->AddLine(LeftCalf, LeftFoot, ESPSkeleton, 1);
					ImGui::GetBackgroundDrawList()->AddLine(Pelvis, RightThigh, ESPSkeleton, 1);
					ImGui::GetBackgroundDrawList()->AddLine(RightThigh, RightCalf, ESPSkeleton, 1);
					ImGui::GetBackgroundDrawList()->AddLine(RightCalf, RightFoot, ESPSkeleton, 1);
				}
				float HealthPed = read<float>(CPed + 0x280);

				if (HealthPed == 0)
				{
					continue;
				}
				if (hpesp) {
					float HealthPercentage = height / 100 * (HealthPed - 100);
					float fixedhealth = HealthPed - 100;

					ImColor col;
					if (fixedhealth >= 75 && fixedhealth <= 100)
					{
						col = ImColor(81, 252, 141, 255);
					}
					else if (fixedhealth < 75 && fixedhealth > 50)
					{
						col = ImColor(191, 245, 64, 255);
					}
					else if (fixedhealth < 50 && fixedhealth > 25)
					{
						col = ImColor(221, 245, 64, 255);
					}
					else if (fixedhealth < 25 && fixedhealth > 0)
					{
						col = ImColor(245, 79, 64, 255);
					}
					DrawHealthBar(ImVec2(Ped2DPos.x + (width / 1.87), Ped2DPos.y + height / 2), ImVec2(4, HealthPercentage), col);
				}
			}
		}
	}
}

bool exploviveammo = false;
void exploit()
{
	if (localplayer) {


		if (godmodes)
		{
			int player = NativeInvoke::Invoke<0xD80958FC74E988A6, int>();
			NativeInvoke::Invoke<0x3882114BDE571AD4, bool>(player, NativeInvoke::Invoke<0xC1213A21, int>(player), true);
		}
		else
		{
			int player = NativeInvoke::Invoke<0xD80958FC74E988A6, int>();
			NativeInvoke::Invoke<0x3882114BDE571AD4, bool>(player, NativeInvoke::Invoke<0xC1213A21, int>(player), false);
		}
		if (ammo2)
		{
			int player = NativeInvoke::Invoke<0xD80958FC74E988A6, int>();
			NativeInvoke::Invoke<0x183DADC6AA953186, bool>(player, NativeInvoke::Invoke<0x5A5E3B67, int>(player), true);
		}
		else
		{
			int player = NativeInvoke::Invoke<0xD80958FC74E988A6, int>();
			NativeInvoke::Invoke<0x183DADC6AA953186, bool>(player, NativeInvoke::Invoke<0x5A5E3B67, int>(player), false);
		}
			if (superpunch) {
				*(float*)(*(DWORD64*)localplayer + 0x0D1C) = 1000.f;

			}
			else {
				*(float*)(*(DWORD64*)localplayer + 0x0D1C) = 0.f;
		}

			if (weaponmanager) {
				if (exploviveammo) {
					uintptr_t CWeaponInfoPtr = read<uintptr_t>(weaponmanager + 0x20);
					if (CWeaponInfoPtr) {
						*(int32_t*)(CWeaponInfoPtr + ammotype) = 0x5;
						*(int32_t*)(CWeaponInfoPtr + 0x24) = 0x1;
					}
				}
		}
		if (norecoil)
		{
			if (weaponmanager)
			{
				uintptr_t CWeaponInfoPtr = read<uintptr_t>(weaponmanager + 0x20);
				if (CWeaponInfoPtr)
				{
					*(float*)(CWeaponInfoPtr + recoil) = 0.f;

				}
			}


		}



		if (nospread)
		{

			if (weaponmanager)
			{
				uintptr_t CWeaponInfoPtr = read<uintptr_t>(weaponmanager + 0x20);
				if (CWeaponInfoPtr)
				{
					*(float*)(CWeaponInfoPtr + spread) = 0.f;

				}
			}
		}

	}
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();

			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}
	if (!localplayer)
	{
		localplayer = read<uintptr_t>(world + 0x8);
	}

	if (localplayer)
	{
		if (!weaponmanager)
		{
			if (SAFE_CALL(GetModuleHandleA)(_("FiveM_GameProcess.exe")) || SAFE_CALL(GetModuleHandleA)(_("FiveM_GTAProcess.exe")))
			{
				weaponmanager = read<uintptr_t>(localplayer + 0x10C8);
				playerinfo = 0x10B8;
				playerid = 0x68;
				recoil = 0x2E8;
				spread = 0x74;
				armor = 0x14B8;
				playerid = 0x68;
				ammotype = 0x20;
				weaponname = 0x5E0;
				AmmoExplosiveType = 0x24;
				range = 0x25C;

			}
			if (SAFE_CALL(GetModuleHandleA)(_("FiveM_b2060_GameProcess.exe")) || SAFE_CALL(GetModuleHandleA)(_("FiveM_b2060_GTAProcess.exe")))
			{
				weaponmanager = read<uintptr_t>(localplayer + 0x10D8);
				playerinfo = 0x10B8;
				playerid = 0x88;
				recoil = 0x2F4;
				spread = 0x84;
				armor = 0x14E0;
				playerid = 0x68;
				ammotype = 0x20;
				weaponname = 0x5F0;
				AmmoExplosiveType = 0x24;
				range = 0x28C;

			}
			if (SAFE_CALL(GetModuleHandleA)(_("FiveM_b2545_GameProcess.exe")) || SAFE_CALL(GetModuleHandleA)(_("FiveM_b2545_GTAProcess.exe")))
			{
				weaponmanager = read<uintptr_t>(localplayer + 0x10D8);
				playerinfo = 0x10C8;
				playerid = 0x88;
				recoil = 0x2F4;
				spread = 0x84;
				armor = 0x14E0 + 0x50;
				playerid = 0x68;
				ammotype = 0x20;
				AmmoExplosiveType = 0x24;
				weaponname = 0x5F0;
				range = 0x28C;

			}
			if (SAFE_CALL(GetModuleHandleA)(_("FiveM_b2612_GameProcess.exe")) || SAFE_CALL(GetModuleHandleA)(_("FiveM_b2612_GTAProcess.exe")) || SAFE_CALL(GetModuleHandleA)("FiveM_b2699_GTAProcess.exe") || SAFE_CALL(GetModuleHandleA)("FiveM_b2699_GameProcess.exe"))
			{
				weaponmanager = read<uintptr_t>(localplayer + 0x10D8);
				playerinfo = 0x10C8;
				playerid = 0x88;
				recoil = 0x2F4;
				spread = 0x84;
				armor = 0x1530;
				playerid = 0x68;
				ammotype = 0x20;
				weaponname = 0x5F0;
				AmmoExplosiveType = 0x24;
				range = 0x28C;

			}
			if (SAFE_CALL(GetModuleHandleA)(_("FiveM_b2189_GameProcess.exe")) || SAFE_CALL(GetModuleHandleA)(_("FiveM_b2189_GTAProcess.exe")))
			{
				weaponmanager = read<uintptr_t>(localplayer + 0x10D8);
				playerinfo = 0x10C8;
				playerid = 0x68;
				recoil = 0x2F4;
				spread = 0x84;
				armor = 0x14E0;
				playerid = 0x68;
				ammotype = 0x20;
				weaponname = 0x5F0;
				AmmoExplosiveType = 0x24;
				range = 0x28C;

			}
			if (SAFE_CALL(GetModuleHandleA)(_("FiveM_b2372_GameProcess.exe")) || SAFE_CALL(GetModuleHandleA)(_("FiveM_b2372_GTAProcess.exe")))
			{
				weaponmanager = read<uintptr_t>(localplayer + 0x10D8);
				playerinfo = 0x10C8;
				playerid = 0x68;
				recoil = 0x2F4;
				spread = 0x84;
				armor = 0x14E0;
				playerid = 0x68;
				ammotype = 0x20;
				weaponname = 0x5F0;
				AmmoExplosiveType = 0x24;
				range = 0x28C;

			}
		}
	}
	ColorChange();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	DrawWaterMark(_("refo#6223"), ImVec2(50, 100));
	if (aimfov) {
			ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2), fovvalu, IM_COL32(255, 255, 255, 255), 40.0f);
		}
	if (Visual) {
			esps();
	}
	if (aimbota) {
		aimbot_();
	}
		exploit();
	if (GetKeyState(VK_HOME)) {
		ImGui::Begin(_("refo"));
		ImGui::Checkbox(_("visual "), &Visual);
		ImGui::Checkbox(_("skeleton"), &iskelet);
		ImGui::Checkbox(_("healthbar"), &hpesp);
		ImGui::Checkbox(_("ignoreself"), &ignoreselfEsp);
		ImGui::Checkbox(_("norecoil "), &norecoil);
		ImGui::Checkbox(_("nospread "), &nospread);
		ImGui::Checkbox(_("infiniteammo "), &ammo2);
		ImGui::Checkbox(_("godmode "), &godmodes);
		ImGui::Checkbox(_("aimbot "), &aimbota);
		ImGui::Checkbox(_("aimfov "), &aimfov);
		ImGui::SliderFloat(_("fovvalue "), &fovvalu, 20.f, 200.f);
		ImGui::SliderFloat(_("smooth "), &smooth, 1.f, 10.f);
		ImGui::Checkbox(_("expammo"), &exploviveammo);
		ImGui::Checkbox(_("superpunch "), &superpunch);
		ImGui::End();
	}
	ImGui::Render();
	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}


DWORD WINAPI thread_main(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}



BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		show_console();
		DisableThreadLibraryCalls(hMod);
		CloseHandle(CreateThread(nullptr, 0, thread_main, hMod, 0, nullptr));
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}
