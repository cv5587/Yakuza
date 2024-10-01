#pragma once
#include "Loader.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

/*
플레이어에 관한 파일만 넣어주세요
*/

BEGIN(Client)
class CLoader_Anim :
	public CLoader
{
private:
	CLoader_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader_Anim() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevel) override;
	virtual HRESULT Loading() override;

protected:
	virtual HRESULT Loading_For_LogoLevel() override;
	virtual	HRESULT Loading_For_Office_1F() override;
	virtual	HRESULT Loading_For_Office_2F() override;
	virtual	HRESULT Loading_For_Office_Boss() override;
	virtual	HRESULT Loading_For_Dogimazo() override;
	virtual	HRESULT Loading_For_Dogimazo_Stairs() override;
	virtual	HRESULT Loading_For_Dogimazo_Lobby() override;
	virtual	HRESULT Loading_For_Dogimazo_Boss() override;
	virtual	HRESULT Loading_For_Street() override;
	virtual	HRESULT Loading_For_CarChase() override;
	virtual	HRESULT Loading_For_Karaoke() override;
	virtual HRESULT Loading_For_Tutorial() override;
	virtual HRESULT Loading_For_NishikiWalk() override;

private:
	virtual HRESULT Loading_Default() override;

public:
	static CLoader_Anim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};
END
