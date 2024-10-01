#include "../Public/Imgui_Manager.h"
#include "GameInstance.h"
#include "../../Imgui/background/imgui_impl_win32.h"
#include "../../Imgui/background/imgui_impl_dx11.h"

#include <Windows.h>
#include <string.h>

#include "PipeLine.h"

#include "ImGuiFileDialog.h"

#include "Object_Manager.h"
#include "UI_Texture.h"
#include "Image_Texture.h"
#include "Text.h"
#include "Btn.h"
#include "Group.h"
#include "UI_Effect.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager() :
    m_pGameInstance { CGameInstance::GetInstance()},
    m_pObjectManager { CObject_Manager::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pObjectManager);
}

HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    m_pObjectManager->Initialize(pDevice, pContext);

    // Win32 초기화 호출
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.Fonts->AddFontFromFileTTF("C:\\Windows\\fonts\\malgun.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());

    /* IMGUI 초기화 */
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.0f));

    filesystem::path projectRootDir = filesystem::current_path();
    filesystem::path ParentDir = projectRootDir.parent_path();
    m_RootDir = ParentDir.parent_path();

    m_Font.push_back(TEXT("Font_nanum24"));
    m_Font.push_back(TEXT("Font_Default"));
    m_Font.push_back(TEXT("Font_nanum30"));
    m_Font.push_back(TEXT("Font_nanum36"));

    return S_OK;
}

void CImgui_Manager::Priority_Tick(_float fTimeDelta)
{
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 0.00f);

    //매 업데이트마다 돌아감
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame(); //new와 같은것 선언해주기

    ImGuiIO& io = ImGui::GetIO();

    Window_Image();
    Window_Binary();
    Window_Binary_Group();

    Guizmo_Test();
}

void CImgui_Manager::Window_Image()
{
    ImGui::Begin(u8"렌더타겟 이미지 생성",nullptr , ImGuiWindowFlags_NoCollapse);

    if (ImGui::CollapsingHeader(u8"그룹 / 렌더타겟 이미지 생성"))
    {
        //1.그룹을 만들기
        auto Objects = m_pObjectManager->Get_Objects();
        ImGui::Text(u8"UI 틀 이름");
        ImGui::InputText(u8" ", m_szGroupName, MAX_PATH);
        if (ImGui::Button(u8"UI 틀 생성"))
        {
            if ('\0' != m_szGroupName[0])
            {
                if (FAILED(m_pObjectManager->Add_Group(m_pGameInstance->StringToWstring(string(m_szGroupName)))))
                {
                    MSG_BOX("Group 생성 실패 / 이미 존재하는 그룹입니다.");
                }
            }
            else
                MSG_BOX("이름 부터 지어줘!");
        }


        if (Objects.size() != 0)
        {
            auto iter = Objects.begin();
            string strName = "";
            if (ImGui::BeginListBox("GroupList"))
            {
                for (int n = 0; n < Objects.size(); n++)
                {
                    const bool is_selected = (m_iGroupIndex == n);
                    strName = m_pGameInstance->WstringToString(iter->first);
                    if (ImGui::Selectable(strName.c_str(), is_selected))
                    {
                        m_iGroupIndex = n;
                        m_strSelectTag = strName;
                        m_iGroupObjectIndex = -1; // 초기화 시키기
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();

                    ++iter;
                }
                ImGui::EndListBox();
            }

            if (ImGui::Button("Group Remove")&& '\0' != m_strSelectTag[0])
            {
                 m_pObjectManager->Remove_Group(m_pGameInstance->StringToWstring(m_strSelectTag));
                 m_iGroupIndex--;

                 if (m_iGroupIndex < 0 || Objects.size() <= 1) // 기존에 받아왔던 Objects 사이즈가 1보다 같거나 작다면
                     m_iGroupIndex = -1;

                 m_iGroupObjectIndex = -1; // 초기화

            }

            ImGui::SameLine();

            if (ImGui::Button("Group Copy"))
            {
                m_pObjectManager->Copy_Group(m_pGameInstance->StringToWstring(m_strSelectTag));
            }

            ImGui::RadioButton(u8"이미지 생성", &m_iObjectType, CObject_Manager::IMG); ImGui::SameLine();
            ImGui::RadioButton(u8"텍스트 생성", &m_iObjectType, CObject_Manager::TEXT);

            ImGui::InputText(u8"이미지이름", m_szImageName, MAX_PATH);

            if (m_iObjectType == CObject_Manager::IMG)
            {
                if (ImGui::Button(u8"이미지 파일 생성"))
                {
                    IGFD::FileDialogConfig config;
                    config.path = "../../Client/Bin/Resources/Textures/UI";
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseTextureKey", "Choose File", ".dds, .png", config);
                }
                if (ImGuiFileDialog::Instance()->Display("ChooseTextureKey")) {
                    if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                        std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                        // action
                        if('\0'!= m_szImageName[0])
                        {
                            CObject_Manager::OBJ_MNG_DESC Desc{};
                            Desc.strName = string(m_szImageName);
                            Desc.strFileName = m_pGameInstance->StringToWstring(fileName);
                            Desc.strFilePath = m_pGameInstance->StringToWstring(filePathName);
                            Desc.iTextureType = CObject_Manager::IMG;

                            if (FAILED(m_pObjectManager->Add_RenderTextureObject(m_pGameInstance->StringToWstring(strName), &Desc)))
                                MSG_BOX("이미지 생성 실패");
                        }
                        else
                        {
                            MSG_BOX("이름 작성");
                        }
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }
            }
            else if (m_iObjectType == CObject_Manager::TEXT)
            {
                ImGui::InputText(u8"텍스트 작성 : ", m_szText, MAX_PATH);

                //static ImVec4 vAmbientColor = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
                //ImGui::ColorEdit4(u8"Color 선택", (float*)&m_vColor, ImGuiColorEditFlags_Float);

                if (ImGui::Button("Create"))
                {
                    //텍스쳐 생성
                    CObject_Manager::OBJ_MNG_DESC Desc{};
                    Desc.strText = m_pGameInstance->StringToWstring(string(m_szText));
                    Desc.iTextureType = m_iObjectType;
                    Desc.vColor = m_vColor;
                    Desc.strName = string(m_szImageName);

                    m_pObjectManager->Add_RenderTextureObject(m_pGameInstance->StringToWstring(strName), &Desc);
                }
            }
            
            if (m_iGroupIndex != -1)
            {
                vector<CUI_Object*> Textures = m_pObjectManager->Get_GroupObject(m_pGameInstance->StringToWstring(m_strSelectTag));

                string strObjectName = "";

                if (Textures.size() > 0)
                {
                    auto iter = Textures.begin();
                    if (ImGui::BeginListBox("ObjectList"))
                    {
                        for (int n = 0; n < Textures.size(); n++)
                        {
                            const bool is_selected = (m_iGroupObjectIndex == n);
                            strObjectName = dynamic_cast<CUI_Texture*>(Textures[n])->Get_Name();
                            if (ImGui::Selectable(strObjectName.c_str(), is_selected))
                            {
                                m_iGroupObjectIndex = n;

                                m_WorldMatirx = *Textures[n]->Get_TransformCom()->Get_WorldFloat4x4();
                                m_StartUV = dynamic_cast<CUI_Texture*>(Textures[n])->Get_StartUV();
                                m_EndUV = dynamic_cast<CUI_Texture*>(Textures[n])->Get_EndUV();
                                m_iBinaryPick = RENDER_TARGET;
                            }
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();

                            if (iter != Textures.end())
                                ++iter;
                        }
                        ImGui::EndListBox();
                    }

                    if (ImGui::Button("Object Remove")&&-1!= m_iGroupObjectIndex)
                    {
                        m_pObjectManager->Remove_Object(m_pGameInstance->StringToWstring(m_strSelectTag), m_iGroupObjectIndex);
                        m_iGroupObjectIndex = -1; // 초기화 시키기
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"앞으로"))
                    {
                        if (FAILED(m_pObjectManager->Move_ObjectIndex(m_pGameInstance->StringToWstring(m_strSelectTag), m_iGroupObjectIndex, CObject_Manager::UP)))
                            MSG_BOX("이동 실패");
                        else
                            m_iGroupObjectIndex++;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"뒤로"))
                    {
                        if (FAILED(m_pObjectManager->Move_ObjectIndex(m_pGameInstance->StringToWstring(m_strSelectTag), m_iGroupObjectIndex, CObject_Manager::DOWN)))
                            MSG_BOX("이동 실패");
                        else
                            m_iGroupObjectIndex--;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"복사"))
                    {
                        if (FAILED(m_pObjectManager->Copy_RenderTextureObject(m_pGameInstance->StringToWstring(m_strSelectTag), m_iGroupObjectIndex)))
                            MSG_BOX("이미지 생성 실패");
                    }
                    Textures = m_pObjectManager->Get_GroupObject(m_pGameInstance->StringToWstring(m_strSelectTag)); // 파일 다시 불러오기
                }

                ImGui::Text(to_string(m_iGroupObjectIndex).c_str());
                if (m_iGroupObjectIndex != -1 && m_iBinaryPick == RENDER_TARGET)
                {
                    ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");

                    if (ImGui::IsKeyPressed(ImGuiKey_W))
                        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
                    if (ImGui::IsKeyPressed(ImGuiKey_E))
                        mCurrentGizmoOperation = ImGuizmo::ROTATE;
                    if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
                        mCurrentGizmoOperation = ImGuizmo::SCALE;

                    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
                        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
                    ImGui::SameLine();
                    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
                        mCurrentGizmoOperation = ImGuizmo::ROTATE;
                    ImGui::SameLine();
                    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
                        mCurrentGizmoOperation = ImGuizmo::SCALE;

                    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
                    ImGuizmo::DecomposeMatrixToComponents((_float*)&m_WorldMatirx, matrixTranslation, matrixRotation, matrixScale);
                    ImGui::InputFloat3("Tr", matrixTranslation);
                    ImGui::InputFloat3("Rt", matrixRotation);
                    ImGui::InputFloat3("Sc", matrixScale);
                    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (_float*)&m_WorldMatirx);

                    Textures[m_iGroupObjectIndex]->Get_TransformCom()->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatirx));


                    _bool isColor = dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Get_isColor();
                    if (ImGui::Checkbox("isColor", &isColor))
                        dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_isColor(isColor);


                    //쉐이더 선택
                    _int iPass =  dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Get_ShaderPass();

                    if (ImGui::RadioButton("Alpha Blend", &iPass, 1))
                        dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_ShaderPass(iPass);
                    else if (ImGui::RadioButton("Color Alpha Blend", &iPass, 2))
                        dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_ShaderPass(iPass);


                    if(dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Get_isColor())
                    {
                        _float4 vColor = dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Get_Color();
                        static bool alpha_preview = true;
                        static bool alpha_half_preview = false;
                        static bool drag_and_drop = true;
                        static bool options_menu = true;
                        static bool hdr = false;
                        ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

                        if (ImGui::ColorEdit4("MyColor##2f", (float*)&vColor, ImGuiColorEditFlags_Float | misc_flags))
                        {
                            dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Set_Color(vColor);
                        }
                    }
                    
                    if(CObject_Manager::TEXT != dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Get_TypeIndex())
                    {
                        ImGui::DragFloat2("StartUV", (float*)&m_StartUV, 0.001f, 0.0f, 1.f);
                        ImGui::DragFloat2("EndUV", (float*)&m_EndUV, 0.001f, 0.0f, 1.f);

                        if (FAILED(dynamic_cast<CUI_Texture*>(Textures[m_iGroupObjectIndex])->Change_UV(m_StartUV, m_EndUV)))
                            MSG_BOX("UV 변경 실패");
                    }
                    
                }
            }
        }

        ImGui::NewLine();
        if (ImGui::Button(u8"UI 틀 저장"))
        {
            if("" != m_strSelectTag)
            {
                wstring strFilePath = TEXT("../../Client/Bin/Resources/Textures/UI/CreateFile/") + m_pGameInstance->StringToWstring(m_strSelectTag) + TEXT(".dds");
                if (FAILED(m_pObjectManager->Create_Texture(m_iGroupIndex, strFilePath)))
                {
                    MSG_BOX("UI 이미지 저장 실패");
                }
            }
            else
            {
                MSG_BOX("UI 만들고 저장해줘");
            }
        }
        ImGui::NewLine();
    }

    ImGui::End();
}

void CImgui_Manager::Window_Binary()
{
    ImGui::Begin(u8"바이너리 그룹");

    auto Objects = m_pObjectManager->Get_BinaryObjects();

    //그룹 선택은 렌더타겟 윈도우와 연동되어있다.
    if (Objects.size() != 0)
    {
        auto iter = Objects.begin();
        string strName = "";
        if (ImGui::BeginListBox("GroupList"))
        {
            for (int n = 0; n < Objects.size(); n++)
            {
                const bool is_selected = (m_iBinaryGroupIndex == n);
                strName = m_pGameInstance->WstringToString(iter->first);
                if (ImGui::Selectable(strName.c_str(), is_selected))
                {
                    m_iBinaryGroupIndex = n;
                    m_strBinarySelectTag = strName;
                    m_iBinaryObjectIndex = -1; // 초기화 시키기
                    m_isOpenBinaryGroup = false;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();

                ++iter;
            }
            ImGui::EndListBox();
        }
    }
    ImGui::NewLine();

    if(-1!= m_iBinaryGroupIndex)
    {
        if (ImGui::Button(u8"바이너리 모두저장"))
        {
            m_pObjectManager->Save_binary();
        }

        if (ImGui::Button(u8"바이너리 로드"))
        {
            IGFD::FileDialogConfig config;
            config.path = (m_RootDir / "Client" / "Bin" / "DataFiles" / "UIData").string();
            ImGuiFileDialog::Instance()->OpenDialog("ChooseLoadBinaryKey", "Choose File", ".dat", config);
        }
        if (ImGuiFileDialog::Instance()->Display("ChooseLoadBinaryKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK  
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();

                m_pObjectManager->Load_binary(m_pGameInstance->StringToWstring(m_strBinarySelectTag), filePathName);
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::NewLine();
    }
        ImGui::Checkbox(u8"바이너리 오브젝트 생성", &m_isCreateBinaryObject);




        if (m_isCreateBinaryObject)
        {
            
            ImGui::SeparatorText(u8"오브젝트 생성");
            ImGui::RadioButton(u8"이미지", &m_iBinaryObjectType, CObject_Manager::IMG); ImGui::SameLine();
            ImGui::RadioButton(u8"버튼", &m_iBinaryObjectType, CObject_Manager::BTN); ImGui::SameLine();
            ImGui::RadioButton(u8"텍스트", &m_iBinaryObjectType, CObject_Manager::TEXT); ImGui::SameLine();
            ImGui::RadioButton(u8"그룹", &m_iBinaryObjectType, CObject_Manager::GROUP); ImGui::SameLine();
            ImGui::RadioButton(u8"UI이펙트", &m_iBinaryObjectType, CObject_Manager::EFFECT);
            ImGui::RadioButton(u8"HEADUI", &m_iBinaryObjectType, CObject_Manager::HEADUI);
            ImGui::NewLine();
            ImGui::InputText(u8"저장할 오브젝트 이름 : ", m_szObjectName, MAX_PATH);

            //오브젝트 생성
            if (m_iBinaryObjectType == CObject_Manager::IMG )
            {

                //일반 이미지
                if (ImGui::Button(u8"이미지 파일 생성"))
                {
                    IGFD::FileDialogConfig config;
                    config.path = (m_RootDir /"Client" / "Bin" / "Resources" / "Textures" / "UI").string(); 
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseBinaryTextureKey", "Choose File", ".dds, .png", config);
                }
                if (ImGuiFileDialog::Instance()->Display("ChooseBinaryTextureKey")) {
                    if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK

                        if ('\0' != m_szObjectName[0])
                        {
                            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                            std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                            // action
                                    // 상대 경로 구하기
                            filesystem::path filePath = filePathName;
                            filesystem::path relativePath = filePath.lexically_relative(m_RootDir);

                            CObject_Manager::OBJ_MNG_DESC Desc{};
                            Desc.strFileName = m_pGameInstance->StringToWstring(fileName);
                            Desc.strFilePath = m_pGameInstance->StringToWstring("../../" + relativePath.string());
                            Desc.iTextureType = CObject_Manager::IMG;
                            Desc.strName = m_szObjectName;

                            if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                                MSG_BOX("이미지 생성 실패");
                        }
                        else
                        {
                            MSG_BOX("이름 작성");
                        }
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }
            }
            else if (m_iBinaryObjectType == CObject_Manager::BTN)
            {
                //버튼 생성
                if (ImGui::Button(u8"이미지 파일"))
                {
                    IGFD::FileDialogConfig config;
                    config.path = (m_RootDir / "Client" / "Bin" / "Resources" / "Textures" / "UI").string();
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseBtnTextureKey", "ChooseBtnFile", ".dds, .png", config);
                }
                if (ImGuiFileDialog::Instance()->Display("ChooseBtnTextureKey")) {
                    if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                        filesystem::path filePath = filePathName;   
                        filesystem::path relativePath = filePath.lexically_relative(m_RootDir); 


                        m_strBtnFilePath = "../../" + relativePath.string();    
                        m_strBtnFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }

                if (ImGui::Button(u8"클릭 이미지 파일"))
                {
                    IGFD::FileDialogConfig config;
                    config.path = (m_RootDir / "Client" / "Bin" / "Resources" / "Textures" / "UI").string();
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseBtnClickTextureKey", "Choose File", ".dds, .png", config);
                }
                if (ImGuiFileDialog::Instance()->Display("ChooseBtnClickTextureKey")) {
                    if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                        filesystem::path filePath = filePathName;
                        filesystem::path relativePath = filePath.lexically_relative(m_RootDir);

                        m_strBtnClickFilePath = "../../" + relativePath.string();
                        m_strBtnClickFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }

                if (ImGui::Button(u8"버튼 생성"))
                {
                    if ('\0' != m_szObjectName[0])
                    {
                        CObject_Manager::OBJ_MNG_DESC Desc{};
                        Desc.strFilePath = m_pGameInstance->StringToWstring(m_strBtnFilePath);
                        Desc.strFileName = m_pGameInstance->StringToWstring(m_strBtnFileName);
                        Desc.strBtnClickFilePath = m_pGameInstance->StringToWstring(m_strBtnClickFilePath);
                        Desc.strBtnClickFileName = m_pGameInstance->StringToWstring(m_strBtnClickFileName);
                        Desc.strName = m_szObjectName;
                        Desc.iTextureType = m_iBinaryObjectType;

                        if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                            MSG_BOX("버튼 생성 실패");
                    }
                    else
                        MSG_BOX("이름작성");
                }
            }
            else if (m_iBinaryObjectType == CObject_Manager::TEXT)
            {
                //텍스트 생성
                ImGui::InputText(u8"텍스트 작성 : ", m_szText, MAX_PATH);

                if (ImGui::Button(u8"텍스트 생성"))
                {
                    if ('\0' != m_szObjectName[0])
                    {
                        //텍스쳐 생성
                        CObject_Manager::OBJ_MNG_DESC Desc{};
                        Desc.strText = m_pGameInstance->StringToWstring(string(m_szText));
                        Desc.iTextureType = m_iBinaryObjectType;
                        Desc.vColor = m_vColor;
                        Desc.strName = m_szObjectName;

                        if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                            MSG_BOX("텍스트 오브젝트 생성 실패");
                    }
                    else
                        MSG_BOX("이름작성");
                }
            }
            else if (m_iBinaryObjectType == CObject_Manager::GROUP)
            {
                //그룹 생성
                if (ImGui::Button(u8"그룹 생성"))
                {
                    if ('\0' != m_szObjectName[0])
                    {
                        CObject_Manager::OBJ_MNG_DESC Desc{};
                        string name = m_szObjectName;
                        Desc.strName = "Group_" + name;
                        Desc.iTextureType = m_iBinaryObjectType;
                        Desc.vColor = m_vColor;

                        if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                            MSG_BOX("바이너리 오브젝트 생성 실패");
                    }
                    else
                        MSG_BOX("이름작성");
                }
            }
            else if (m_iBinaryObjectType == CObject_Manager::EFFECT)
            {
                //이펙트 생성
                if (ImGui::Button(u8"이미지 파일 생성"))
                {
                    IGFD::FileDialogConfig config;
                    config.path = (m_RootDir / "Client" / "Bin" / "Resources" / "Textures" / "UI").string();
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseBinaryEffectTextureKey", "Choose File", ".dds, .png", config);
                }
                if (ImGuiFileDialog::Instance()->Display("ChooseBinaryEffectTextureKey")) {
                    if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                        if ('\0' != m_szObjectName[0])
                        {
                            std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                            filesystem::path filePath = filePathName;
                            filesystem::path relativePath = filePath.lexically_relative(m_RootDir);

                            CObject_Manager::OBJ_MNG_DESC Desc{};
                            Desc.strFileName = m_pGameInstance->StringToWstring(fileName);
                            Desc.strFilePath = m_pGameInstance->StringToWstring("../../" + relativePath.string());
                            Desc.iTextureType = m_iBinaryObjectType;
                            Desc.strName = m_szObjectName;

                            if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                                MSG_BOX("이미지 생성 실패");
                        }
                        else
                            MSG_BOX("이름작성");
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }

            }
            else   if (m_iBinaryObjectType == CObject_Manager::HEADUI)
            {

                //일반 이미지
                if (ImGui::Button(u8"이미지 파일 생성"))
                {
                    IGFD::FileDialogConfig config;
                    config.path = (m_RootDir / "Client" / "Bin" / "Resources" / "Textures" / "UI").string();
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseBinaryTextureKey", "Choose File", ".dds, .png", config);
                }
                if (ImGuiFileDialog::Instance()->Display("ChooseBinaryTextureKey")) {
                    if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK

                        if ('\0' != m_szObjectName[0])
                        {
                            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                            std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                            // action
                                    // 상대 경로 구하기
                            filesystem::path filePath = filePathName;
                            filesystem::path relativePath = filePath.lexically_relative(m_RootDir);

                            CObject_Manager::OBJ_MNG_DESC Desc{};
                            Desc.strFileName = m_pGameInstance->StringToWstring(fileName);
                            Desc.strFilePath = m_pGameInstance->StringToWstring("../../" + relativePath.string());
                            Desc.iTextureType = CObject_Manager::HEADUI;
                            Desc.strName = m_szObjectName;

                            if (FAILED(m_pObjectManager->Add_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), &Desc)))
                                MSG_BOX("이미지 생성 실패");
                        }
                        else
                        {
                            MSG_BOX("이름 작성");
                        }
                    }

                    // close
                    ImGuiFileDialog::Instance()->Close();
                }
             }
        }

        //생성된 오브젝트 리스트
        if (m_iBinaryGroupIndex != -1)
        {
            auto BinaryObject = m_pObjectManager->Get_GroupBinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag));

            if (ImGui::BeginListBox("BinaryObjectList"))
            {
                for (int n = 0; n < BinaryObject.size(); n++)
                {
                    const bool is_selected = (m_iBinaryObjectIndex == n);
                    string strBinaryName = BinaryObject[n]->Get_Name();
                    if (ImGui::Selectable(strBinaryName.c_str(), is_selected))
                    {
                        m_iBinaryObjectIndex = n;
                        m_WorldMatirx = *BinaryObject[n]->Get_TransformCom()->Get_WorldFloat4x4();
                        m_iBinaryPick = BINARY_OBJECT;
                        m_isOpenBinaryGroup = BinaryObject[m_iBinaryObjectIndex]->Get_TypeIndex() == CObject_Manager::GROUP;

                        if(CObject_Manager::GROUP !=BinaryObject[m_iBinaryObjectIndex]->Get_TypeIndex())
                        {
                            m_StartUV = dynamic_cast<CUI_Texture*>(BinaryObject[n])->Get_StartUV();
                            m_EndUV = dynamic_cast<CUI_Texture*>(BinaryObject[n])->Get_EndUV();
                            m_UpPoint = dynamic_cast<CUI_Texture*>(BinaryObject[n])->Get_UpPoint();
                            m_DownPoint = dynamic_cast<CUI_Texture*>(BinaryObject[n])->Get_DownPoint();
                        }
                        if (CObject_Manager::BTN == BinaryObject[m_iBinaryObjectIndex]->Get_TypeIndex())
                        {
                            m_ClickStartUV= dynamic_cast<CBtn*>(BinaryObject[n])->Get_ClickStartUV();
                            m_ClickEndUV = dynamic_cast<CBtn*>(BinaryObject[n])->Get_ClickEndUV();
                            m_ClickColor = dynamic_cast<CBtn*>(BinaryObject[n])->Get_ClickColor();
                        }
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }

            if (ImGui::Button("Remove"))
            {
                if(-1!= m_iBinaryObjectIndex)
                {
                    if (BinaryObject[m_iBinaryObjectIndex]->Get_TypeIndex() == CObject_Manager::GROUP)
                        m_isOpenBinaryGroup = false;

                    if (FAILED(m_pObjectManager->Remove_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex)))
                    {
                        MSG_BOX("바이너리 오브젝트 삭제 불가");
                    }

                    auto BinaryObject = m_pObjectManager->Get_GroupBinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag));


                        m_iBinaryObjectIndex = -1;
                    
                }
            }
            if (ImGui::Button(u8"앞으로"))
            {
                if (FAILED(m_pObjectManager->Move_BinaryObjectIndex(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex, CObject_Manager::UP)))
                    MSG_BOX("이동 실패");
                else
                    m_iBinaryObjectIndex++;
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"뒤로"))
            {
                if (FAILED(m_pObjectManager->Move_BinaryObjectIndex(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex, CObject_Manager::DOWN)))
                    MSG_BOX("이동 실패");
                else
                    m_iBinaryObjectIndex--;
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"복사"))
            {
                if (FAILED(m_pObjectManager->Copy_BinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex)))
                    MSG_BOX("이미지 생성 실패");
            }

            BinaryObject = m_pObjectManager->Get_GroupBinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag));

            //생성된 바이너리에 따라 수정 옵션 추가 
            if (m_iBinaryObjectIndex != -1 && m_iBinaryPick == BINARY_OBJECT)
            {

                ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");

                if (ImGui::IsKeyPressed(ImGuiKey_W))
                    mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
                if (ImGui::IsKeyPressed(ImGuiKey_E))
                    mCurrentGizmoOperation = ImGuizmo::ROTATE;
                if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
                    mCurrentGizmoOperation = ImGuizmo::SCALE;

                if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
                    mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
                ImGui::SameLine();
                if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
                    mCurrentGizmoOperation = ImGuizmo::ROTATE;
                ImGui::SameLine();
                if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
                    mCurrentGizmoOperation = ImGuizmo::SCALE;

                float matrixTranslation[3], matrixRotation[3], matrixScale[3];
                ImGuizmo::DecomposeMatrixToComponents((_float*)&m_WorldMatirx, matrixTranslation, matrixRotation, matrixScale);
                ImGui::InputFloat3("Tr", matrixTranslation);
                ImGui::InputFloat3("Rt", matrixRotation);
                ImGui::InputFloat3("Sc", matrixScale);
                ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (_float*)&m_WorldMatirx);

                BinaryObject[m_iBinaryObjectIndex]->Get_TransformCom()->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatirx));

                _bool isEvent = dynamic_cast<CUI_Object*>(BinaryObject[m_iBinaryObjectIndex])->Get_Event();
                if (ImGui::Checkbox("isEvent", &isEvent))
                    dynamic_cast<CUI_Object*>(BinaryObject[m_iBinaryObjectIndex])->Set_Event(isEvent);

               static char szObName[MAX_PATH] ;
                ImGui::InputText(u8"이름 변경 ", szObName, MAX_PATH);
                if (ImGui::Button(u8"변경하기"))
                {
                    string Name = szObName;
                    dynamic_cast<CUI_Object*>(BinaryObject[m_iBinaryObjectIndex])->Set_Name(Name);
                }
                //쉐이더 선택
                if (!m_isOpenBinaryGroup)
                {


                    if (CObject_Manager::EFFECT != dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_TypeIndex())
                    {
                        _bool isAnim = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_isAnim();
                        if (ImGui::Checkbox("isAnim", &isAnim))
                            dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_isAnim(isAnim);
                        ImGui::SameLine();

                        _bool isColor = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_isColor();
                        if (ImGui::Checkbox("isColor", &isColor))
                            dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_isColor(isColor);
                        ImGui::SameLine();

                        _bool isScreen = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_isScreen();
                        if (ImGui::Checkbox("isScreen", &isScreen))
                            dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_isScreen(isScreen);
                        ImGui::SameLine();
                        _bool isLoop = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_isAnimLoop();
                        if (ImGui::Checkbox("isLoop", &isLoop))
                            dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_isAnimLoop(isLoop);



                        if (!isAnim && !isColor && !isScreen)
                        {
                            dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(1);
                        }
                        else if (isAnim && !isColor && !isScreen)
                        {
                            dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(4);
                        }
                        else if (!isAnim && isColor && !isScreen)
                        {
                            dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(2);
                        }
                        else if (isAnim && isColor && !isScreen)
                        {
                            dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(5);
                        }
                        else if (isAnim && isColor && isScreen)
                        {
                            dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(6);
                        }
                        else if (!isAnim && isColor && isScreen)
                        {
                            dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(7);
                        }
                        else if (!isAnim && isColor && isScreen)
                        {
                            dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(7);
                        }


                        if (isAnim)
                        {

                            _bool isPlay = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_isPlay();
                            if (ImGui::Checkbox("isPlay", &isPlay))
                                dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_isPlay(isPlay);

                            ImGui::SameLine();
                            _bool isReverse = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_isReverse();
                            if (ImGui::Checkbox("isReverse", &isReverse))
                                dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_isReverse(isReverse);

                            _float2 fAnimTime = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_AnimTime();
                            if (ImGui::DragFloat2("AnimTime", (float*)&fAnimTime, 0.001f, 0.0f, 5.f))
                                dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_AnimTime(fAnimTime);

                            _float3 vStartPos = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_StartPos();
                            if (ImGui::DragFloat3("StartPos", (float*)&vStartPos, 0.001f))
                                dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_StartPos(vStartPos);

                            _float2 vAnimScale = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_AnimScale();
                            if (ImGui::DragFloat2("AnimScale", (float*)&vAnimScale, 0.001f))
                                dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_AnimScale(vAnimScale);


                            _float2 fControlAlpha = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_ControlAlpha();
                            if (ImGui::DragFloat2("ControlAlpha", (float*)&fControlAlpha, 0.001f, 0.0f, 1.0f))
                                dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ControlAlpha(fControlAlpha);

                        }


                        if(isColor)
                        {
                            if (dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_isColor())
                            {
                                _float4 vColor = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_Color();
                                static bool alpha_preview = true;
                                static bool alpha_half_preview = false;
                                static bool drag_and_drop = true;
                                static bool options_menu = true;
                                static bool hdr = false;
                                ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

                                if (ImGui::ColorEdit4("MyColor##2f", (float*)&vColor, ImGuiColorEditFlags_Float | misc_flags))
                                {
                                    dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_Color(vColor);
                                }
                            }
                            if (dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_isColor())
                            {
                                _float4 vColor = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_EndColor();
                                static bool alpha_preview = true;
                                static bool alpha_half_preview = false;
                                static bool drag_and_drop = true;
                                static bool options_menu = true;
                                static bool hdr = false;
                                ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

                                if (ImGui::ColorEdit4("MyEndColor##2f", (float*)&vColor, ImGuiColorEditFlags_Float | misc_flags))
                                {
                                    dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_EndColor(vColor);
                                }
                            }
                        }


                        if (CObject_Manager::TEXT != dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_TypeIndex())
                        {
                            _float4 vUpPoint = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_UpPoint();
                            if (ImGui::DragFloat4("UpPoint", (float*)&vUpPoint, 0.001f, -10.f, 10.f))
                            {
                                m_UpPoint = vUpPoint;
                                if (FAILED(dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Change_Point(m_UpPoint, m_DownPoint)))
                                    MSG_BOX("Point 변경 실패");
                            }
                            _float4 vDownPoint = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_DownPoint();
                            if (ImGui::DragFloat4("DownPoint", (float*)&vDownPoint, 0.001f, -10.f, 10.f))
                            {
                                m_DownPoint = vDownPoint;
                                if (FAILED(dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Change_Point(m_UpPoint, m_DownPoint)))
                                    MSG_BOX("Point 변경 실패");
                            }
                            ImGui::DragFloat2("StartUV", (float*)&m_StartUV, 0.001f, 0.0f, 1.f);
                            ImGui::DragFloat2("EndUV", (float*)&m_EndUV, 0.001f, 0.0f, 1.f);

                            if (FAILED(dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Change_UV(m_StartUV, m_EndUV)))
                                MSG_BOX("UV 변경 실패");
                        }
                        else
                        {
                            _int Align = dynamic_cast<CText*>(BinaryObject[m_iBinaryObjectIndex])->Get_Align();    
                            if (ImGui::RadioButton(u8"가운데", &Align, 0))
                                dynamic_cast<CText*>(BinaryObject[m_iBinaryObjectIndex])->Set_Align(static_cast<_uint>(Align));
                            ImGui::SameLine();
                            if(ImGui::RadioButton(u8"우측", &Align, 1))
                                dynamic_cast<CText*>(BinaryObject[m_iBinaryObjectIndex])->Set_Align(static_cast<_uint>(Align));
                            ImGui::SameLine();
                            if(ImGui::RadioButton(u8"좌측", &Align, 2))
                                dynamic_cast<CText*>(BinaryObject[m_iBinaryObjectIndex])->Set_Align(static_cast<_uint>(Align));

                            for (size_t i = 0; i < m_Font.size(); i++)
                            {
                                if (m_Font[i] == dynamic_cast<CText*>(BinaryObject[m_iBinaryObjectIndex])->Get_Font())
                                {
                                    m_FontIndex = i;
                                    break;
                                }
                            }

                            static char szObText[MAX_PATH];
                            ImGui::InputText(u8"내용 변경 ", szObText, MAX_PATH);
                            if (ImGui::Button(u8"내용변경하기"))
                            {
                                string Text = szObText;
                                dynamic_cast<CText*>(BinaryObject[m_iBinaryObjectIndex])->Set_Text(m_pGameInstance->StringToWstring(Text));
                            }

                            if (ImGui::BeginListBox("Font"))
                            {
                                for (int n = 0; n < m_Font.size(); n++)
                                {
                                    const bool is_selected = (m_FontIndex == n);
                                    string strName = m_pGameInstance->WstringToString(m_Font[n]);

                                    if (ImGui::Selectable(strName.c_str(), is_selected))
                                    {
                                        m_FontIndex = n;
                                        dynamic_cast<CText*>(BinaryObject[m_iBinaryObjectIndex])->Set_Font(m_Font[m_FontIndex]);
                                    }
                                    if (is_selected)
                                        ImGui::SetItemDefaultFocus();
                                }
                                ImGui::EndListBox();

                            }
                        }

                        if (CObject_Manager::BTN == dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_TypeIndex())
                        {
                            if (dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_isColor())
                            {
                                _float4 vColor = dynamic_cast<CBtn*>(BinaryObject[m_iBinaryObjectIndex])->Get_ClickColor(); 
                                static bool alpha_preview = true;
                                static bool alpha_half_preview = false;
                                static bool drag_and_drop = true;
                                static bool options_menu = true;
                                static bool hdr = false;
                                ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

                                if (ImGui::ColorEdit4("MyColor##3f", (float*)&vColor, ImGuiColorEditFlags_Float | misc_flags))
                                {
                                    dynamic_cast<CBtn*>(BinaryObject[m_iBinaryObjectIndex])->Set_ClickColor(vColor);    
                                }
                            }

                            ImGui::DragFloat2("ClickStartUV", (float*)&m_ClickStartUV, 0.001f, 0.0f, 1.f);
                            ImGui::DragFloat2("ClickEndUV", (float*)&m_ClickEndUV, 0.001f, 0.0f, 1.f);


                            if (FAILED(dynamic_cast<CBtn*>(BinaryObject[m_iBinaryObjectIndex])->Chage_ClickUV(m_ClickStartUV, m_ClickEndUV)))
                                MSG_BOX("ClickUV 변경 실패");

                            _bool Click = dynamic_cast<CBtn*>(BinaryObject[m_iBinaryObjectIndex])->Get_Click();
                            if (ImGui::Checkbox("Click", &Click))
                            {
                                dynamic_cast<CBtn*>(BinaryObject[m_iBinaryObjectIndex])->Set_Click(Click);
                            }



                        }

                    }
                    else
                    {
                        dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_ShaderPass(3);

                        _float4 vColor = dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Get_Color();
                        static bool alpha_preview = true;
                        static bool alpha_half_preview = false;
                        static bool drag_and_drop = true;
                        static bool options_menu = true;
                        static bool hdr = false;
                        ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

                        if (ImGui::ColorEdit4("MyColor##2f", (float*)&vColor, ImGuiColorEditFlags_Float | misc_flags))
                        {
                            dynamic_cast<CUI_Texture*>(BinaryObject[m_iBinaryObjectIndex])->Set_Color(vColor);
                        }
                      

                      _float3 vLifeTime = dynamic_cast<CUI_Effect*>(BinaryObject[m_iBinaryObjectIndex])->Get_LifeTime();
                      _float fLife[2] = { vLifeTime.y, vLifeTime.z };
                      if (ImGui::DragFloat2("LifeTime", fLife, 0.001f, 0.0f, 10.f))
                      {
                          vLifeTime.y = fLife[0];
                          vLifeTime.z = fLife[1];
                          dynamic_cast<CUI_Effect*>(BinaryObject[m_iBinaryObjectIndex])->Set_LifeTime(vLifeTime);
                      }

                      _float fSpeed = dynamic_cast<CUI_Effect*>(BinaryObject[m_iBinaryObjectIndex])->Get_Speed();
                      if (ImGui::DragFloat("LifeSpeed", &fSpeed, 0.001f, 0.0f, 10.f))
                      {
                          dynamic_cast<CUI_Effect*>(BinaryObject[m_iBinaryObjectIndex])->Set_Speed(fSpeed);
                      }
                        

                    }



                }
            }
        }
        else m_isOpenBinaryGroup = false;
    
    
         ImGui::End();
}

void CImgui_Manager::Window_Binary_Group()
{
    if (!m_isOpenBinaryGroup) return;

    ImGui::Begin(u8"바이너리 오브젝트 그룹들");

    //생성
    ImGui::Checkbox(u8"오브젝트 생성", &m_isCreateBinaryGroupObject);

    if (m_isCreateBinaryGroupObject)
    {
        auto BinaryObject = m_pObjectManager->Get_GroupBinaryObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag));
        //생성 가능하다면
        ImGui::SeparatorText(u8"오브젝트 생성");
        ImGui::RadioButton(u8"이미지", &m_iBinaryGroupObjectType, CObject_Manager::IMG); ImGui::SameLine();
        ImGui::RadioButton(u8"버튼", &m_iBinaryGroupObjectType, CObject_Manager::BTN); ImGui::SameLine();
        ImGui::RadioButton(u8"텍스트", &m_iBinaryGroupObjectType, CObject_Manager::TEXT); ImGui::SameLine();
        ImGui::RadioButton(u8"UI 이펙트", &m_iBinaryGroupObjectType, CObject_Manager::EFFECT); ImGui::SameLine();
        ImGui::RadioButton(u8"HEADUI", &m_iBinaryGroupObjectType, CObject_Manager::HEADUI);

        ImGui::NewLine();
        ImGui::InputText(u8"저장할 오브젝트 이름 : ", m_szBinaryGroupObjectName, MAX_PATH);

        //오브젝트 생성
        if (m_iBinaryGroupObjectType == CObject_Manager::IMG || m_iBinaryGroupObjectType == CObject_Manager::HEADUI)
        {
            //일반 이미지
            if (ImGui::Button(u8"이미지 파일 생성"))
            {
                IGFD::FileDialogConfig config;
                config.path = (m_RootDir / "Client" / "Bin" / "Resources" / "Textures" / "UI").string();
                ImGuiFileDialog::Instance()->OpenDialog("ChooseBinaryGroupTextureKey", "Choose File", ".dds, .png", config);
            }
            if (ImGuiFileDialog::Instance()->Display("ChooseBinaryGroupTextureKey")) {
                if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                    if ('\0' != m_szBinaryGroupObjectName[0])
                    {
                        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                        std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                        filesystem::path filePath = filePathName;
                        filesystem::path relativePath = filePath.lexically_relative(m_RootDir);
                        // action

                        CImage_Texture::UI_TEXTURE_DESC Desc{};
                        Desc.strTextureFileName = m_pGameInstance->StringToWstring(fileName);
                        Desc.strTextureFilePath = m_pGameInstance->StringToWstring("../../" + relativePath.string());
                        Desc.iTypeIndex = CObject_Manager::IMG;
                        Desc.strName = m_szBinaryGroupObjectName;
                        Desc.isParent = true;
                        Desc.pParentMatrix = BinaryObject[m_iBinaryObjectIndex]->Get_TransformCom()->Get_WorldFloat4x4();
                        Desc.strParentName = BinaryObject[m_iBinaryObjectIndex]->Get_Name();

                        CImage_Texture* pImage = dynamic_cast<CImage_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &Desc));
                        dynamic_cast<CGroup*>(BinaryObject[m_iBinaryObjectIndex])->Set_PartObject(pImage);
                    }
                    else
                        MSG_BOX("이름작성");
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }
        }
        else if (m_iBinaryGroupObjectType == CObject_Manager::BTN)
        {
            //버튼 생성
            if (ImGui::Button(u8"이미지 파일"))
            {
                IGFD::FileDialogConfig config;
                config.path = (m_RootDir / "Client" / "Bin" / "Resources" / "Textures" / "UI").string();
                ImGuiFileDialog::Instance()->OpenDialog("ChooseBtnGroupTextureKey", "Choose File", ".dds, .png", config);
            }
            if (ImGuiFileDialog::Instance()->Display("ChooseBtnGroupTextureKey")) {
                if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK

                    filesystem::path filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    filesystem::path filePath = filePathName;
                    filesystem::path relativePath = filePath.lexically_relative(m_RootDir);
                    m_strBtnFilePath = "../../" + relativePath.string();
                    m_strBtnFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }

            if (ImGui::Button(u8"클릭 이미지 파일"))
            {
                IGFD::FileDialogConfig config;
                config.path = (m_RootDir / "Client" / "Bin" / "Resources" / "Textures" / "UI").string();
                ImGuiFileDialog::Instance()->OpenDialog("ChooseBtnGroupClickTextureKey", "Choose File", ".dds, .png", config);
            }
            if (ImGuiFileDialog::Instance()->Display("ChooseBtnGroupClickTextureKey")) {
                if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                    filesystem::path filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    filesystem::path filePath = filePathName;
                    filesystem::path relativePath = filePath.lexically_relative(m_RootDir);
                    m_strBtnClickFilePath = "../../" + relativePath.string();
                    m_strBtnClickFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }

            if (ImGui::Button(u8"버튼 생성"))
            {
                if ('\0' != m_szBinaryGroupObjectName[0])
                {
                    CBtn::BTN_DESC Desc{};
                    Desc.strTextureFilePath = m_pGameInstance->StringToWstring(m_strBtnFilePath);
                    Desc.strTextureFileName = m_pGameInstance->StringToWstring(m_strBtnFileName);
                    Desc.strClickFilePath = m_pGameInstance->StringToWstring(m_strBtnClickFilePath);
                    Desc.strClickFileName = m_pGameInstance->StringToWstring(m_strBtnClickFileName);
                    Desc.strName = m_szBinaryGroupObjectName;
                    Desc.iTypeIndex = m_iBinaryGroupObjectType;
                    Desc.isParent = true;
                    Desc.pParentMatrix = BinaryObject[m_iBinaryObjectIndex]->Get_TransformCom()->Get_WorldFloat4x4();
                    Desc.strParentName = BinaryObject[m_iBinaryObjectIndex]->Get_Name();
                    CBtn* pBtn = dynamic_cast<CBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Btn"), &Desc));
                    if (nullptr == pBtn)
                        MSG_BOX("버튼 생성 실패");

                    dynamic_cast<CGroup*>(BinaryObject[m_iBinaryObjectIndex])->Set_PartObject(pBtn);
                }
                else
                    MSG_BOX("이름작성");
            }
        }
        else if (m_iBinaryGroupObjectType == CObject_Manager::TEXT)
        {
            //텍스트 생성
            ImGui::InputText(u8"텍스트 작성 : ", m_szText, MAX_PATH);

            //static ImVec4 vAmbientColor = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
            //ImGui::ColorEdit4(u8"vColor 선택", (float*)&m_vColor, ImGuiColorEditFlags_Float);

            if (ImGui::Button(u8"텍스트 생성"))
            {
                if ('\0' != m_szBinaryGroupObjectName[0])
                {
                    //텍스쳐 생성
                    CText::TEXT_DESC Desc{};
                    Desc.strText = m_pGameInstance->StringToWstring(string(m_szText));
                    Desc.iTypeIndex = m_iBinaryGroupObjectType;
                    Desc.vColor = m_vColor;
                    Desc.strName = m_szBinaryGroupObjectName;
                    Desc.isParent = true;
                    Desc.pParentMatrix = BinaryObject[m_iBinaryObjectIndex]->Get_TransformCom()->Get_WorldFloat4x4();
                    Desc.strParentName = BinaryObject[m_iBinaryObjectIndex]->Get_Name();
                    CText* pText = dynamic_cast<CText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Text"), &Desc));
                    dynamic_cast<CGroup*>(BinaryObject[m_iBinaryObjectIndex])->Set_PartObject(pText);
                }
                else
                    MSG_BOX("이름작성");

            }
        }
        else if (m_iBinaryGroupObjectType == CObject_Manager::EFFECT)
        {
            //이펙트 생성
            if (ImGui::Button(u8"이펙트 파일 생성"))
            {
                IGFD::FileDialogConfig config;
                config.path = (m_RootDir / "Client" / "Bin" / "Resources" / "Textures" / "UI").string();
                ImGuiFileDialog::Instance()->OpenDialog("ChooseBinaryGroupEffectTextureKey", "Choose File", ".dds, .png", config);
            }
            if (ImGuiFileDialog::Instance()->Display("ChooseBinaryGroupEffectTextureKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                { // action if OK
                    if ('\0' != m_szBinaryGroupObjectName[0])
                    {
                        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                        std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                        // action
                        filesystem::path filePath = filePathName;
                        filesystem::path relativePath = filePath.lexically_relative(m_RootDir);


                        CUI_Effect::UI_EFFECT_DESC Desc{};

                        Desc.strTextureFileName = m_pGameInstance->StringToWstring(fileName);
                        Desc.strTextureFilePath = m_pGameInstance->StringToWstring("../../" + relativePath.string());
                        Desc.iTypeIndex = m_iBinaryGroupObjectType;
                        Desc.strName = m_szBinaryGroupObjectName;
                        Desc.isParent = true;
                        Desc.pParentMatrix = BinaryObject[m_iBinaryObjectIndex]->Get_TransformCom()->Get_WorldFloat4x4();
                        Desc.strParentName = BinaryObject[m_iBinaryObjectIndex]->Get_Name();
                        CUI_Effect* pEffect = dynamic_cast<CUI_Effect*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIEffect"), &Desc));
                        dynamic_cast<CGroup*>(BinaryObject[m_iBinaryObjectIndex])->Set_PartObject(pEffect);
                    }
                    else
                        MSG_BOX("이름작성");
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }
        }
    }

    //생성된 오브젝트 확인 가능
    vector<CUI_Object*> Objects = m_pObjectManager->Get_BinaryGroupObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex);
    if (ImGui::BeginListBox("BinaryGroupObjectList"))
    {
         for (int n = 0; n < Objects.size(); n++)
         {
             const bool is_selected = (m_iBinaryGroupObjectIndex == n);
             string strName = Objects[n]->Get_Name();
             if (ImGui::Selectable(strName.c_str(), is_selected))
             {
                 m_iBinaryGroupObjectIndex = n;
                 m_WorldMatirx = *Objects[n]->Get_TransformCom()->Get_WorldFloat4x4();
                 m_StartUV = dynamic_cast<CUI_Texture*>(Objects[n])->Get_StartUV();
                 m_EndUV = dynamic_cast<CUI_Texture*>(Objects[n])->Get_EndUV();
                 m_UpPoint = dynamic_cast<CUI_Texture*>(Objects[n])->Get_UpPoint();
                 m_DownPoint = dynamic_cast<CUI_Texture*>(Objects[n])->Get_DownPoint();
                 if (9 == dynamic_cast<CUI_Texture*>(Objects[n])->Get_ShaderPass())
                 {
                     m_isCircle = true;
                 }


                 if (CObject_Manager::BTN == Objects[n]->Get_TypeIndex())
                 {
                     m_ClickStartUV = dynamic_cast<CBtn*>(Objects[n])->Get_ClickStartUV();
                     m_ClickEndUV = dynamic_cast<CBtn*>(Objects[n])->Get_ClickEndUV();
                 }
                 m_iBinaryPick = GROUP_OBJECT;
             }
             if (is_selected)
                 ImGui::SetItemDefaultFocus();
         }
         ImGui::EndListBox();
    }



    if (ImGui::Button("Group Object Remove"))
    {
        if(-1!= m_iBinaryGroupObjectIndex)
        {
            m_pObjectManager->Remove_GroupObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex, m_iBinaryGroupObjectIndex);
            m_iBinaryGroupObjectIndex = -1; // 초기화 시키기S
        }
    }
    ImGui::SameLine();

    if (ImGui::Button(u8"앞으로"))
    {
        if (FAILED(m_pObjectManager->Move_BinaryGroupObjectIndex(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex, m_iBinaryGroupObjectIndex, CObject_Manager::UP)))
            MSG_BOX("이동 실패");
        else
            m_iBinaryGroupObjectIndex++;
    }
    ImGui::SameLine();
    if (ImGui::Button(u8"뒤로"))
    {
        if (FAILED(m_pObjectManager->Move_BinaryGroupObjectIndex(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex, m_iBinaryGroupObjectIndex, CObject_Manager::DOWN)))
            MSG_BOX("이동 실패");
        else
            m_iBinaryGroupObjectIndex--;
    }
    ImGui::SameLine();
    if (ImGui::Button(u8"복사"))
    {
        if (FAILED(m_pObjectManager->Copy_BinaryGroupObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex, m_iBinaryGroupObjectIndex)))
            MSG_BOX("이미지 생성 실패");
    }

  Objects = m_pObjectManager->Get_BinaryGroupObject(m_pGameInstance->StringToWstring(m_strBinarySelectTag), m_iBinaryObjectIndex);

    if (m_iBinaryGroupObjectIndex != -1 && m_iBinaryPick == GROUP_OBJECT)
    {
        //바이너리 선택했을때
        ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");

        if (ImGui::IsKeyPressed(ImGuiKey_W))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;   
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
            mCurrentGizmoOperation = ImGuizmo::SCALE;   

        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;

        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents((_float*)&m_WorldMatirx, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (_float*)&m_WorldMatirx);

        Objects[m_iBinaryGroupObjectIndex]->Get_TransformCom()->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatirx));
        dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_WorldMatrix(m_WorldMatirx);

        static char szGroupObName[MAX_PATH];
        ImGui::InputText(u8"이름 변경 ", szGroupObName, MAX_PATH);
        if (ImGui::Button(u8"변경하기"))
        {
            string Name = szGroupObName;
            dynamic_cast<CUI_Object*>(Objects[m_iBinaryGroupObjectIndex])->Set_Name(Name);
        }

        if(CObject_Manager::EFFECT != dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_TypeIndex())
        {
            _bool isAnim = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_isAnim();
            if (ImGui::Checkbox("isAnim", &isAnim))
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_isAnim(isAnim);
            ImGui::SameLine();

            _bool isColor = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_isColor();
            if (ImGui::Checkbox("isColor", &isColor))
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_isColor(isColor);
            ImGui::SameLine();
            _bool isScreen = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_isScreen();
            if (ImGui::Checkbox("isScreen", &isScreen))
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_isScreen(isScreen);
            ImGui::SameLine();
            _bool isLoop = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_isAnimLoop();
            if (ImGui::Checkbox("isLoop", &isLoop))
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_isAnimLoop(isLoop);

            if (!isAnim && !isColor && !isScreen)
            {
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(1);
            }
            else if (isAnim && !isColor && !isScreen)
            {
                if(true == m_isCircle)
                    dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(9);
                else
                    dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(4);

            }
            else if (!isAnim && isColor && !isScreen)
            {
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(2);
            }
            else  if (isAnim && isColor &&!isScreen)
            {
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(5);
            }
            else if (isAnim && isColor && isScreen)
            {
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(6);
            }
            else if (!isAnim && isColor && isScreen)
            {
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(7);
            }



            if (isAnim)
            {

                _bool isPlay = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_isPlay();
                if (ImGui::Checkbox("isPlay", &isPlay))
                    dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_isPlay(isPlay);

                ImGui::SameLine();
                _bool isReverse = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_isReverse();
                if (ImGui::Checkbox("isReverse", &isReverse))
                    dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_isReverse(isReverse);

                ImGui::SameLine();
                if (9 == dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_ShaderPass())
                    m_isCircle = true;
                else
                    m_isCircle = false;

                if (ImGui::Checkbox("isCircle", &m_isCircle))
                    dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(9);


                _float2 fAnimTime = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_AnimTime();
                if (ImGui::DragFloat2("AnimTime", (float*)&fAnimTime, 0.001f, 0.0f, 5.f))
                    dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_AnimTime(fAnimTime);

                _float3 vStartPos = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_StartPos();
                if (ImGui::DragFloat3("StartPos", (float*)&vStartPos, 0.001f))
                    dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_StartPos(vStartPos);

                _float2 vAnimScale = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_AnimScale();
                if (ImGui::DragFloat2("AnimScale", (float*)&vAnimScale, 0.001f))
                    dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_AnimScale(vAnimScale);


                _float2 fControlAlpha = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_ControlAlpha();
                if (ImGui::DragFloat2("ControlAlpha", (float*)&fControlAlpha, 0.001f, 0.0f, 1.0f))
                    dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ControlAlpha(fControlAlpha);



            }

            if(isColor)
            {
                if (dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_isColor())
                {
                    _float4 vColor = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_Color();
                    static bool alpha_preview = true;
                    static bool alpha_half_preview = false;
                    static bool drag_and_drop = true;
                    static bool options_menu = true;
                    static bool hdr = false;
                    ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

                    if (ImGui::ColorEdit4("MyColor##2f", (float*)&vColor, ImGuiColorEditFlags_Float | misc_flags))
                    {
                        dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_Color(vColor);
                    }
                }

                if (dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_isColor())
                {
                    _float4 vColor = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_EndColor();
                    static bool alpha_preview = true;
                    static bool alpha_half_preview = false;
                    static bool drag_and_drop = true;
                    static bool options_menu = true;
                    static bool hdr = false;
                    ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

                    if (ImGui::ColorEdit4("MyEndColor##2f", (float*)&vColor, ImGuiColorEditFlags_Float | misc_flags))
                    {
                        dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_EndColor(vColor);
                    }
                }
            }


            if (CObject_Manager::TEXT != dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_TypeIndex())
            {

                _float4 vUpPoint = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_UpPoint();
                if (ImGui::DragFloat4("UpPoint", (float*)&vUpPoint, 0.001f, -10.f, 10.f))
                {
                    m_UpPoint = vUpPoint;
                    if (FAILED(dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Change_Point(m_UpPoint, m_DownPoint)))
                        MSG_BOX("Point 변경 실패");
                }
                _float4 vDownPoint = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_DownPoint();
                if (ImGui::DragFloat4("DownPoint", (float*)&vDownPoint, 0.001f, -10.f, 10.f))
                {
                    m_DownPoint = vDownPoint;
                    if (FAILED(dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Change_Point(m_UpPoint, m_DownPoint)))
                        MSG_BOX("Point 변경 실패");
                }

                ImGui::DragFloat2("StartUV", (float*)&m_StartUV, 0.001f, 0.0f, 1.f);
                ImGui::DragFloat2("EndUV", (float*)&m_EndUV, 0.001f, 0.0f, 1.f);

                if (FAILED(dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Change_UV(m_StartUV, m_EndUV)))
                    MSG_BOX("UV 변경 실패");
            }
            else
            {
                _int Align = dynamic_cast<CText*>(Objects[m_iBinaryGroupObjectIndex])->Get_Align();
                if (ImGui::RadioButton(u8"가운데", &Align, 0))
                    dynamic_cast<CText*>(Objects[m_iBinaryGroupObjectIndex])->Set_Align(static_cast<_uint>(Align));
                ImGui::SameLine();
                if (ImGui::RadioButton(u8"우측", &Align, 1))
                    dynamic_cast<CText*>(Objects[m_iBinaryGroupObjectIndex])->Set_Align(static_cast<_uint>(Align));
                ImGui::SameLine();
                if (ImGui::RadioButton(u8"좌측", &Align, 2))
                    dynamic_cast<CText*>(Objects[m_iBinaryGroupObjectIndex])->Set_Align(static_cast<_uint>(Align));

                static char szGroupObText[MAX_PATH];
                ImGui::InputText(u8"내용 변경 ", szGroupObText, MAX_PATH);
                if (ImGui::Button(u8"내용변경하기"))
                {
                    string Text = szGroupObText;
                    dynamic_cast<CText*>(Objects[m_iBinaryGroupObjectIndex])->Set_Text(m_pGameInstance->StringToWstring(Text));
                }


                for (size_t i = 0; i < m_Font.size(); i++)
                {
                    if (m_Font[i] == dynamic_cast<CText*>(Objects[m_iBinaryGroupObjectIndex])->Get_Font())
                    {
                        m_FontIndex = i;
                        break;
                    }
                }


                if (ImGui::BeginListBox("Font"))
                {
                    for (int n = 0; n < m_Font.size(); n++)
                    {
                        const bool is_selected = (m_FontIndex == n);
                        string strName = m_pGameInstance->WstringToString(m_Font[n]);

                        if (ImGui::Selectable(strName.c_str(), is_selected))
                        {
                            m_FontIndex = n;
                            dynamic_cast<CText*>(Objects[m_iBinaryGroupObjectIndex])->Set_Font(m_Font[m_FontIndex]);
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndListBox();

                }
            }



            if (CObject_Manager::BTN == dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_TypeIndex())
            {
                ImGui::DragFloat2("ClickStartUV", (float*)&m_ClickStartUV, 0.001f, 0.0f, 1.f);
                ImGui::DragFloat2("ClickEndUV", (float*)&m_ClickEndUV, 0.001f, 0.0f, 1.f);

                if (FAILED(dynamic_cast<CBtn*>(Objects[m_iBinaryGroupObjectIndex])->Chage_ClickUV(m_ClickStartUV, m_ClickEndUV)))
                    MSG_BOX("ClickUV 변경 실패");

                _bool Click = dynamic_cast<CBtn*>(Objects[m_iBinaryGroupObjectIndex])->Get_Click();
                if (ImGui::Checkbox("Click", &Click))
                {
                    dynamic_cast<CBtn*>(Objects[m_iBinaryGroupObjectIndex])->Set_Click(Click);
                }

            }


        }
        else
        {
            dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ShaderPass(3);


            _bool isPlay = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_isPlay();
            if (ImGui::Checkbox("isPlay", &isPlay))
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_isPlay(isPlay);
            ImGui::SameLine();
            _bool isLoop = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_isAnimLoop();
            if (ImGui::Checkbox("isLoop", &isLoop))
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_isAnimLoop(isLoop);

            _float2 fControlAlpha = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_ControlAlpha();
            if (ImGui::DragFloat2("ControlAlpha", (float*)&fControlAlpha, 0.001f, 0.0f, 1.0f))
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_ControlAlpha(fControlAlpha);

            _float3 vStartPos = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_StartPos();
            if (ImGui::DragFloat3("StartPos", (float*)&vStartPos, 0.001f))
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_StartPos(vStartPos);

            _float2 vAnimScale = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_AnimScale();
            if (ImGui::DragFloat2("AnimScale", (float*)&vAnimScale, 0.001f))
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_AnimScale(vAnimScale);

            _float4 vColor = dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Get_Color();
            static bool alpha_preview = true;
            static bool alpha_half_preview = false;
            static bool drag_and_drop = true;
            static bool options_menu = true;
            static bool hdr = false;
            ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

            if (ImGui::ColorEdit4("MyColor##2f", (float*)&vColor, ImGuiColorEditFlags_Float | misc_flags))
            {
                dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Set_Color(vColor);
            }
            

            _float3 vLifeTime = dynamic_cast<CUI_Effect*>(Objects[m_iBinaryGroupObjectIndex])->Get_LifeTime();

            if (ImGui::DragFloat3("LifeTime", (_float*) & vLifeTime, 0.001f, 0.0f, 10.f))
            {
                dynamic_cast<CUI_Effect*>(Objects[m_iBinaryGroupObjectIndex])->Set_LifeTime(vLifeTime);
            }

            _float fSpeed = dynamic_cast<CUI_Effect*>(Objects[m_iBinaryGroupObjectIndex])->Get_Speed();
            if (ImGui::DragFloat("LifeSpeed", &fSpeed, 0.001f, 0.0f, 10.f))
            {
                dynamic_cast<CUI_Effect*>(Objects[m_iBinaryGroupObjectIndex])->Set_Speed(fSpeed);
            }

            ImGui::DragFloat2("StartUV", (float*)&m_StartUV, 0.001f, 0.0f, 1.f);
            ImGui::DragFloat2("EndUV", (float*)&m_EndUV, 0.001f, 0.0f, 1.f);

            if (FAILED(dynamic_cast<CUI_Texture*>(Objects[m_iBinaryGroupObjectIndex])->Change_UV(m_StartUV, m_EndUV)))
                MSG_BOX("UV 변경 실패");

            _bool isUVAnim = dynamic_cast<CUI_Effect*>(Objects[m_iBinaryGroupObjectIndex])->Get_isUVAnim();
            if (ImGui::Checkbox("UVAnim", &isUVAnim))
                dynamic_cast<CUI_Effect*>(Objects[m_iBinaryGroupObjectIndex])->Set_isUVAnim(isUVAnim);

         }
}



    ImGui::End();
}

void CImgui_Manager::Guizmo_Test()
{
    ImGuizmo::BeginFrame();

    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    float viewManipulateRight = g_iWinSizeX;
    float viewManipulateTop = 0;

    ImGuizmo::SetRect(0, 0, g_iWinSizeX, g_iWinSizeY);

    _matrix cameraView = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
    _matrix viewInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW); // 카메라 월드 
    _matrix cameraProjection = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);
    _float viewMatrix[16], projMatrix[16], viewInverseMatrix[16];


    memcpy(&viewMatrix, &cameraView, sizeof(_matrix));
    memcpy(&viewInverseMatrix, &viewInverse, sizeof(_matrix));
    memcpy(&projMatrix, &cameraProjection, sizeof(_matrix));

   // ImGuizmo::DrawGrid(viewMatrix, projMatrix, (_float*)&identityMatrix, 100.f);

    if (m_iGroupObjectIndex != -1 || m_iBinaryObjectIndex != -1)
        ImGuizmo::Manipulate((_float*)&m_ViewMatrix, (_float*)&m_ProjMatrix, mCurrentGizmoOperation, mCurrentGizmoMode, (_float*)&m_WorldMatirx, NULL, useSnap ? &snap[0] : NULL);

    //memcpy(&cameraView, &viewInverseMatrix, sizeof(_float) * 16);
    //m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixInverse(nullptr, cameraView));
}

HRESULT CImgui_Manager::Create_Texture()
{
    //if (FAILED(m_pGameInstance->Copy_Resource(TEXT("Target_BackBuffer"), m_pTexture2D)))
    //    return E_FAIL;

    //if (FAILED(SaveDDSTextureToFile(m_pContext, m_pTexture2D, TEXT("../../Create/Bin/Resources/Textures/UI/CreateFile/test.dds"))))
    //    return E_FAIL;

    return S_OK;
}

void CImgui_Manager::Late_Tick(_float fTimeDelta)
{
}

void CImgui_Manager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

void CImgui_Manager::Release_Imgui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CImgui_Manager::GetInstance()->DestroyInstance();
}

void CImgui_Manager::Free()
{
    Safe_Release(m_pGameInstance);

    Safe_Release(m_pObjectManager);
    

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

}