#pragma once
#include "includes.h"
#include "Player.h"
#include "LocalPlayer.h"
#include "EnemyList.h"
#include "Offsets.h"
/*
ÿ��ѭ��:
	�������е���,
	��׼,
	�ж��ܷ����
	��ǹ
*/
class Vector3;
class MyHack
{
public:
	MyHack();
	~MyHack();
	void Init(); // ��ʼ��
	void Update(); // ѭ���и��²���, ����
	void Exploit(); // ������

	bool CheckVaild(Player*);

	struct Var
	{
		bool isOpen = false;
	}var;

	struct Buttons {
		DWORD startBtn = VK_F2;
	}button;

	EnemyList enemies;
	uint32_t clientdll;
	LocalPlayer* plocal;
};

MyHack::MyHack()
{
	enemies.clear();
	clientdll = (uint32_t)(GetModuleHandle("client_panorama.dll"));
	plocal = LocalPlayer::Get();

}

MyHack::~MyHack()
{
}

inline void MyHack::Init()
{
	// ��ʼ����ȡ���е��˲���, д������, �ӿ�����ٶ�
	if (*(uintptr_t*)plocal) {
		uintptr_t localEnt = *(uintptr_t*)plocal;
		int localTeam = *plocal->GetTeam();

		// ��ʱ��ôд, ������Ҫ��̬����player����
		for (int i = 0; i < *Player::GetMaxPlayer(); i++)
		{
			Player* tmp = Player::GetPlayer(i);
			if (!*(uintptr_t*)tmp)continue;

			if (*tmp->GetTeam() != localTeam)
				enemies.add((DWORD)tmp);
		}
	}
}

inline void MyHack::Update()
{
	// ������ť�¼�
	if (GetAsyncKeyState(button.startBtn) & 1)
		var.isOpen = !var.isOpen;

	// ��������Ϊ0ʱ, ���µ���
	if (enemies.idx == 0) {
		Sleep(1000); //��һ�غ�?
		Init();
	}
}

inline void FixAngle(int myWeaponID,float currentDistance) {
	//Sleep(50); //clock18
	float delay;
	switch (myWeaponID & 0xff)
	{
	case 16: delay = 0.2; break; //m4a4
	case 11: delay = 0.15; break; // G3SG1, ����������, ������Ҫ����
	case 59:
	case 7: delay = 0.35; break; //ak
	case 40:delay = 0.2; break; //ssg, ����, ���������Ѵ�
	case 9: delay = 0.2; break; //awp, 0.15�����޸�����
	case 39: delay = 0.15; break; //sg, ������, ����0.1�е���
	case 13: delay = 0.4; break; //������
	case 10: delay = 0.1; break; //famas
	case 2: delay = 0.3; break; //˫ǹ
	case 8: delay = 0.15; break; //aug, ��������0.1
	case 14: delay = 0.3; break; //m249
	case 28: delay = 0.3; break; //�ڸ��
	case 34: delay = 0.3; break; //mp9
	case 61: delay = 0.25; break; //mp9
	case 38: delay = 0.15; break; //scar ��������0.1
	default: delay = 0.2;
	}
	printf("id:%d\tdelay:%f\tins:%f\n", myWeaponID, delay, currentDistance);
	Sleep(delay * currentDistance);
}

inline void MyHack::Exploit()
{
	if (!var.isOpen)return;

	DWORD i = 0;
	for (; i < enemies.idx; i++) {
		Player* enemy = (Player*)enemies.get(i);
		if (!enemy)break;
		if (*(uintptr_t*)enemy)
		{
			uintptr_t localPlayer = *(uintptr_t*)plocal;
			if (!localPlayer)return; //local player empty

			// ���Player�Ƿ���������
			if (!CheckVaild(enemy)) {
				// ���µ�����Ϣ
				enemies.del(i);
				continue;
			}
			// ������
			if (plocal->GetHealth() == 0) continue;

			int* pweapen = plocal->GetMyWeapen();
			if (!pweapen)continue;

AIM:
			plocal->AimAt(enemy->GetBonePos(8)); //��׼
			float currentDistance = plocal->GetDistance(enemy->GetOrigin());


			Sleep(10);
			int crosshairId = *(int*)plocal->GetCrosshairId();
			if (crosshairId <= 64 && crosshairId > 0) //����id
			{
				*(int*)(clientdll + hazedumper::signatures::dwForceAttack) = 5; //���
				Sleep(87); // ������ɿ�, �ɱ������Ծ���, ���������Ҫ΢��
				*(int*)(clientdll + hazedumper::signatures::dwForceAttack) = 4; //ֹͣ���
				Sleep(10); // �ȴ�����ֵˢ��

				// �ع�׼��
				FixAngle(*pweapen,currentDistance);
				//static int ti = 0;
				//ti++;
				//printf("time%d: %d, %d\n", ti, *enemy->GetHealth(),*plocal->GetShortFire());
				if (*enemy->GetHealth() > 0) {
					//printf("%d-time not kill\n", ti);
					goto  AIM;
				}
			}
		}
	}

}

inline bool MyHack::CheckVaild(Player* currentPlayer)
{
	LocalPlayer* localPlayer = plocal;
	if (!currentPlayer || !(*(uint32_t*)currentPlayer) || (uint32_t)currentPlayer == (uint32_t)localPlayer)
	{
		return false;
	}

	if (*currentPlayer->GetTeam() == *localPlayer->GetTeam())
	{
		return false;
	}

	if (*currentPlayer->GetHealth() < 1 || *localPlayer->GetHealth() < 1)
	{
		return false;
	}

	return true;
}
