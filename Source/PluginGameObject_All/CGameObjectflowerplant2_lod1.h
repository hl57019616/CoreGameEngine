#ifndef __CGAMEOBJECTFLOWERPLANT2_LOD1_H__
#define __CGAMEOBJECTFLOWERPLANT2_LOD1_H__

#include "CGameObject.h"
#include "Plugins/IGameObjectFactory.h"

namespace Core
{
namespace Plugin
{
	class CGameObjectFlowerplant2_lod1 : public CGameObject
	{
	public:
		CGameObjectFlowerplant2_lod1(const char* Name, Vector3& Pos);
		~CGameObjectFlowerplant2_lod1();

		void Update(const f32& elapsedTime);

	};

	class CGameObjectFlowerplant2_lod1Factory : public IGameObjectFactory
	{
	public:
		CGameObjectFlowerplant2_lod1Factory();
		~CGameObjectFlowerplant2_lod1Factory();

		CGameObject* CreateObject(const char* Name, Vector3& Pos);
	};
}
}

#endif // __CGAMEOBJECTFLOWERPLANT2_LOD1_H__
