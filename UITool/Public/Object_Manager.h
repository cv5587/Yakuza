#pragma once
#include "Base.h"
#include "UITool_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CRenderTarget;
class CShader;
class CVIBuffer_Rect;
END

BEGIN(UITool)
class CObject_Manager :
    public CBase
{
    DECLARE_SINGLETON(CObject_Manager)

    //HEADUI와 IMG는 비슷해서 툴에서 볼때는 IMG 로 보고 
    //클라에서 쓸때는 HEADUI로 씀

public:
    enum TEXTURE_TYPE { IMG, BTN, TEXT,  GROUP, EFFECT, HEADUI, TEXTURE_TYPE_END };
    enum MOVE_TYPE { UP, DOWN, MOVE_END };
    typedef struct {
        _uint iTextureType;
        wstring strFilePath;
        wstring strFileName;
        wstring strBtnClickFilePath;
        wstring strBtnClickFileName;
        wstring strText;
        _uint iTextType;
        _float4 vColor;
        //이펙트
        _float3 vLifeTime;
        _float fSpeed;

        string strName;
    } OBJ_MNG_DESC;

private:
    CObject_Manager();
    virtual ~CObject_Manager() = default;

public:
    map<const wstring, vector<class CUI_Object*>> Get_Objects() { return m_Objects; }
    vector<class CUI_Object*> Get_GroupObject(const wstring& strTag);
    vector<class CUI_Object*> Get_BinaryGroupObject(const wstring& strTag, _int iIndex); // 바이너리 오브젝트들 중 그룹인 오브젝트가 가지고 있는 리스트를 가지고 있다.

    map<const wstring, vector<class CUI_Object*>> Get_BinaryObjects() { return m_BinaryObjects; }
    vector<class CUI_Object*> Get_GroupBinaryObject(const wstring& strTag);

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Tick(const _float& fTimeDelta);
    void Late_Tick(const _float& fTimeDelta);
    HRESULT Render();

public://렌더 진행
    HRESULT Render_Object();
    HRESULT Render_Copy();
    HRESULT Render_BackBuffer();

public:
    HRESULT Add_Group(const wstring& strObjectTag);
    HRESULT Add_RenderTextureObject(const wstring& strObjectTag, void* pArg);
    HRESULT Copy_RenderTextureObject(const wstring& strObjectTag, _uint iIndex);
    HRESULT Add_BinaryObject(const wstring& strObjectTag, void* pArg);
    HRESULT Copy_BinaryObject(const wstring& strObjectTag, _uint iIndex);
    HRESULT Copy_BinaryGroupObject(const wstring& strObjectTag, const _uint ibinaryIndex, _uint iIndex);

    HRESULT Copy_Group(const wstring& strTag);
    HRESULT Remove_Group(const wstring& strTag);
    HRESULT Remove_GroupObject(const wstring& strTag, const _uint ibinaryIndex, const _uint iIndex);
    HRESULT Remove_Object(const wstring& strTag, _uint iIndex);
    HRESULT Remove_BinaryObject(const wstring& strTag, _uint iIndex);
    HRESULT Create_Texture(_uint iIndex, const wstring& strFilePath);

    HRESULT Move_ObjectIndex(const wstring& strTag, _uint iIndex, _uint iMoveType);
    HRESULT Move_BinaryObjectIndex(const wstring& strTag, _uint iIndex, _uint iMoveType);
    HRESULT Move_BinaryGroupObjectIndex(const wstring& strTag, const _uint ibinaryIndex, _uint iIndex, _uint iMoveType);


public :
    HRESULT Save_binary();
    HRESULT Load_binary(const wstring& strObjectTag, const string FilePath);
private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };

    vector<CRenderTarget*> m_Texture2Ds;
    vector<CRenderTarget*> m_BinaryTexture2Ds;
    vector< CRenderTarget*> m_CopyBackTexture2Ds;
    map<const wstring, vector<class CUI_Object*>> m_Objects;//렌더타겟용
    map<const wstring, vector<class CUI_Object*>> m_BinaryObjects;//바이너리화 할애들

    
private: // Render용
    ID3D11RenderTargetView* m_pBackBufferView = { nullptr };
    ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };

    CShader* m_pShaderCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

    _float m_fDebugStart = { 50.f };
    _float m_iTextureHeightCount = { 0.f };
    _float m_iTextureWidthCount = { 0.f };

    _float4x4 m_WorldMatrix;
    _float4x4 m_ViewMatrix;
    _float4x4 m_ProjMatrix;

private:
    vector<class CUI_Object*>* Find_RenderTextureObject(const wstring& strObjectTag);
    vector<class CUI_Object*>* Find_BinaryObject(const wstring& strObjectTag);

    HRESULT Create_Texture2D();
    HRESULT Create_BinaryTexture2D();
    HRESULT Texture2D_Begin(vector<CRenderTarget*> Textures, _uint iIndex, _bool isClear = true);
    HRESULT Texture2D_End();

public: 
    static void Release_ObjectManager();
    virtual void Free();
};
END
