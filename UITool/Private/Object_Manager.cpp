#include "Object_Manager.h"

#include "GameInstance.h"

#include "Image_Texture.h"
#include "Text.h"
#include "Group.h"
#include "Btn.h"
#include "UI_Effect.h"	

#include "UI_Object.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager(): m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

vector<class CUI_Object*> CObject_Manager::Get_GroupObject(const wstring& strTag)
{
	vector<class CUI_Object*>* pFineObjects = Find_RenderTextureObject(strTag);

	if (nullptr == pFineObjects)
		return vector<class CUI_Object*>();

	return *pFineObjects;
}

vector<class CUI_Object*> CObject_Manager::Get_BinaryGroupObject(const wstring& strTag, _int iIndex)
{
	vector<class CUI_Object*>* pFineObjects = Find_BinaryObject(strTag);

	if (nullptr == pFineObjects)
		return vector<class CUI_Object*>();

	if ((*pFineObjects)[iIndex]->Get_TypeIndex() != GROUP)
		return vector<class CUI_Object*>();

	return dynamic_cast<CGroup*>((*pFineObjects)[iIndex])->Get_PartObjects();
}

vector<class CUI_Object*> CObject_Manager::Get_GroupBinaryObject(const wstring& strTag)
{
	vector<class CUI_Object*>* pFineObjects = Find_BinaryObject(strTag);

	if (nullptr == pFineObjects)
		return vector<class CUI_Object*>();

	return *pFineObjects;
}

HRESULT CObject_Manager::Copy_Group(const wstring& strTag)
{
	static _uint iCopyIndex = 0;

	vector<class CUI_Object*>* pFineObjects = Find_RenderTextureObject(strTag);

	if (nullptr == pFineObjects)
		return E_FAIL;

	_tchar strCopyTag[MAX_PATH];
	wstring strOrigTag = strTag + L"%d";
	wsprintfW(strCopyTag, strOrigTag.c_str(), iCopyIndex);

	vector<class CUI_Object*> CopyFineObjects;

	for (auto pTexture : *pFineObjects)
	{
		if (pTexture->Get_TypeIndex() == IMG)
		{
			CUI_Texture* pUI = dynamic_cast<CUI_Texture*>(pTexture);

			CUI_Texture::UI_TEXTURE_DESC Desc{};
			Desc.strTextureFilePath = pUI->Get_FilePath();
			Desc.strTextureFileName = pUI->Get_FileName();
			Desc.iTypeIndex = pUI->Get_TypeIndex();
			Desc.vUpPoint = pUI->Get_UpPoint();
			Desc.vDownPoint = pUI->Get_DownPoint();
			Desc.isScreen = pUI->Get_isScreen();
			Desc.vEndColor = pUI->Get_EndColor();


			CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &Desc));
			if (nullptr == pImage)
				return E_FAIL;

			pImage->Set_Color(pUI->Get_Color());	
			pImage->Set_SizeX(pUI->Get_SizeX());
			pImage->Set_SizeY(pUI->Get_SizeY());
			pImage->Set_ShaderPass(pUI->Get_ShaderPass());
			pImage->Change_UV(pUI->Get_StartUV(), pUI->Get_EndUV());
			pImage->Get_TransformCom()->Set_WorldMatrix(pUI->Get_TransformCom()->Get_WorldMatrix());			

			CopyFineObjects.emplace_back(pImage);
		}
		else if (pTexture->Get_TypeIndex() == TEXT)
		{
			CText* pText = dynamic_cast<CText*>(pTexture);

			CText::TEXT_DESC Desc{};
			Desc.strText = pText->Get_Text();
			Desc.vColor = pText->Get_Color();
			Desc.iTypeIndex = pText->Get_TypeIndex();


			CText* pObjText = dynamic_cast<CText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Text"), &Desc));
			if (nullptr == pObjText)
				return E_FAIL;

			CopyFineObjects.emplace_back(pObjText);
		}

	}

	m_Objects.emplace(strCopyTag, CopyFineObjects);
	if (FAILED(Create_Texture2D()))
		return E_FAIL;
	
	//Binary용 
	vector<class CUI_Object*> BinaryObjects;
	m_BinaryObjects.emplace(strCopyTag, BinaryObjects);
	if (FAILED(Create_BinaryTexture2D()))
		return E_FAIL;

	iCopyIndex++; //인덱스 늘리기

	return S_OK;
}

HRESULT CObject_Manager::Remove_Group(const wstring& strTag)
{
	vector<class CUI_Object*>* pFineObjects = Find_RenderTextureObject(strTag);
	if (nullptr == pFineObjects)
		return E_FAIL;

	for (auto& pObject : *pFineObjects)
		Safe_Release(pObject);

	m_Objects.erase(strTag);

	vector<class CUI_Object*>* pFineBinaryObjects = Find_BinaryObject(strTag);
	if (nullptr == pFineBinaryObjects)
		return E_FAIL;

	for (auto& pObject : *pFineBinaryObjects)
		Safe_Release(pObject);

	m_BinaryObjects.erase(strTag);

	return S_OK;
}

HRESULT CObject_Manager::Remove_GroupObject(const wstring& strTag, const _uint ibinaryIndex,const _uint iIndex)
{
	vector<class CUI_Object*>* pFineBinaryObjects = Find_BinaryObject(strTag);
	if (nullptr == pFineBinaryObjects)
		return E_FAIL;

	dynamic_cast<CGroup*>((*pFineBinaryObjects)[ibinaryIndex])->Remove_PartObject(iIndex);

	return S_OK;
}

HRESULT CObject_Manager::Remove_Object(const wstring& strTag, _uint iIndex)
{
	vector<class CUI_Object*>* pFineObjects = Find_RenderTextureObject(strTag);

	if (nullptr == pFineObjects)
		return E_FAIL;

	Safe_Release((*pFineObjects)[iIndex]);
	pFineObjects->erase((*pFineObjects).begin() + iIndex);

	return S_OK;
}

HRESULT CObject_Manager::Remove_BinaryObject(const wstring& strTag, _uint iIndex)
{
	vector<class CUI_Object*>* pFineObjects = Find_BinaryObject(strTag);

	if (nullptr == pFineObjects)
		return E_FAIL;

	Safe_Release((*pFineObjects)[iIndex]);
	pFineObjects->erase((*pFineObjects).begin() + iIndex);

	return S_OK;
}

HRESULT CObject_Manager::Create_Texture(_uint iIndex, const wstring& strFilePath)
{
	return m_Texture2Ds[iIndex]->Create_Texture(strFilePath);
}

HRESULT CObject_Manager::Move_ObjectIndex(const wstring& strTag, _uint iIndex, _uint iMoveType)
{
	vector<class CUI_Object*>* pFineObjects = Find_RenderTextureObject(strTag);

	if (nullptr == pFineObjects)
		return E_FAIL;

	//예외처리
	if ((iMoveType == DOWN && iIndex <= 0) || (iMoveType == UP && iIndex >= (*pFineObjects).size() - 1) || (iMoveType == MOVE_END))
		return E_FAIL;

	//스왑
	auto iter1 = (*pFineObjects).begin() + iIndex;
	auto iter2 = iMoveType == DOWN ? (*pFineObjects).begin() + iIndex - 1 : (*pFineObjects).begin() + iIndex + 1;

	iter_swap(iter1, iter2);

	return S_OK;
}

HRESULT CObject_Manager::Move_BinaryObjectIndex(const wstring& strTag, _uint iIndex, _uint iMoveType)
{
	vector<class CUI_Object*>* pFineObjects = Find_BinaryObject(strTag);

	if (nullptr == pFineObjects)
		return E_FAIL;

	//예외처리
	if ((iMoveType == DOWN && iIndex <= 0) || (iMoveType == UP && iIndex >= (*pFineObjects).size() - 1) || (iMoveType == MOVE_END))
		return E_FAIL;

	//스왑
	auto iter1 = (*pFineObjects).begin() + iIndex;
	auto iter2 = iMoveType == DOWN ? (*pFineObjects).begin() + iIndex - 1 : (*pFineObjects).begin() + iIndex + 1;	

	iter_swap(iter1, iter2);	

	return S_OK;	
}

HRESULT CObject_Manager::Move_BinaryGroupObjectIndex(const wstring& strTag, const _uint ibinaryIndex, _uint iIndex, _uint iMoveType)
{
	vector<class CUI_Object*>* pFineObjects = Find_BinaryObject(strTag);
	if (nullptr == pFineObjects)
		return E_FAIL;

	vector<class CUI_Object*>* pFineGroupObjects = dynamic_cast<CGroup*>((*pFineObjects)[ibinaryIndex])->Get_pPartObjects();

	//예외처리
	if ((iMoveType == DOWN && iIndex <= 0) || (iMoveType == UP && iIndex >= (*pFineGroupObjects).size() - 1) || (iMoveType == MOVE_END))
		return E_FAIL;

	//스왑
	auto iter1 = (*pFineGroupObjects).begin() + iIndex;
	auto iter2 = iMoveType == DOWN ? (*pFineGroupObjects).begin() + iIndex - 1 : (*pFineGroupObjects).begin() + iIndex + 1;

	iter_swap(iter1, iter2);

	return S_OK;
}

HRESULT CObject_Manager::Save_binary()
{
	string strDirectory = "../../Client/Bin/DataFiles/UIData/";

	fs::path path(strDirectory);	
	if (!exists(path))	
		fs::create_directory(path);	

	for (auto& Pair : m_BinaryObjects)
	{
		for (auto& pObj : Pair.second)
		{
			switch (pObj->Get_TypeIndex())
			{
			case UITool::CObject_Manager::IMG:
			case UITool::CObject_Manager::HEADUI:
			case UITool::CObject_Manager::BTN:	
			case UITool::CObject_Manager::TEXT:
			case UITool::CObject_Manager::EFFECT:
				pObj->Save_binary(strDirectory);
				break;
			case UITool::CObject_Manager::GROUP:
			{
				string Directory = strDirectory;
				string Name = pObj->Get_Name();
				_uint Type = pObj->Get_TypeIndex();

				Directory = Directory + Name + ".dat";

				ofstream out(Directory, ios::binary);

				out.write((char*)&Type, sizeof(_uint));

				_int strlength = Name.length();
				out.write((char*)&strlength, sizeof(_int));
				out.write(Name.c_str(), strlength);


				dynamic_cast<CGroup*>(pObj)->Save_Groupbinary(out);

				out.close();
			}
				break;
			default:
				break;
			}
			
		}
	}

	return S_OK;
}
//지금 타겟에 바이너리파일을 생성시켜줌
HRESULT CObject_Manager::Load_binary(const wstring& strObjectTag, const string FilePath)
{
	auto pObjects = Find_BinaryObject(strObjectTag);

	ifstream in(FilePath, ios::binary);	

	if (!in.is_open()) {
		MSG_BOX("개방 실패");
		return E_FAIL;
	}
	_uint Type;
	
	in.read((char*)&Type, sizeof(_uint));

	switch (Type)	
	{
	case IMG:
	case HEADUI:
	{
		CImage_Texture::tUITextureDesc pDesc{};
		pDesc.iTypeIndex = Type;

		_int strTexturelength;
		char charBox[MAX_PATH] = {};
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);	
		pDesc.strName = charBox;

		in.read((char*)&pDesc.isParent, sizeof(_bool));

		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		pDesc.strParentName = charBox;

		string path;
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*) &charBox, strTexturelength);
		path = charBox;
		pDesc.strTextureFilePath = m_pGameInstance->StringToWstring(path);

		ZeroMemory(charBox, MAX_PATH);
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		path = charBox;
		pDesc.strTextureFileName = m_pGameInstance->StringToWstring(path);

		in.read((char*)&pDesc.fStartUV, sizeof(_float2));
		in.read((char*)&pDesc.fEndUV, sizeof(_float2));
		in.read((char*)&pDesc.vColor, sizeof(_float4));
		in.read((char*)&pDesc.isColor, sizeof(_bool));
		in.read((char*)&pDesc.iShaderPass, sizeof(_uint));
		in.read((char*)&pDesc.WorldMatrix, sizeof(_float4x4));

		in.read((char*)&pDesc.bAnim, sizeof(_bool));
		in.read((char*)&pDesc.fAnimTime, sizeof(_float2));
		in.read((char*)&pDesc.vStartPos, sizeof(_float3));
		
		in.read((char*)&pDesc.fControlAlpha, sizeof(_float2));
		in.read((char*)&pDesc.isReverse, sizeof(_bool));

		in.read((char*)&pDesc.isEvent, sizeof(_bool));
		in.read((char*)&pDesc.isScreen, sizeof(_bool));
		in.read((char*)&pDesc.isAnimLoop, sizeof(_bool));

		in.read((char*)&pDesc.vUpPoint, sizeof(_float4));
		if (isnan(pDesc.vUpPoint.x))
			pDesc.vUpPoint = _float4(0.f, 0.f, 0.f, 0.f);

		in.read((char*)&pDesc.vDownPoint, sizeof(_float4));
		if (isnan(pDesc.vDownPoint.x))
			pDesc.vDownPoint = _float4(0.f, 0.f, 0.f, 0.f);


		in.read((char*)&pDesc.vEndColor, sizeof(_float4));
		if (isnan(pDesc.vEndColor.x) || isnan(pDesc.vEndColor.y) || isnan(pDesc.vEndColor.z) || isnan(pDesc.vEndColor.w))
			pDesc.vEndColor = _float4(1.f, 1.f, 1.f, 1.f);

		in.read((char*)&pDesc.vAnimScale, sizeof(_float2));
		if (0==(pDesc.vAnimScale.x) && 0==(pDesc.vAnimScale.y))
			pDesc.vAnimScale = _float2(1.f, 1.f);

		pDesc.isLoad = true;
		in.close();

		CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &pDesc));
		if (nullptr == pImage)
			return E_FAIL;

		pObjects->emplace_back(pImage);
	}
		break;

	case BTN:
	{
		CBtn::BTN_DESC pDesc{};	
		pDesc.iTypeIndex = Type;	

		_int strTexturelength;
		char charBox[MAX_PATH] = {};
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		pDesc.strName = charBox;

		in.read((char*)&pDesc.isParent, sizeof(_bool));

		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		pDesc.strParentName = charBox;

		string path;
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		path = charBox;
		pDesc.strTextureFilePath = m_pGameInstance->StringToWstring(path);

		ZeroMemory(charBox, MAX_PATH);
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		path = charBox;
		pDesc.strTextureFileName = m_pGameInstance->StringToWstring(path);

		in.read((char*)&pDesc.fStartUV, sizeof(_float2));
		in.read((char*)&pDesc.fEndUV, sizeof(_float2));
		in.read((char*)&pDesc.vColor, sizeof(_float4));
		in.read((char*)&pDesc.isColor, sizeof(_bool));
		in.read((char*)&pDesc.iShaderPass, sizeof(_uint));
		in.read((char*)&pDesc.WorldMatrix, sizeof(_float4x4));

		in.read((char*)&pDesc.bAnim, sizeof(_bool));
		in.read((char*)&pDesc.fAnimTime, sizeof(_float2));
		in.read((char*)&pDesc.vStartPos, sizeof(_float3));

		in.read((char*)&pDesc.fControlAlpha, sizeof(_float2));
		in.read((char*)&pDesc.isReverse, sizeof(_bool));

		in.read((char*)&pDesc.isEvent, sizeof(_bool));
		in.read((char*)&pDesc.isScreen, sizeof(_bool));


		in.read((char*)&pDesc.vEndColor, sizeof(_float4));
		in.read((char*)&pDesc.vAnimScale, sizeof(_float2));

		if (pDesc.bAnim)
		{
		//	pDesc.vEndColor = pDesc.vColor;
			//pDesc.vAnimScale = _float2(1.f, 1.f);
		}


		//개별
		ZeroMemory(charBox, MAX_PATH);	
		in.read((char*)&strTexturelength, sizeof(_int));	
		in.read((char*)&charBox, strTexturelength);	
		path = charBox;	
		pDesc.strClickFilePath = m_pGameInstance->StringToWstring(path);

		ZeroMemory(charBox, MAX_PATH);
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		path = charBox;
		pDesc.strClickFileName = m_pGameInstance->StringToWstring(path);

		in.read((char*)&pDesc.ClickStartUV, sizeof(_float2));
		in.read((char*)&pDesc.ClickEndUV, sizeof(_float2));
		in.read((char*)&pDesc.vClickColor, sizeof(_float4));

		pDesc.isLoad = true;



		in.close();

		CBtn* pBtn = dynamic_cast<CBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Btn"), &pDesc));
		if (nullptr == pBtn)
			return E_FAIL;

		pObjects->emplace_back(pBtn);
	}
		break;

	case TEXT:
	{
		CText::TEXT_DESC pDesc{};
		pDesc.iTypeIndex = Type;

		_int strTexturelength;
		char charBox[MAX_PATH] = {};
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		pDesc.strName = charBox;

		in.read((char*)&pDesc.isParent, sizeof(_bool));

		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		pDesc.strParentName = charBox;

		string path;

		in.read((char*)&pDesc.vColor, sizeof(_float4));
		in.read((char*)&pDesc.isColor, sizeof(_bool));
		in.read((char*)&pDesc.iShaderPass, sizeof(_uint));
		in.read((char*)&pDesc.WorldMatrix, sizeof(_float4x4));

		in.read((char*)&pDesc.bAnim, sizeof(_bool));
		in.read((char*)&pDesc.fAnimTime, sizeof(_float2));
		in.read((char*)&pDesc.vStartPos, sizeof(_float3));

		in.read((char*)&pDesc.fControlAlpha, sizeof(_float2));
		in.read((char*)&pDesc.isReverse, sizeof(_bool));

		in.read((char*)&pDesc.isEvent, sizeof(_bool));
		in.read((char*)&pDesc.isScreen, sizeof(_bool));
		in.read((char*)&pDesc.vEndColor, sizeof(_float4));
		//개별
		ZeroMemory(charBox, MAX_PATH);
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		path = charBox;
		pDesc.strText = m_pGameInstance->StringToWstring(path);

		in.read((char*)&pDesc.iAlign, sizeof(_uint));

		ZeroMemory(charBox, MAX_PATH);
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		path = charBox;
		pDesc.Font = m_pGameInstance->StringToWstring(path);


		pDesc.isLoad = true;
		in.close();

		CText* pText = dynamic_cast<CText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Text"), &pDesc));
		if (nullptr == pText)
			return E_FAIL;

		pObjects->emplace_back(pText);
	}
		break;

	case GROUP:
	{
		CGroup::UI_OBJECT_DESC pDesc{};
		pDesc.iTypeIndex = Type;
		
		_int strTexturelength;
		char charBox[MAX_PATH] = {};
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		pDesc.strName = charBox;

		CGroup* pGroup = dynamic_cast<CGroup*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Group"), &pDesc));
		if (nullptr == pGroup)
			return E_FAIL;

		pGroup->Load_Groupbinary(in);

		in.close();

		pObjects->emplace_back(pGroup);
	}
		break;

	case EFFECT:
	{
		CUI_Effect::UI_EFFECT_DESC pDesc{};
		pDesc.iTypeIndex = Type;

		_int strTexturelength;
		char charBox[MAX_PATH] = {};
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		pDesc.strName = charBox;

		in.read((char*)&pDesc.isParent, sizeof(_bool));

		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		pDesc.strParentName = charBox;

		string path;
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		path = charBox;
		pDesc.strTextureFilePath = m_pGameInstance->StringToWstring(path);

		ZeroMemory(charBox, MAX_PATH);
		in.read((char*)&strTexturelength, sizeof(_int));
		in.read((char*)&charBox, strTexturelength);
		path = charBox;
		pDesc.strTextureFileName = m_pGameInstance->StringToWstring(path);

		in.read((char*)&pDesc.fStartUV, sizeof(_float2));
		in.read((char*)&pDesc.fEndUV, sizeof(_float2));
		in.read((char*)&pDesc.vColor, sizeof(_float4));
		in.read((char*)&pDesc.isColor, sizeof(_bool));
		in.read((char*)&pDesc.iShaderPass, sizeof(_uint));
		in.read((char*)&pDesc.WorldMatrix, sizeof(_float4x4));

		in.read((char*)&pDesc.bAnim, sizeof(_bool));
		in.read((char*)&pDesc.fAnimTime, sizeof(_float2));
		in.read((char*)&pDesc.vStartPos, sizeof(_float3));

		in.read((char*)&pDesc.fControlAlpha, sizeof(_float2));
		in.read((char*)&pDesc.isReverse, sizeof(_bool));

		in.read((char*)&pDesc.isEvent, sizeof(_bool));
		in.read((char*)&pDesc.isScreen, sizeof(_bool));

		//in.read((char*)&pDesc.vAnimScale, sizeof(_float2));
		pDesc.vAnimScale = _float2(1.f, 1.f);
		//개별

		in.read((char*)&pDesc.vLifeTime, sizeof(_float3));	
		in.read((char*)&pDesc.fSpeed, sizeof(_float));
		in.read((char*)&pDesc.isUVAnim, sizeof(_bool));

		pDesc.isLoad = true;
		in.close();

		CUI_Effect* pUIEffect = dynamic_cast<CUI_Effect*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIEffect"), &pDesc));
		if (nullptr == pUIEffect)
			return E_FAIL;

		pObjects->emplace_back(pUIEffect);
	}
		break;

	default:
		break;
	}

	in.close();

	return S_OK;
}

HRESULT CObject_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)	
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	m_pVIBufferCom = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxUI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	D3D11_VIEWPORT ViewPort{};
	_uint iNumViewPort = 1;

	m_pContext->RSGetViewports(&iNumViewPort, &ViewPort);

	/* 화면을 꽉 채워주기 위한 월드변환행렬. */
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewPort.Width, ViewPort.Height, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPort.Width, ViewPort.Height, 0.f, 1.f));

	//백버퍼 카피
	CRenderTarget* pCopyBackBuffer = CRenderTarget::Create(m_pDevice, m_pContext, 1280.f, 720.f, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f),false);
	if (nullptr == pCopyBackBuffer)
		return E_FAIL;
#ifdef _DEBUG

	pCopyBackBuffer->Ready_Debug(g_iWinSizeX - 100.f, 100.f, 200.f, 200.f);
	m_CopyBackTexture2Ds.emplace_back(pCopyBackBuffer);
#endif // _DEBUG


	return S_OK;
}

void CObject_Manager::Tick(const _float& fTimeDelta)
{
	for (auto& Pair : m_Objects)
	{
		for (auto& pObj : Pair.second)
			pObj->Tick(fTimeDelta);
	}

	for (auto& Pair : m_BinaryObjects)
	{
		for (auto& pObj : Pair.second)
			pObj->Tick(fTimeDelta);
	}
}

void CObject_Manager::Late_Tick(const _float& fTimeDelta)
{
	for (auto& Pair : m_Objects)
	{
		for (auto& pObj : Pair.second)
			pObj->Late_Tick(fTimeDelta);
	}

	for (auto& Pair : m_BinaryObjects)
	{
		for (auto& pObj : Pair.second)
			pObj->Late_Tick(fTimeDelta);
	}
}

HRESULT CObject_Manager::Render()
{
	if (FAILED(Render_Object()))
		return E_FAIL;

	if (FAILED(Render_Copy()))
		return E_FAIL;

	if (FAILED(Render_BackBuffer()))
		return E_FAIL;

	////디버깅용 렌더 타겟 뷰
	//for (auto& pTexture : m_Texture2Ds)
	//	pTexture->Render_Debug(m_pShaderCom, m_pVIBufferCom);
	//	
	//for (auto& pTexture : m_CopyBackTexture2Ds)
	//	pTexture->Render_Debug(m_pShaderCom, m_pVIBufferCom);

	return S_OK;
}

HRESULT CObject_Manager::Render_Object()
{
	_uint iIndex = 0;
	auto BinaryIter = m_BinaryObjects.begin();

	for (auto& Pair : m_Objects)
	{
		Texture2D_Begin(m_Texture2Ds, iIndex);

		//렌더타겟 먼저 그려줌
		for (auto& pObj : Pair.second)
			pObj->Render();

		for (auto& pObj : BinaryIter->second)
			pObj->Render();

		Texture2D_End();

		++BinaryIter;
		++iIndex;
	}

	return S_OK;




	//_uint iIndex = 0;
	//auto BinaryIter = m_BinaryObjects.begin();

	//for (auto& Pair : m_Objects)
	//{
	//	//Texture2D_Begin(m_Texture2Ds, iIndex);

	//	////렌더타겟 먼저 그려줌
	//	//for (auto& pObj : Pair.second)
	//	//	pObj->Render();

	//	//Texture2D_End();

	//	Texture2D_Begin(m_BinaryTexture2Ds, iIndex);

	//	//바이너리화 할 오브젝트들 그려줌
	//	for (auto& pObj : BinaryIter->second)
	//		pObj->Render();

	//	Texture2D_End();

	//	++BinaryIter;
	//	++iIndex;
	//}
	//	return S_OK;

}

HRESULT CObject_Manager::Render_Copy()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	//CopyTexture2D에 여태까지 그린 모든 렌더타겟 합산 
	for (int i = 0; i < m_Texture2Ds.size(); ++i)
	{
		Texture2D_Begin(m_CopyBackTexture2Ds, 0, i == 0);
		if (FAILED(m_Texture2Ds[i]->Bind_SVR(m_pShaderCom, "g_Texture")))
			return E_FAIL;

		m_pShaderCom->Begin(0);
		m_pVIBufferCom->Render();

		Texture2D_End();

		//Texture2D_Begin(m_CopyBackTexture2Ds, 0, false);
		//if (FAILED(m_BinaryTexture2Ds[i]->Bind_SVR(m_pShaderCom, "g_Texture")))
		//	return E_FAIL;

		//m_pShaderCom->Begin(3);
		//m_pVIBufferCom->Render();

		//Texture2D_End();
	}

	return S_OK;
}

HRESULT CObject_Manager::Render_BackBuffer()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_CopyBackTexture2Ds[0]->Bind_SVR(m_pShaderCom, "g_Texture")))//모든 데이터 합산.
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CObject_Manager::Add_Group(const wstring& strObjectTag) 
{
	//그룹 생성
	//그룹 생성 후 RenderTexture에 들어갈 오브젝트용 map과 바이너리화를 진행할 오브젝트용 map에 똑같이 저장한다.

	vector<CUI_Object*>* pObjects = Find_RenderTextureObject(strObjectTag);

	if (nullptr != pObjects)
		return E_FAIL;

	vector<CUI_Object*> Object;
	vector<CUI_Object*> TypeObjects;
	
	m_Objects.emplace(strObjectTag, Object);
	m_BinaryObjects.emplace(strObjectTag, TypeObjects);

	if (FAILED(Create_Texture2D()))
		return E_FAIL;

	if (FAILED(Create_BinaryTexture2D()))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_Manager::Add_RenderTextureObject(const wstring& strObjectTag, void* pArg)
{
	//RenderTargetGroup에 오브젝트 추가

	vector<CUI_Object*>* pObjects = Find_RenderTextureObject(strObjectTag);

	OBJ_MNG_DESC* pDesc = static_cast<OBJ_MNG_DESC*>(pArg);
	_uint iType = pDesc->iTextureType;
	// 타입별로 클래스 생성.

	if (nullptr == pObjects)
	{
		//생성
		vector<CUI_Object*> Objects;
		pObjects = &Objects;
	}

	if (iType == IMG)
	{
		CUI_Texture::UI_TEXTURE_DESC TextureDesc = {};
		TextureDesc.strTextureFileName = pDesc->strFileName;
		TextureDesc.strTextureFilePath = pDesc->strFilePath;
		TextureDesc.iTypeIndex = pDesc->iTextureType;
		TextureDesc.strName = pDesc->strName;
		CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &TextureDesc));
		if (nullptr == pImage)
			return E_FAIL;

		pObjects->emplace_back(pImage);
	}
	else if (iType == TEXT)
	{
		CText::TEXT_DESC TextDesc = {};
		TextDesc.strText = pDesc->strText;
		TextDesc.vColor = pDesc->vColor;
		TextDesc.strName = pDesc->strName;
		TextDesc.iTypeIndex = pDesc->iTextureType;	
		CText* pText = dynamic_cast<CText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Text"), &TextDesc));
		if (nullptr == pText)
			return E_FAIL;

		pObjects->emplace_back(pText);
	}

	if (nullptr == pObjects)
	{
		m_Objects.emplace(strObjectTag, *pObjects);
	}

	return S_OK;
}

HRESULT CObject_Manager::Copy_RenderTextureObject(const wstring& strObjectTag, _uint iIndex)
{
	//RenderTargetGroup에 오브젝트 추가

	vector<CUI_Object*>* pObjects = Find_RenderTextureObject(strObjectTag);

	//복사 객체 찾아오기 (인덱스로)
	OBJ_MNG_DESC pDesc{};
	pDesc.iTextureType = (*pObjects)[iIndex]->Get_TypeIndex();
	static _uint Index = 0;


	// 타입별로 클래스 생성.
	if (nullptr == pObjects)
	{
		//생성
		vector<CUI_Object*> Objects;	
		pObjects = &Objects;	
	}

	if (pDesc.iTextureType == IMG)
	{
		
		CUI_Texture::UI_TEXTURE_DESC TextureDesc = {};
		TextureDesc.strTextureFileName = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_FileName();
		TextureDesc.strTextureFilePath = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_FilePath();
		TextureDesc.iTypeIndex = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_TypeIndex();
		TextureDesc.strName = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_Name()+"%d";
		char buffer[MAX_PATH] = { "" };
		sprintf(buffer, TextureDesc.strName.c_str(), Index);
		string result(buffer);
		TextureDesc.strName = result;

		CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &TextureDesc));
		if (nullptr == pImage)
			return E_FAIL;

		pImage->Set_SizeX(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_SizeX());
		pImage->Set_SizeY(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_SizeY());
		pImage->Set_ShaderPass(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_ShaderPass());
		pImage->Set_Color(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_Color());
		pImage->Change_UV(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_StartUV(), dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_EndUV());
		pImage->Get_TransformCom()->Set_WorldMatrix(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_TransformCom()->Get_WorldMatrix());
		pObjects->emplace_back(pImage);
	}
	else if (pDesc.iTextureType == TEXT)
	{
		CText::TEXT_DESC TextDesc = {};
		TextDesc.strText = dynamic_cast<CText*>((*pObjects)[iIndex])->Get_Text();	
		TextDesc.vColor = dynamic_cast<CText*>((*pObjects)[iIndex])->Get_Color();

		CText* pText = dynamic_cast<CText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Text"), &TextDesc));
		if (nullptr == pText)
			return E_FAIL;

		pObjects->emplace_back(pText);
	}

	if (nullptr == pObjects)
	{
		m_Objects.emplace(strObjectTag, *pObjects);
	}

	return S_OK;
}

HRESULT CObject_Manager::Add_BinaryObject(const wstring& strObjectTag, void* pArg)
{
	//BinaryObjects에 오브젝트 추가

	vector<CUI_Object*>* pBinaryObjects = Find_BinaryObject(strObjectTag);

	OBJ_MNG_DESC* pDesc = static_cast<OBJ_MNG_DESC*>(pArg);
	_uint iType = pDesc->iTextureType;

	if (nullptr == pBinaryObjects)
	{
		vector<CUI_Object*> Objects;
		pBinaryObjects = &Objects;
	}

	// 타입별로 클래스 생성.
	if (iType == IMG || iType == HEADUI)
	{
		CUI_Texture::UI_TEXTURE_DESC TextureDesc = {};
		TextureDesc.strTextureFileName = pDesc->strFileName;
		TextureDesc.strTextureFilePath = pDesc->strFilePath;
		TextureDesc.strName = pDesc->strName;
		TextureDesc.iTypeIndex = pDesc->iTextureType;

		CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &TextureDesc));
		if (nullptr == pImage)
			return E_FAIL;

		pBinaryObjects->emplace_back(pImage);
	}
	else if (iType == BTN)
	{
		CBtn::BTN_DESC BtnDesc{};
		BtnDesc.strTextureFilePath = pDesc->strFilePath;
		BtnDesc.strTextureFileName = pDesc->strFileName;
		BtnDesc.strClickFileName = pDesc->strBtnClickFileName;
		BtnDesc.strClickFilePath = pDesc->strBtnClickFilePath;
		BtnDesc.strName = pDesc->strName;
		BtnDesc.iTypeIndex = pDesc->iTextureType;


		CBtn* pBtn	= dynamic_cast<CBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Btn"), &BtnDesc));
		if (nullptr == pBtn)
			return E_FAIL;
		pBinaryObjects->emplace_back(pBtn);
	}
	else if (iType == TEXT)
	{
		CText::TEXT_DESC TextDesc = {};
		TextDesc.strText = pDesc->strText;
		TextDesc.vColor = pDesc->vColor;
		TextDesc.strName = pDesc->strName;
		TextDesc.iTypeIndex = pDesc->iTextureType;

		CText* pText = dynamic_cast<CText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Text"), &TextDesc));
		if (nullptr == pText)
			return E_FAIL;

		pBinaryObjects->emplace_back(pText);
	}
	else if (iType == GROUP)
	{
		CGroup::UI_OBJECT_DESC GroupDesc{};
		GroupDesc.strName = pDesc->strName;
		GroupDesc.iTypeIndex = pDesc->iTextureType;

		CGroup* pGroup = dynamic_cast<CGroup*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Group"), &GroupDesc));
		if (nullptr == pGroup)
			return E_FAIL;

		pBinaryObjects->emplace_back(pGroup);
	}
	else if (iType == EFFECT)
	{
		CUI_Effect::UI_EFFECT_DESC EffectDesc{};
		EffectDesc.strTextureFileName = pDesc->strFileName;
		EffectDesc.strTextureFilePath = pDesc->strFilePath;
		EffectDesc.strName = pDesc->strName;
		EffectDesc.iTypeIndex = pDesc->iTextureType;
		EffectDesc.vLifeTime = pDesc->vLifeTime;
		EffectDesc.fSpeed = pDesc->fSpeed;
		EffectDesc.vColor = pDesc->vColor;

		CUI_Effect* pUIEffect = dynamic_cast<CUI_Effect*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIEffect"), &EffectDesc));
		if (nullptr == pUIEffect)
			return E_FAIL;

		pBinaryObjects->emplace_back(pUIEffect);
	}


	if (nullptr == pBinaryObjects)
	{
		m_Objects.emplace(strObjectTag, *pBinaryObjects);
	}

	return S_OK;
}

HRESULT CObject_Manager::Copy_BinaryObject(const wstring& strObjectTag, _uint iIndex)
{
	vector<CUI_Object*>* pObjects = Find_BinaryObject(strObjectTag);

	_uint Type =(*pObjects)[iIndex]->Get_TypeIndex();
	static _uint Index = 0;

	if (IMG == Type)
	{
		CUI_Texture::UI_TEXTURE_DESC TextureDesc = {};
		TextureDesc.strTextureFileName = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_FileName();	
		TextureDesc.strTextureFilePath = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_FilePath();	
		TextureDesc.iTypeIndex = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_TypeIndex();	
		TextureDesc.strName = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_Name() + "%d";	
		char buffer[MAX_PATH] = { "" };	
		sprintf(buffer, TextureDesc.strName.c_str(), Index);		
		string result(buffer);	
		TextureDesc.strName = result;	

		TextureDesc.bAnim = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_isAnim();
		TextureDesc.vStartPos = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_StartPos();
		TextureDesc.fAnimTime = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_AnimTime();
		TextureDesc.vColor = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_Color();
		TextureDesc.isColor = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_isColor();
		TextureDesc.fControlAlpha = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_ControlAlpha();
		TextureDesc.iShaderPass = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_ShaderPass();

		TextureDesc.vUpPoint = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_UpPoint();
		TextureDesc.vDownPoint = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_DownPoint();
		TextureDesc.vEndColor = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_EndColor();
		TextureDesc.isScreen = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_isScreen();
		TextureDesc.vAnimScale = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_AnimScale();
		CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &TextureDesc));
		if (nullptr == pImage)
			return E_FAIL;

		pImage->Set_SizeX(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_SizeX());
		pImage->Set_SizeY(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_SizeY());

		pImage->Change_UV(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_StartUV(), dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_EndUV());
		pImage->Get_TransformCom()->Set_WorldMatrix(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_TransformCom()->Get_WorldMatrix());

		pObjects->emplace_back(pImage);
	}
	else if(BTN== Type)
	{
		CBtn::BTN_DESC BtnDesc = {};
		BtnDesc.strTextureFileName = dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_FileName();
		BtnDesc.strTextureFilePath = dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_FilePath();

		BtnDesc.strClickFileName = dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_ClickFileName();
		BtnDesc.strClickFilePath = dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_ClickFilePath();

		BtnDesc.iTypeIndex = dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_TypeIndex();
		BtnDesc.strName = dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_Name() + "%d";
		char buffer[MAX_PATH] = { "" };
		sprintf(buffer, BtnDesc.strName.c_str(), Index);
		string result(buffer);
		BtnDesc.strName = result;

		BtnDesc.bAnim = dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_isAnim();
		BtnDesc.vStartPos = dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_StartPos();
		BtnDesc.fAnimTime = dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_AnimTime();
		BtnDesc.vColor = dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_Color();
		BtnDesc.isColor = dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_isColor();
		BtnDesc.fControlAlpha = dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_ControlAlpha();
		BtnDesc.iShaderPass = dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_ShaderPass();

		CBtn* pBtn = dynamic_cast<CBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Btn"), &BtnDesc));
		if (nullptr == pBtn)
			return E_FAIL;

		pBtn->Set_SizeX(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_SizeX());
		pBtn->Set_SizeY(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_SizeY());

		pBtn->Change_UV(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_StartUV(), dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_EndUV());
		pBtn->Chage_ClickUV(dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_ClickStartUV(), dynamic_cast<CBtn*>((*pObjects)[iIndex])->Get_ClickEndUV());
		pBtn->Get_TransformCom()->Set_WorldMatrix(dynamic_cast<CUI_Texture*>((*pObjects)[iIndex])->Get_TransformCom()->Get_WorldMatrix());

		pObjects->emplace_back(pBtn);
	}

	return S_OK;
}

HRESULT CObject_Manager::Copy_BinaryGroupObject(const wstring& strObjectTag, const _uint ibinaryIndex, _uint iIndex)
{
	vector<CUI_Object*>* pFineObjects = Find_BinaryObject(strObjectTag);	
	if (nullptr == pFineObjects)
		return E_FAIL;

	vector<class CUI_Object*>* pFineGroupObjects = dynamic_cast<CGroup*>((*pFineObjects)[ibinaryIndex])->Get_pPartObjects();	


	_uint Type = (*pFineGroupObjects)[iIndex]->Get_TypeIndex();
	static _uint Index = 0;

	if (IMG == Type)
	{
		CUI_Texture::UI_TEXTURE_DESC TextureDesc = {};
		TextureDesc.strTextureFileName = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_FileName();
		TextureDesc.strTextureFilePath = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_FilePath();
		TextureDesc.iTypeIndex = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_TypeIndex();
		TextureDesc.strName = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_Name() + "%d";
		char buffer[MAX_PATH] = { "" };
		sprintf(buffer, TextureDesc.strName.c_str(), Index);
		string result(buffer);
		TextureDesc.strName = result;

		TextureDesc.bAnim = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_isAnim();
		TextureDesc.vStartPos = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_StartPos();
		TextureDesc.fAnimTime = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_AnimTime();
		TextureDesc.isColor = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_isColor();
		TextureDesc.fControlAlpha = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_ControlAlpha();
		TextureDesc.isParent = true;
		TextureDesc.pParentMatrix = (*pFineObjects)[ibinaryIndex]->Get_TransformCom()->Get_WorldFloat4x4();

		TextureDesc.isScreen = (*pFineGroupObjects)[iIndex]->Get_isScreen();
		TextureDesc.isEvent = (*pFineGroupObjects)[iIndex]->Get_Event();

		TextureDesc.vUpPoint = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_UpPoint();
		TextureDesc.vDownPoint = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_DownPoint();
		TextureDesc.vAnimScale = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_AnimScale();


		CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &TextureDesc));
		if (nullptr == pImage)
			return E_FAIL;

		pImage->Set_SizeX(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_SizeX());
		pImage->Set_SizeY(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_SizeY());
		pImage->Set_ShaderPass(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_ShaderPass());
		pImage->Set_Color(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_Color());
		pImage->Change_UV(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_StartUV(), dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_EndUV());
		pImage->Get_TransformCom()->Set_WorldMatrix(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_TransformCom()->Get_WorldMatrix());


		pFineGroupObjects->emplace_back(pImage);
	}
	else if (TEXT == Type)
	{
		CText::TEXT_DESC TextDesc = {};
		TextDesc.strTextureFileName = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_FileName();
		TextDesc.strTextureFilePath = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_FilePath();
		TextDesc.iTypeIndex = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_TypeIndex();
		TextDesc.strName = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_Name() + "%d";
		char buffer[MAX_PATH] = { "" };
		sprintf(buffer, TextDesc.strName.c_str(), Index);
		string result(buffer);
		TextDesc.strName = result;

		TextDesc.bAnim = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_isAnim();
		TextDesc.vStartPos = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_StartPos();
		TextDesc.fAnimTime = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_AnimTime();
		TextDesc.isColor = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_isColor();
		TextDesc.vColor = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_Color();
		TextDesc.vEndColor = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_EndColor();
		TextDesc.fControlAlpha = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_ControlAlpha();
		TextDesc.isParent = true;
		TextDesc.pParentMatrix = (*pFineObjects)[ibinaryIndex]->Get_TransformCom()->Get_WorldFloat4x4();

		TextDesc.isScreen = (*pFineGroupObjects)[iIndex]->Get_isScreen();
		TextDesc.isEvent = (*pFineGroupObjects)[iIndex]->Get_Event();

		TextDesc.vUpPoint = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_UpPoint();
		TextDesc.vDownPoint = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_DownPoint();

		TextDesc.iAlign = dynamic_cast<CText*>((*pFineGroupObjects)[iIndex])->Get_Align();
		TextDesc.strText = dynamic_cast<CText*>((*pFineGroupObjects)[iIndex])->Get_Text();
		TextDesc.Font = dynamic_cast<CText*>((*pFineGroupObjects)[iIndex])->Get_Font();
		CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Text"), &TextDesc));
		if (nullptr == pImage)
			return E_FAIL;		

		pImage->Set_ShaderPass(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_ShaderPass());
		pImage->Get_TransformCom()->Set_WorldMatrix(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_TransformCom()->Get_WorldMatrix());

		pFineGroupObjects->emplace_back(pImage);
	}
	else if (EFFECT == Type)
	{
		CUI_Effect::UI_EFFECT_DESC TextureDesc = {};

		TextureDesc.strTextureFileName = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_FileName();
		TextureDesc.strTextureFilePath = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_FilePath();
		TextureDesc.iTypeIndex = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_TypeIndex();
		TextureDesc.strName = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_Name() + "%d";
		char buffer[MAX_PATH] = { "" };
		sprintf(buffer, TextureDesc.strName.c_str(), Index);
		string result(buffer);
		TextureDesc.strName = result;

		TextureDesc.bAnim = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_isAnim();
		TextureDesc.vStartPos = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_StartPos();
		TextureDesc.fAnimTime = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_AnimTime();
		TextureDesc.isColor = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_isColor();
		TextureDesc.fControlAlpha = dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_ControlAlpha();
		TextureDesc.isParent = true;
		TextureDesc.pParentMatrix = (*pFineObjects)[ibinaryIndex]->Get_TransformCom()->Get_WorldFloat4x4();

		TextureDesc.vLifeTime = dynamic_cast<CUI_Effect*>((*pFineGroupObjects)[iIndex])->Get_LifeTime();
		TextureDesc.fSpeed = dynamic_cast<CUI_Effect*>((*pFineGroupObjects)[iIndex])->Get_Speed();

		TextureDesc.isScreen = (*pFineGroupObjects)[iIndex]->Get_isScreen();
		TextureDesc.isEvent = (*pFineGroupObjects)[iIndex]->Get_Event();
		CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIEffect"), &TextureDesc));
		if (nullptr == pImage)
			return E_FAIL;

		pImage->Set_SizeX(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_SizeX());
		pImage->Set_SizeY(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_SizeY());
		pImage->Set_ShaderPass(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_ShaderPass());
		pImage->Set_Color(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_Color());
		pImage->Change_UV(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_StartUV(), dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_EndUV());
		pImage->Get_TransformCom()->Set_WorldMatrix(dynamic_cast<CUI_Texture*>((*pFineGroupObjects)[iIndex])->Get_TransformCom()->Get_WorldMatrix());


		pFineGroupObjects->emplace_back(pImage);
	}

	return S_OK;
}

vector<CUI_Object*>* CObject_Manager::Find_RenderTextureObject(const wstring& strObjectTag)
{
	auto pObject = m_Objects.find(strObjectTag);

	if (pObject == m_Objects.end())
		return nullptr;

	return &(pObject->second);
}

vector<class CUI_Object*>* CObject_Manager::Find_BinaryObject(const wstring& strObjectTag)
{
	auto pObject = m_BinaryObjects.find(strObjectTag);

	if (pObject == m_BinaryObjects.end())
		return nullptr;

	return &(pObject->second);
}

HRESULT CObject_Manager::Create_Texture2D()
{
	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, 1280.f, 720.f, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f),false);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_Texture2Ds.emplace_back(pRenderTarget);


#ifdef _DEBUG
	pRenderTarget->Ready_Debug(m_fDebugStart + ( 100 * m_iTextureWidthCount), m_fDebugStart + (100 * m_iTextureHeightCount), 100.f, 100.f);
#endif // _DEBUG

	
	m_iTextureHeightCount++;
	if (m_fDebugStart * m_iTextureHeightCount >= 450.f)
	{
		m_iTextureHeightCount = 0.f;
		m_iTextureWidthCount++;
	}

	return S_OK;
}

HRESULT CObject_Manager::Create_BinaryTexture2D()
{
	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, 1280.f, 720.f, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f),false);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_BinaryTexture2Ds.emplace_back(pRenderTarget);

	return S_OK;
}

HRESULT CObject_Manager::Texture2D_Begin(vector<CRenderTarget*> Textures, _uint iIndex, _bool isClear)
{
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
	nullptr
	};

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV); //초기화 해주는 작업

	_uint		iNumRenderTargets = {};

	ID3D11RenderTargetView* pRenderTargets[8] = {};

	if (isClear) Textures[iIndex]->Clear();
	pRenderTargets[iNumRenderTargets++] = Textures[iIndex]->Get_RTV();

	m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, m_pDepthStencilView);

	return S_OK;
}

HRESULT CObject_Manager::Texture2D_End()
{
	m_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);
	return S_OK;
}

void CObject_Manager::Release_ObjectManager()
{
	//CObject_Manager::GetInstance()->Free();

	CObject_Manager::GetInstance()->DestroyInstance();
}

void CObject_Manager::Free()
{
	for (auto& Pair : m_Objects)
	{
		for (auto& pObj : Pair.second)
			Safe_Release(pObj);
	}

	for (auto& Pair : m_BinaryObjects)
	{
		for (auto& pObj : Pair.second)
			Safe_Release(pObj);
	}

	for (auto& pTexture2D : m_Texture2Ds)
		Safe_Release(pTexture2D);

	for (auto& pTexture2D : m_BinaryTexture2Ds)
		Safe_Release(pTexture2D);

	for (auto& pTexture2D : m_CopyBackTexture2Ds)
		Safe_Release(pTexture2D);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferView);


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
}
