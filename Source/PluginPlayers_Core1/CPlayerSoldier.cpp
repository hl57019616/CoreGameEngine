#include "CPlayerSoldier.h"
#include "CPlayerPimpl.h"
#include "CGameManager.h"
#include "CCameraManager.h"
#include "CItemsManager.h"
#include "IPhysicsStrategy.h"
#include "CPlayerSoundComponent.h"
#include "AnimationMapping.h"
#include "Ai/CFpsVehicle.h"

#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "OgreAnimationState.h"

using namespace Core::Physics;
using namespace Core::Plugin;

namespace Core
{
namespace Plugin
{

CPlayerSoldier::CPlayerSoldier(const char* Name, IPhysicsStrategy* PhysicsStrategy) : CPlayer(Name, PhysicsStrategy)
{
	m_PlayerPimpl->m_MeshName = "Soldier.mesh";
	auto sceneManager = m_PlayerPimpl->m_GameManager->GetSceneManager();

	// Setup basic node structure to handle 3rd person cameras
	m_PlayerPimpl->m_PlayerNode = sceneManager->getRootSceneNode()->createChildSceneNode(Name);

	m_PlayerPimpl->m_PlayerEntity = sceneManager->createEntity(Name, m_PlayerPimpl->m_MeshName);
	m_PlayerPimpl->m_PlayerNode->attachObject(m_PlayerPimpl->m_PlayerEntity);

	m_PlayerPimpl->m_PlayerNode->setPosition(0, 0, 0);

	// ====== Set the player type ========
	m_PlayerPimpl->m_PlayerType = "SOLDIER";
	// ===================================

	// === Set the player bone names =====
	m_PlayerPimpl->m_RootBoneName = "";
	m_PlayerPimpl->m_HeadBoneName = "";
	// ===================================

	// Does the characater have a controller
	m_PlayerPimpl->isCharacterController = true;
	// ===================================

	m_PlayerPimpl->PostInit(this);
}

CPlayerSoldier::~CPlayerSoldier()
{
	m_PlayerPimpl->m_GameManager->GetSceneManager()->destroySceneNode(m_PlayerPimpl->Name);
}

void CPlayerSoldier::SetupMapping()
{
}

void CPlayerSoldier::Update(const f32& elapsedTime)
{
	UpdateDefault(elapsedTime);
}

void CPlayerSoldier::OnShotFired()
{
	CPlayer::OnShotFired();
}
// CPlayerSoldierFactory
CPlayerSoldierFactory::CPlayerSoldierFactory()
{
	PlayerType = "SOLDIER";
}

CPlayerSoldierFactory::~CPlayerSoldierFactory()
{
}

Core::CPlayer* CPlayerSoldierFactory::CreatePlayer(const char* Name, Physics::IPhysicsStrategy* PhysicsStrategy)
{
	return new CPlayerSoldier(Name, PhysicsStrategy);
}

}
}
