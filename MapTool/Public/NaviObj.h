#pragma once

#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CNaviObj final : public CGameObject
{
public:
	enum POINTTYPE
	{
		STRAIGHT,
		CORNER,
		POINT_END
	};


	typedef struct tNaviObj : public CGameObject::GAMEOBJECT_DESC
	{
		XMMATRIX		vStartPos;
		ROUTE_IO		tNaviDesc;
	}NAVIOBJ_DESC;

private:
	CNaviObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNaviObj(const CNaviObj& rhs);
	virtual ~CNaviObj() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	ROUTE_IO		Get_RouteIO(); 
	void			Set_NaviNum(int num) { m_eRoute.iCellNums = num; }
	void			Set_Color(int num) { m_iColor = num; }
	_int			Get_PointOpiton() { return m_eRoute.iPointOption; }
	void			Set_PointOption(_int iOption) { m_eRoute.iPointOption = iOption; }
	int				Get_VecNum() { return m_vRouteNums.size(); }
	vector<int>&		Get_RouteNumsVec() { return m_vRouteNums; }
	void			Add_RouteNum_InVec(int	iNum) { m_vRouteNums.push_back(iNum); }
	void			Delete_RouteNum_InVec(int	iNum);
	void			DeleteAll_RouteNum_InVec() { m_vRouteNums.clear(); }

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	ROUTE_IO			m_eRoute;

	int					m_iColor = { 1 };
	vector<int>								m_vRouteNums;

public:
	HRESULT							Add_Components(void* pArg);
	HRESULT							Bind_ShaderResources();

public:
	static CNaviObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};

END