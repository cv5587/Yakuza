#include "Model.h"
#include "GameInstance.h"

#include "Mesh.h"
#include "Channel.h"
#include "Bone.h"

#include "Texture.h"
#include "Shader.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CComponent{pDevice, pContext}
	, m_bOrigin{ true } /* 원형일때 1번만 지우기 */
{
}

CModel::CModel(const CModel& rhs)
    :CComponent{ rhs }
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_Materials{ rhs.m_Materials }
	, m_eModelType{ rhs.m_eModelType }
	, m_PreTransformMatrix{ rhs.m_PreTransformMatrix }
	, m_iAnimations{ rhs.m_iAnimations }
	, m_AnimDesc{ rhs.m_AnimDesc }
	, m_AnimLoops{ rhs.m_AnimLoops }
	, m_vDecalMaterials { rhs.m_vDecalMaterials }
	, m_iCameras{ rhs.m_iCameras }
	, m_FovAnimation{ rhs.m_FovAnimation }
	, m_isTool {rhs.m_isTool}
	, m_vModelLocalSize {rhs.m_vModelLocalSize}
	, m_vMaxPoints{ rhs.m_vMaxPoints }
	, m_vMinPoints{ rhs.m_vMinPoints }
{
	for (auto& pPrototypeAnimation : rhs.m_Animations)
		m_Animations.emplace_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.emplace_back(pPrototypeBone->Clone());

	m_iNumBones = m_Bones.size();

	for (auto& pMesh : rhs.m_Meshes)
		m_Meshes.emplace_back(pMesh->Clone());

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		for (auto& pTexture : m_Materials[i].pMaterialTextures)
			Safe_AddRef(pTexture);
	}
}

HRESULT CModel::Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, _bool isExported, _bool isTool)
{
	const char* blockword = "building";

	m_isTool = isTool;

	if (strstr(pModelFilePath, blockword) != nullptr) {
		m_bSaveMaterial = false;
	}

	m_eModelType = eModelType;

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	if (!isExported)
	{
		_uint		iFlag = aiProcess_ConvertToLeftHanded;

		if (TYPE_NONANIM == eModelType	||	TYPE_PARTICLE == eModelType)
			iFlag |= aiProcess_PreTransformVertices | aiProcess_GenNormals |  aiProcess_GenUVCoords| aiProcess_CalcTangentSpace;// aiProcessPreset_TargetRealtime_Fast;
		else
			iFlag |= aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_SortByPType;

		m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
		if (nullptr == m_pAIScene)
			return E_FAIL;

		// 메시를 만들기 전에 본을 먼저 세팅해둬야한다.
		if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
			return E_FAIL;

		if (FAILED(Ready_Meshes()))
			return E_FAIL;

		if (FAILED(Ready_Materials(pModelFilePath)))
			return E_FAIL;

		if (FAILED(Ready_Animations()))
			return E_FAIL;

		string pBinPath = m_pGameInstance->Get_Directory(pModelFilePath) + "/Bin/" + m_pGameInstance->Get_FileName(pModelFilePath) + ".dat";

		if (FAILED(Export_Model(pBinPath, pModelFilePath)))
			return E_FAIL;
	}
	else
	{

		string str = pModelFilePath;
		if (FAILED(Import_Model(str, isTool)))
			return E_FAIL;
	}

	if (0 < m_Meshes.size())
	{
		// model 자체의 크기 구함
		_float3		vMaxPosition = m_Meshes[0]->Get_MaxPoints();
		_float3		vMinPosition = m_Meshes[0]->Get_MinPoints();

		for (size_t i = 1; i < m_iNumMeshes; ++i)
		{
			vMinPosition.x = vMinPosition.x > m_Meshes[i]->Get_MinPoints().x ? m_Meshes[i]->Get_MinPoints().x : vMinPosition.x;
			vMinPosition.y = vMinPosition.y > m_Meshes[i]->Get_MinPoints().y ? m_Meshes[i]->Get_MinPoints().y : vMinPosition.y;
			vMinPosition.z = vMinPosition.z > m_Meshes[i]->Get_MinPoints().z ? m_Meshes[i]->Get_MinPoints().z : vMinPosition.z;

			vMaxPosition.x = vMaxPosition.x < m_Meshes[i]->Get_MaxPoints().x ? m_Meshes[i]->Get_MaxPoints().x : vMaxPosition.x;
			vMaxPosition.y = vMaxPosition.y < m_Meshes[i]->Get_MaxPoints().y ? m_Meshes[i]->Get_MaxPoints().y : vMaxPosition.y;
			vMaxPosition.z = vMaxPosition.z < m_Meshes[i]->Get_MaxPoints().z ? m_Meshes[i]->Get_MaxPoints().z : vMaxPosition.z;
		}

		m_vMaxPoints = vMaxPosition;
		m_vMinPoints = vMinPosition;
		m_vModelLocalSize = _float3(abs(vMaxPosition.x - vMinPosition.x), abs(vMaxPosition.y - vMinPosition.y), abs(vMaxPosition.z - vMinPosition.z));
	}

	if (TYPE_NONANIM != eModelType && TYPE_PARTICLE != eModelType)
	{
		string pCameraFovFilePath = "../Bin/DataFiles/CameraFoVAnimationData/" + m_pGameInstance->Get_FileName(pModelFilePath) + "_camera_fov.csv";

		Ready_CameraAnimations(pCameraFovFilePath);
	}

	m_AnimLoops.resize(m_iAnimations);
	fill(m_AnimLoops.begin(), m_AnimLoops.end(), false);			//m_AnimLoops를 일단 전부 false로 초기화한다.

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix), m_Bones, m_isTool);

		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MESH_MATERIAL		MeshMaterial{};
		
		if (true == m_bSaveMaterial)
		{
			string name = m_pAIScene->mMaterials[i]->GetName().C_Str();
			memcpy(MeshMaterial.strMaterialName, name.c_str(), sizeof(name));
		}
		
		for (size_t j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString pPath;
			

			if (FAILED(m_pAIScene->mMaterials[i]->GetTexture((aiTextureType)j, 0, &pPath)))
				continue;

			_char szFileName[MAX_PATH] = "";
			_char szExt[MAX_PATH] = "";

			_splitpath_s(pPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			if (strcmp(szFileName, "."))
				continue;

			_char		szDrive[MAX_PATH] = "";
			_char		szDirectory[MAX_PATH] = "";

 			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			_char		szFullPath[MAX_PATH] = "";

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szRealFullPath[MAX_PATH] = TEXT("");

			// WideChar로 형변환해주는 함수. code page를 아스키로 쓴다는 옵션 CP_ACP
			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szRealFullPath, MAX_PATH);


			MeshMaterial.pMaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szRealFullPath, 1);
			if (nullptr == MeshMaterial.pMaterialTextures[j])
				return E_FAIL;
		}
		m_Materials.emplace_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	//반복문을 빠져나온 이후일 경우에 부모 뼈는 항상 vector에 가장 마지막에 삽입된 뼈이기때문에
	// 부모의 뼈는 size - 1이 된다.
	_uint		iParent = m_Bones.size() - 1;

	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
	{
		Ready_Bones(pAINode->mChildren[i], iParent);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Ready_CameraAnimations(string& pBinFilePath)
{
	// 카메라가 없는 경우라면 그냥 함수 종료
	if (1 > m_iCameras) return S_OK;

	ifstream in(pBinFilePath, ios::binary);

	if (!in.is_open()) {
#ifdef _DEBUG
		MSG_BOX("Camera FoV 파일 개방 실패 (카메라 애니메이션이 없는 경우는 무시해도됨)");
#endif // _DEBUG
		return E_FAIL;
	}

	string line;
	// 헤더를 건너뜁니다.
	getline(in, line);

	while (getline(in, line)) {
		istringstream lineStream(line);
		string cell;

		FOV_ANIMATION Data{};

		// 애니메이션 이름
		getline(lineStream, cell, ',');
		Data.strAnimationName = cell;

		// 프레임 번호
		getline(lineStream, cell, ',');
		Data.iKeyFrameIndex = stoi(cell);

		// FoV 값
		getline(lineStream, cell, ',');
		Data.fFov = stof(cell);

		m_FovAnimation.push_back(Data);
	}
	
	return S_OK;
}

HRESULT CModel::Export_Model(string& pBinFilePath, const _char* pModelFilePath)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	string strBinDirectory = pGameInstance->Get_Directory(pBinFilePath);

	fs::path path(strBinDirectory);
	if(!exists(path))
		fs::create_directory(path);

	ofstream out(pBinFilePath, ios::binary);

	// 본 정보 저장하기.
	Export_Bones(out);

	// 메시 정보 저장하기
	Export_Meshes(out, pBinFilePath);

	// 메테리얼 정보 저장하기
	Export_Materials(out, pModelFilePath, pBinFilePath);

	// 애니메이션 정보 저장하기
	Export_Animations(out);

	// 카메라 갯수 저장하기
	Export_CamNums(out);
	
	out.close();

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CModel::Export_Bones(ofstream& out)
{
	// ReadyBones에서 저장하는 데이터들만 저장한다.
	_int iNumBones = m_Bones.size();

	// 사용하지 않는 뼈를 제거한다
	_int	iCount = { 0 };
	for (size_t i = 0; i < iNumBones; i++)
	{
		if ("" != m_pGameInstance->Extract_String(m_Bones[i]->Get_Name(), '[', ']'))
		{
			// decal이란 이름은 실제 사용하는 뼈인듯해서 제거하면안된다.
			regex Regex("decal");
			if(!regex_search(m_Bones[i]->Get_Name(), Regex))
				++iCount;
		}
	}

	iNumBones -= iCount;

	out.write((char*)&iNumBones, sizeof(_uint));

	for (auto& Bone : m_Bones)
	{
		string strValue = Bone->Get_Name();
		if ("" != m_pGameInstance->Extract_String(Bone->Get_Name(), '[', ']'))
		{
			// decal이란 이름은 실제 사용하는 뼈인듯해서 제거하면안된다.
			regex Regex("decal");
			if (!regex_search(Bone->Get_Name(), Regex))
				continue;
		}
		
		_int iValue = strValue.size();
		out.write((char*)&iValue, sizeof(_uint));
		out.write(strValue.c_str(), strValue.size());

		_int iParentIndex = Bone->Get_ParentBoneIndex();

		//iValue = (iParentIndex - iCount) < -1 ? iParentIndex : iParentIndex - iCount;
		iValue = iParentIndex;
		out.write((char*)&iValue, sizeof(_int));

		_float4x4 TransformationMatrix = *(Bone->Get_TransformationMatrix());
		out.write((char*)&TransformationMatrix, sizeof(_float4x4));
	}
	return S_OK;
}

HRESULT CModel::Export_Meshes(ofstream& out, const string& strOutDirectory)
{
	_uint iNumMeshes = m_iNumMeshes;

	out.write((char*)&iNumMeshes, sizeof(_uint));

	for (size_t i = 0; i < m_iNumMeshes; ++i)
	{
		string strValue = m_pAIScene->mMeshes[i]->mName.data;

		_uint iValue = strValue.size();
		out.write((char*)&iValue, sizeof(_uint));
		out.write(strValue.c_str(), strValue.size());

		iValue = m_pAIScene->mMeshes[i]->mNumVertices;
		out.write((char*)&iValue, sizeof(_uint));

		iValue = m_pAIScene->mMeshes[i]->mNumFaces;
		out.write((char*)&iValue, sizeof(_uint));

		iValue = m_pAIScene->mMeshes[i]->mMaterialIndex;
		out.write((char*)&iValue, sizeof(_uint));

		iValue = m_pAIScene->mMeshes[i]->mNumBones;
		out.write((char*)&iValue, sizeof(_uint));

		for (size_t j = 0; j < m_pAIScene->mMeshes[i]->mNumVertices; j++)
		{
			_float3 vVector;
			_float2 vTexcoord;

			memcpy(&vVector, &m_pAIScene->mMeshes[i]->mVertices[j], sizeof(_float3)) ;
			out.write((char*)&vVector, sizeof(_float3));

			memcpy(&vVector, &m_pAIScene->mMeshes[i]->mNormals[j], sizeof(_float3));
			out.write((char*)&vVector, sizeof(_float3));

			memcpy(&vTexcoord, &m_pAIScene->mMeshes[i]->mTextureCoords[0][j], sizeof(_float2));
			out.write((char*)&vTexcoord, sizeof(_float2));

			memcpy(&vVector, &m_pAIScene->mMeshes[i]->mTangents[j], sizeof(_float3));
			out.write((char*)&vVector, sizeof(_float3));
		}

		for (size_t j = 0; j < m_pAIScene->mMeshes[i]->mNumFaces; j++)
		{
			_uint arrInt[3];
			memcpy(&arrInt, &m_pAIScene->mMeshes[i]->mFaces[j].mIndices[0], sizeof(_uint) * 3);
			out.write((char*)&arrInt, sizeof(_uint) * 3);
		}

		for (size_t j = 0; j < m_pAIScene->mMeshes[i]->mNumBones; j++)
		{
			string strBoneName = m_pAIScene->mMeshes[i]->mBones[j]->mName.data;
			_uint iBoneNameSize = strBoneName.size();
			out.write((char*)&iBoneNameSize, sizeof(_uint));
			out.write(strBoneName.c_str(), strBoneName.size());

			_uint iNumWeights = m_pAIScene->mMeshes[i]->mBones[j]->mNumWeights;
			out.write((char*)&iNumWeights, sizeof(_uint));

			for (size_t k = 0; k < iNumWeights; k++)
			{
				_uint iVertexId = m_pAIScene->mMeshes[i]->mBones[j]->mWeights[k].mVertexId;
				out.write((char*)&iVertexId, sizeof(_uint));

				_float fValue = m_pAIScene->mMeshes[i]->mBones[j]->mWeights[k].mWeight;
				out.write((char*)&fValue, sizeof(_float));
			}

			_float4x4 matValue;
			memcpy(&matValue, &m_pAIScene->mMeshes[i]->mBones[j]->mOffsetMatrix, sizeof(_float4x4));
			out.write((char*)&matValue, sizeof(_float4x4));
		}
	}

	return S_OK;
}

HRESULT CModel::Export_Materials(ofstream& out, const _char* pModelFilePath, const string& strOutDirectory)
{
	out.write((char*)&m_iNumMaterials, sizeof(_uint));

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		string name = m_pAIScene->mMaterials[i]->GetName().C_Str();
		_uint iNumMaterialName = name.size();
		out.write((char*)&iNumMaterialName, sizeof(_uint));
		out.write(name.c_str(), iNumMaterialName);

		for (size_t j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString pPath;
			if (FAILED(m_pAIScene->mMaterials[i]->GetTexture((aiTextureType)j, 0, &pPath)))
			{
				_uint iZero = 0;
				string strZero = "";

				out.write((char*)&iZero, sizeof(_uint));
				out.write(strZero.c_str(), strZero.size());
				continue;
			}

			_char szFileName[MAX_PATH] = "";
			_char szExt[MAX_PATH] = "";

			_splitpath_s(pPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			_char		szDrive[MAX_PATH] = "";
			_char		szDirectory[MAX_PATH] = "";

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			_char		szFullPath[MAX_PATH] = "";

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szRealFullPath[MAX_PATH] = TEXT("");

			// WideChar로 형변환해주는 함수. code page를 아스키로 쓴다는 옵션 CP_ACP
			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szRealFullPath, MAX_PATH);

			string strPath = szFullPath;
			size_t pos = strPath.find("Bin");
			string strFullPath = strPath.substr(pos);

			_uint iLength = strFullPath.size();

			out.write((char*)&iLength, sizeof(_uint));
			out.write(strFullPath.data(), strFullPath.size());
		}
	}

	return S_OK;
}

HRESULT CModel::Export_Animations(ofstream& out)
{
	out.write((char*)&m_iAnimations, sizeof(_uint));
	
	for (size_t i = 0; i < m_iAnimations; i++)
	{
		aiAnimation* Animation = m_pAIScene->mAnimations[i];

		_uint iAnimNameLength = strlen(Animation->mName.data);
		out.write((char*)&iAnimNameLength, sizeof(_uint));
		out.write(Animation->mName.data, iAnimNameLength);

		out.write((char*)&Animation->mDuration, sizeof(_double));
		out.write((char*)&Animation->mTicksPerSecond, sizeof(_double));
		out.write((char*)&Animation->mNumChannels, sizeof(_uint));
		
		for (size_t j = 0; j < Animation->mNumChannels; j++)
		{
			aiNodeAnim* Channel = Animation->mChannels[j];

			_uint iChannelNameLength = strlen(Channel->mNodeName.data);
			out.write((char*)&iChannelNameLength, sizeof(_uint));
			out.write(Channel->mNodeName.data, iChannelNameLength);

			out.write((char*)&Channel->mNumScalingKeys, sizeof(_uint));
			out.write((char*)&Channel->mNumRotationKeys, sizeof(_uint));
			out.write((char*)&Channel->mNumPositionKeys, sizeof(_uint));

			_uint iNumKeyFrames = max(Channel->mNumScalingKeys, Channel->mNumRotationKeys);
			iNumKeyFrames = max(iNumKeyFrames, Channel->mNumPositionKeys);

			for (size_t k = 0; k < iNumKeyFrames; k++)
			{
				if (Channel->mNumScalingKeys > k)
				{
					out.write((char*)&Channel->mScalingKeys[k].mTime, sizeof(_double));
					out.write((char*)&Channel->mScalingKeys[k].mValue, sizeof(_float3));
				}
				
				if (Channel->mNumRotationKeys > k)
				{
					_float4 vRotationKeys{};
					vRotationKeys.x = Channel->mRotationKeys[k].mValue.x;
					vRotationKeys.y = Channel->mRotationKeys[k].mValue.y;
					vRotationKeys.z = Channel->mRotationKeys[k].mValue.z;
					vRotationKeys.w = Channel->mRotationKeys[k].mValue.w;

					out.write((char*)&Channel->mRotationKeys[k].mTime, sizeof(_double));
					out.write((char*)&vRotationKeys, sizeof(_float4));
				}

				if (Channel->mNumPositionKeys > k)
				{
					out.write((char*)&Channel->mPositionKeys[k].mTime, sizeof(_double));
					out.write((char*)&Channel->mPositionKeys[k].mValue, sizeof(_float3));
				}

			}
		}
	}

	return S_OK;
}

HRESULT CModel::Export_CamNums(ofstream& out)
{
	m_iCameras = m_pAIScene->mNumCameras;

	out.write((char*)&m_iCameras, sizeof(_uint));

	return S_OK;
}

HRESULT CModel::Import_Model(string& pBinFilePath, _bool isTool)
{
	ifstream in(pBinFilePath, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("Model 바이너리 파일 개방 실패");
		return E_FAIL;
	}

	// 읽어올 때 Mesh만 저장한 파일이라면 Mesh정보만 Import하는 기능을 추가해야한다.
	string strFileName = m_pGameInstance->Get_FileName(pBinFilePath);

	//본 읽어오기
	if(FAILED(Import_Bones(in)))
		return E_FAIL;
	
	//메시 읽어오기
	if (FAILED(Import_Meshes(in)))
		return E_FAIL;

	// 머테리얼 읽어오기
	if (FAILED(Import_Materials(in, isTool)))
		return E_FAIL;

	// 애니메이션 읽어오기
	if (FAILED(Import_Animations(in)))
		return E_FAIL;

	// 카메라 갯수 읽어오기
	if (FAILED(Import_CamNums(in)))
		return E_FAIL;

	in.close();

	/* DECAL 쓰는 MESH NUMBER 저장해두기 */
	Find_Mesh_Using_DECAL();

	return S_OK;
}

HRESULT CModel::Import_Bones(ifstream& in)
{
	// 본 읽어오기
	_int iNumBones = 0;
	in.read((char*)&iNumBones, sizeof(_uint));

	for (size_t i = 0; i < iNumBones; i++)
	{
		BAiNode BAINode{};

		_uint iStringSize = 0;
		in.read((char*)&iStringSize, sizeof(_uint));

		in.read(BAINode.mName, iStringSize);
		in.read((char*)&BAINode.mParentBoneIndex, sizeof(_int));
		in.read((char*)&BAINode.mTransformationMatrix, sizeof(_float4x4));

		m_Bones.emplace_back(CBone::Create(&BAINode));
	}

	return S_OK;
}

HRESULT CModel::Import_Meshes(ifstream& in)
{
	in.read((char*)&m_iNumMeshes, sizeof(_uint));

	for (size_t i = 0; i < m_iNumMeshes; ++i)
	{
		BAiMesh BAIMesh{};

		_uint iNameSize = 0;
		in.read((char*)&iNameSize, sizeof(_uint));
		in.read(BAIMesh.mName, iNameSize);

		in.read((char*)&BAIMesh.mNumVertices, sizeof(_uint));
		in.read((char*)&BAIMesh.mNumFaces, sizeof(_uint));
		in.read((char*)&BAIMesh.mMaterialIndex, sizeof(_uint));
		in.read((char*)&BAIMesh.mNumBones, sizeof(_uint));

		BAIMesh.mVertices = new _float3[BAIMesh.mNumVertices];
		BAIMesh.mNormals = new _float3[BAIMesh.mNumVertices];
		BAIMesh.mTextureCoords[0] = new _float2[BAIMesh.mNumVertices];
		BAIMesh.mTangents = new _float3[BAIMesh.mNumVertices];

		for (size_t j = 0; j < BAIMesh.mNumVertices; j++)
		{
			in.read((char*)&BAIMesh.mVertices[j], sizeof(_float3));
			in.read((char*)&BAIMesh.mNormals[j], sizeof(_float3));
			in.read((char*)&BAIMesh.mTextureCoords[0][j], sizeof(_float2));
			in.read((char*)&BAIMesh.mTangents[j], sizeof(_float3));
		}

		BAIMesh.mFaces = new BAiFace[BAIMesh.mNumFaces];

		for (size_t j = 0; j < BAIMesh.mNumFaces; j++)
		{
			BAIMesh.mFaces[j].mIndices = new _uint[3];

			in.read((char*)&BAIMesh.mFaces[j].mIndices[0], sizeof(_uint) * 3);
		}

		BAIMesh.mBones = new BAiBone * [BAIMesh.mNumBones];

		for (size_t j = 0; j < BAIMesh.mNumBones; j++)
		{
			BAIMesh.mBones[j] = new BAiBone();

			_uint iBoneNameSize = 0;
			in.read((char*)&iBoneNameSize, sizeof(_uint));
			in.read(BAIMesh.mBones[j]->mName, iBoneNameSize);

			in.read((char*)&BAIMesh.mBones[j]->mNumWeights, sizeof(_uint));

			BAIMesh.mBones[j]->mWeights = new BAiVertexWeight[BAIMesh.mBones[j]->mNumWeights];

			for (size_t k = 0; k < BAIMesh.mBones[j]->mNumWeights; k++)
			{
				in.read((char*)&BAIMesh.mBones[j]->mWeights[k].mVertexId, sizeof(_uint));
				in.read((char*)&BAIMesh.mBones[j]->mWeights[k].mWeight, sizeof(_float));
			}

			in.read((char*)&BAIMesh.mBones[j]->mOffsetMatrix, sizeof(_float4x4));
		}

		m_Meshes.emplace_back(CMesh::Create(m_pDevice, m_pContext, m_eModelType, &BAIMesh, XMLoadFloat4x4(&m_PreTransformMatrix), m_Bones, m_isTool));

		for (size_t j = 0; j < BAIMesh.mNumBones; j++)
		{
			Safe_Delete_Array(BAIMesh.mBones[j]->mWeights);
			Safe_Delete(BAIMesh.mBones[j]);
		}
		Safe_Delete_Array(BAIMesh.mBones);

		for (size_t j = 0; j < BAIMesh.mNumFaces; j++)
		{
			Safe_Delete_Array(BAIMesh.mFaces[j].mIndices);
		}

		Safe_Delete_Array(BAIMesh.mFaces);
		Safe_Delete_Array(BAIMesh.mTangents);
		Safe_Delete_Array(BAIMesh.mTextureCoords[0]);
		Safe_Delete_Array(BAIMesh.mNormals);
		Safe_Delete_Array(BAIMesh.mVertices);
	}

	return S_OK;
}

HRESULT CModel::Import_Materials(ifstream& in, _bool isTool)
{
	in.read((char*)&m_iNumMaterials, sizeof(_uint));

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MESH_MATERIAL		MeshMaterial{};
		_uint iNumMaterialName = 0;
		in.read((char*)&iNumMaterialName, sizeof(_uint));
		in.read(MeshMaterial.strMaterialName, iNumMaterialName);

		for (size_t j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			_char		szFullPath[MAX_PATH] = "";
			_tchar		szRealFullPath[MAX_PATH] = TEXT("");

			_uint iPathLen = 0;
			in.read((char*)&iPathLen, sizeof(_uint));
			in.read((char*)&szFullPath, iPathLen);


			string input = szFullPath;
			stringstream ss(input);
			string token;
			vector<string> tokens;

			// '/'를 구분자로 문자열을 분리
			while (std::getline(ss, token, '/')) {
				tokens.push_back(token);
			}

			if (!tokens.empty())
			{
				string lastToken = tokens.back();

				if (lastToken == ".") {
					continue;
				}
			}

			// WideChar로 형변환해주는 함수. code page를 아스키로 쓴다는 옵션 CP_ACP
			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szRealFullPath, MAX_PATH);

			if (0 == iPathLen) continue;

			wstring wstrPath = TEXT("");
			string strPath = "";
			if (isTool)
			{
				wstrPath = TEXT("../../Client/");
				strPath = "../../Client/";
			}
			else
			{
				wstrPath = TEXT("../");
				strPath = "../";
			}
			wstrPath += szRealFullPath;
			strPath += szFullPath;


			MeshMaterial.pMaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, wstrPath.data(), 1);
			if (nullptr == MeshMaterial.pMaterialTextures[j])
				return E_FAIL;

			//if (j == aiTextureType_EMISSIVE)
			//{
			//	/* DECAL */
			//	DECAL_DESC		decalDesc;
			//	decalDesc.iMaterialNum = i;
			
			//	memcpy(&decalDesc.sTextureFullPath, strPath.data(), sizeof(char) * MAX_PATH);

			//	//m_vDecalMaterials.push_back(decalDesc);
			//}
		}
		m_Materials.emplace_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Import_Animations(ifstream& in)
{
	in.read((char*)&m_iAnimations, sizeof(_uint));

	for (size_t i = 0; i < m_iAnimations; i++)
	{
		BAiAnimation Animation{};

		_uint	iAnimNameLength = 0;

		in.read((char*)&iAnimNameLength, sizeof(_uint));
		in.read(Animation.mName, iAnimNameLength);

		in.read((char*)&Animation.mDuration, sizeof(_double));
		in.read((char*)&Animation.mTicksPerSecond, sizeof(_double));
		in.read((char*)&Animation.mNumChannels, sizeof(_uint));

		Animation.mChannels = new BAiNodeAnim * [Animation.mNumChannels];

		for (size_t j = 0; j < Animation.mNumChannels; j++)
		{
			Animation.mChannels[j] = new BAiNodeAnim();

			_uint iChannelNameLength = 0;
			in.read((char*)&iChannelNameLength, sizeof(_uint));
			in.read(Animation.mChannels[j]->mName, iChannelNameLength);

			in.read((char*)&Animation.mChannels[j]->mNumScalingKeys, sizeof(_uint));
			in.read((char*)&Animation.mChannels[j]->mNumRotationKeys, sizeof(_uint));
			in.read((char*)&Animation.mChannels[j]->mNumPositionKeys, sizeof(_uint));

			_uint iNumKeyFrames = max(Animation.mChannels[j]->mNumScalingKeys, Animation.mChannels[j]->mNumRotationKeys);
			iNumKeyFrames = max(iNumKeyFrames, Animation.mChannels[j]->mNumPositionKeys);

			Animation.mChannels[j]->mScalingKeys = new VectorKey[iNumKeyFrames];
			Animation.mChannels[j]->mRotationKeys = new QuatKey[iNumKeyFrames];
			Animation.mChannels[j]->mPositionKeys = new VectorKey[iNumKeyFrames];

			for (size_t k = 0; k < iNumKeyFrames; k++)
			{
				if (Animation.mChannels[j]->mNumScalingKeys > k)
				{
					in.read((char*)&Animation.mChannels[j]->mScalingKeys[k].mTime, sizeof(_double));
					in.read((char*)&Animation.mChannels[j]->mScalingKeys[k].mValue, sizeof(_float3));
				}

				if (Animation.mChannels[j]->mNumRotationKeys > k)
				{
					in.read((char*)&Animation.mChannels[j]->mRotationKeys[k].mTime, sizeof(_double));
					in.read((char*)&Animation.mChannels[j]->mRotationKeys[k].mValue, sizeof(_float4));
				}

				if (Animation.mChannels[j]->mNumPositionKeys > k)
				{
					in.read((char*)&Animation.mChannels[j]->mPositionKeys[k].mTime, sizeof(_double));
					in.read((char*)&Animation.mChannels[j]->mPositionKeys[k].mValue, sizeof(_float3));
				}
			}
		}

		CAnimation* pAnimation = CAnimation::Create(&Animation, m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);

		for (size_t j = 0; j < Animation.mNumChannels; j++)
		{
			Safe_Delete_Array(Animation.mChannels[j]->mScalingKeys);
			Safe_Delete_Array(Animation.mChannels[j]->mRotationKeys);
			Safe_Delete_Array(Animation.mChannels[j]->mPositionKeys);

			Safe_Delete(Animation.mChannels[j]);
		}
		Safe_Delete_Array(Animation.mChannels);
	}

	return S_OK;
}

HRESULT CModel::Import_CamNums(ifstream& in)
{
	in.read((char*)&m_iCameras, sizeof(_uint));

	return S_OK;
}

void CModel::Find_Mesh_Using_DECAL()
{
	/* DECAL 쓰는 MESH NUMBER 저장해두기 */
	for (int i = 0; i < m_vDecalMaterials.size(); i++)
	{
		int		iNum = 0;
		int* pMaterialNums = new int[MAX_PATH];

		for (int j = 0; j < m_iNumMeshes; j++)
		{
			if (m_vDecalMaterials[i].iMaterialNum == m_Meshes[j]->Get_MaterialIndex())
			{
				pMaterialNums[iNum] = j;
				iNum++;
			}
		}

		m_vDecalMaterials[i].iMeshNum = iNum;

		if (iNum > 0)
		{
			m_vDecalMaterials[i].pMeshIndices = new int[iNum];
			
			for (int j = 0; j < iNum ; j++)
			{
				m_vDecalMaterials[i].pMeshIndices[j] = pMaterialNums[j];
			}

		}

		Safe_Delete_Array(pMaterialNums);
		
	}

}

void CModel::Check_Separation_Parents(CBone* pBone, _int iAnimType)
{
	if (pBone->Get_ParentBoneIndex() != -1)
	{
		if (m_Bones[pBone->Get_ParentBoneIndex()]->Get_Separation() == iAnimType)
		{
			pBone->Set_Separation(iAnimType);
			Check_Separation_Parents(m_Bones[pBone->Get_ParentBoneIndex()], iAnimType);
		}
	}
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	//m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render(m_isTool);

	return S_OK;
}

void CModel::Bind_BoneMatrices(_uint iNumMeshIndex)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * 512);

	m_Meshes[iNumMeshIndex]->Bind_Matrices(m_Bones, m_MeshBoneMatrices);
}

HRESULT CModel::Bind_Compute(CComputeShader* pComputeShader, _uint iNumMeshIndex)
{
	return m_Meshes[iNumMeshIndex]->Bind_Compute(pComputeShader);
}


HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iNumMeshIndex, aiTextureType eTextureType)
{
	auto* pTexture = m_Materials[m_Meshes[iNumMeshIndex]->Get_MaterialIndex()].pMaterialTextures[eTextureType];
	if (nullptr == pTexture)
		return E_FAIL;

	return pTexture->Bind_ShaderResource(pShader, pConstantName, 0);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iNumMeshIndex)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * 512);

	m_Meshes[iNumMeshIndex]->Fill_Matrices(m_Bones, m_MeshBoneMatrices);

	return pShader->Bind_Matrices(pConstantName, m_MeshBoneMatrices, 512);
}

bool CModel::Check_Exist_Material(_uint iNumMeshIndex, aiTextureType eTextureType)
{
	auto* pTexture = m_Materials[m_Meshes[iNumMeshIndex]->Get_MaterialIndex()].pMaterialTextures[eTextureType];
	if (nullptr == pTexture)
		return false;

	return true;
}

void CModel::Play_Animation(_float fTimeDelta, _bool isRoot, string strExcludeBoneName)
{
	if (1 > m_Animations.size()) return;

	if(0.0 == m_ChangeInterval)
		m_Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_AnimDesc.isLoop, isRoot, strExcludeBoneName);
	else
	{
		if (m_Animations[m_AnimDesc.iAnimIndex]->Get_Changed())
			m_Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_AnimDesc.isLoop, isRoot, strExcludeBoneName);
		else
			m_Animations[m_AnimDesc.iAnimIndex]->Update_Change_Animation(fTimeDelta, m_Bones, m_Animations[m_iPrevAnimIndex], m_ChangeInterval, isRoot);
	}

	/* 전체뼈를 순회하면서 모든 뼈의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
}

void CModel::Play_Animation(_float fTimeDelta, CAnim* pAnim, _bool isLoop, _int iAnimIndex, _bool isRoot, string strExcludeBoneName)
{
	//애니메이션 목록 전달하기 
	vector<CAnimation*> Animations = pAnim->Get_Animations();

	if (1 > Animations.size()) return;

	// 넘겨받은 iAnimIndex이 -1 이라면 기존대로 실행하고
	if (0 > iAnimIndex)
	{
		pAnim->Set_CurrentAnimIndex(m_AnimDesc.iAnimIndex);

		if (0.0 == m_ChangeInterval)
			Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, isLoop, isRoot, strExcludeBoneName);
		else
		{
			if (Animations[m_AnimDesc.iAnimIndex]->Get_Changed())
				Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, isLoop, isRoot, strExcludeBoneName);
			else
			{
				Animations[m_AnimDesc.iAnimIndex]->Update_Change_Animation(fTimeDelta, m_Bones, Animations[m_iPrevAnimIndex], m_ChangeInterval, isRoot);
			}
		}
	}
	// -1이 아니라면 넘겨받은 인덱스를 실행시킨다.
	else
	{
		pAnim->Set_CurrentAnimIndex(iAnimIndex);

		if (0.0 == m_ChangeInterval)
			Animations[iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, isLoop, isRoot, strExcludeBoneName);
		else
		{
			if (Animations[iAnimIndex]->Get_Changed())
				Animations[iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, isLoop, isRoot, strExcludeBoneName);
			else
			{
				Animations[iAnimIndex]->Update_Change_Animation(fTimeDelta, m_Bones, Animations[m_iPrevAnimIndex], m_ChangeInterval, isRoot);
			}
		}
	}

	/* 전체뼈를 순회하면서 모든 뼈의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
}

void CModel::Play_Animation(_float fTimeDelta, const ANIMATION_DESC& AnimDesc, _bool isRoot, string strExcludeBoneName)
{
	if (1 > m_Animations.size()) return;

	if (0.0 == m_ChangeInterval)
		m_Animations[AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, AnimDesc.isLoop, isRoot, strExcludeBoneName);
	else
	{
		if (m_Animations[AnimDesc.iAnimIndex]->Get_Changed())
			m_Animations[AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, AnimDesc.isLoop, isRoot, strExcludeBoneName);
		else
			m_Animations[AnimDesc.iAnimIndex]->Update_Change_Animation(fTimeDelta, m_Bones, m_Animations[m_iPrevAnimIndex], m_ChangeInterval, isRoot);
	}

	/* 전체뼈를 순회하면서 모든 뼈의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
}

// 컷신 실행용
void CModel::Play_Animation_CutScene(_float fTimeDelta, CAnim* pAnim, _bool isLoop, _int iAnimIndex, _bool isRoot, string strExcludeBoneName)
{
	//애니메이션 목록 전달하기 
	if (nullptr == pAnim)
	{
		if (1 > m_Animations.size()) return;

		m_ChangeInterval = 0.0;

		// 넘겨받은 iAnimIndex이 -1 이라면 기존대로 실행하고
		if (0 > iAnimIndex)
		{
			if (0.0 == m_ChangeInterval)
				m_Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_AnimDesc.isLoop, isRoot, strExcludeBoneName);
			else
			{
				if (m_Animations[m_AnimDesc.iAnimIndex]->Get_Changed())
					m_Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_AnimDesc.isLoop, isRoot, strExcludeBoneName);
				else
					m_Animations[m_AnimDesc.iAnimIndex]->Update_Change_Animation(fTimeDelta, m_Bones, m_Animations[m_iPrevAnimIndex], m_ChangeInterval, isRoot);
			}
		}
		// -1이 아니라면 넘겨받은 인덱스를 실행시킨다.
		else
		{
			if (0.0 == m_ChangeInterval)
				m_Animations[iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_AnimDesc.isLoop, isRoot, strExcludeBoneName);
			else
			{
				if (m_Animations[iAnimIndex]->Get_Changed())
					m_Animations[iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_AnimDesc.isLoop, isRoot, strExcludeBoneName);
				else
					m_Animations[iAnimIndex]->Update_Change_Animation(fTimeDelta, m_Bones, m_Animations[m_iPrevAnimIndex], m_ChangeInterval, isRoot);
			}
		}

	}
	else
	{
		vector<CAnimation*> Animations = pAnim->Get_Animations();

		if (1 > Animations.size()) return;

		m_ChangeInterval = 0.0;

		// 넘겨받은 iAnimIndex이 -1 이라면 기존대로 실행하고
		if (0 > iAnimIndex)
		{
			pAnim->Set_CurrentAnimIndex(m_AnimDesc.iAnimIndex);

			if (0.0 == m_ChangeInterval)
				Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, isLoop, isRoot, strExcludeBoneName);
			else
			{
				if (Animations[m_AnimDesc.iAnimIndex]->Get_Changed())
					Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, isLoop, isRoot, strExcludeBoneName);
				else
				{
					Animations[m_AnimDesc.iAnimIndex]->Update_Change_Animation(fTimeDelta, m_Bones, Animations[m_iPrevAnimIndex], m_ChangeInterval, isRoot);
				}
			}
		}
		// -1이 아니라면 넘겨받은 인덱스를 실행시킨다.
		else
		{
			pAnim->Set_CurrentAnimIndex(iAnimIndex);

			if (0.0 == m_ChangeInterval)
				Animations[iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, isLoop, isRoot, strExcludeBoneName);
			else
			{
				if (Animations[iAnimIndex]->Get_Changed())
					Animations[iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, isLoop, isRoot, strExcludeBoneName);
				else
				{
					Animations[iAnimIndex]->Update_Change_Animation(fTimeDelta, m_Bones, Animations[m_iPrevAnimIndex], m_ChangeInterval, isRoot);
				}
			}
		}
	}

	/* 전체뼈를 순회하면서 모든 뼈의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
}

void CModel::Play_Animation_Monster(_float fTimeDelta, class CAnim* pAnim, _bool isLoop, _bool isRoot)
{
	//애니메이션 목록 전달하기 ;
	vector<CAnimation*> Animations = pAnim->Get_Animations();

	if (1 > Animations.size()) return;

	pAnim->Set_CurrentAnimIndex(m_AnimDesc.iAnimIndex);

	if (0.0 == m_ChangeInterval)
		Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, isLoop, isRoot);
	else
	{
		if (Animations[m_AnimDesc.iAnimIndex]->Get_Changed())
			Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, isLoop, isRoot);
		else
		{
			Animations[m_AnimDesc.iAnimIndex]->Update_Change_Animation(fTimeDelta, m_Bones, m_PreAnimations[m_iPrevAnimIndex], m_ChangeInterval, isRoot);
		}
	}

	/* 전체뼈를 순회하면서 모든 뼈의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
}

void CModel::Play_Animation_Separation(_float fTimeDelta, _uint iAnimIndex, CAnim* pAnim, _bool isLoop, _int iAnimType, _float fChangeInterval)
{
		//애니메이션 목록 전달하기 ;
	vector<CAnimation*> Animations = pAnim->Get_Animations();

	if (1 > Animations.size()) return;

	if (0.0 == fChangeInterval)
		Animations[iAnimIndex]->Update_TransformationMatrix_Separation(fTimeDelta, m_Bones, isLoop, iAnimType);
	else
	{
		if (Animations[iAnimIndex]->Get_Changed())
		{
			pAnim->Set_PrevAnimIndex(pAnim->Get_CurrentAnimIndex());
			pAnim->Set_CurrentAnimIndex(iAnimIndex);
			Animations[iAnimIndex]->Update_TransformationMatrix_Separation(fTimeDelta, m_Bones, isLoop, iAnimType);
		}
		else
		{
			Animations[iAnimIndex]->Update_Change_Animation_Separation(fTimeDelta, m_Bones, Animations[pAnim->Get_PrevAnimIndex()], fChangeInterval, iAnimType);
		}
	}

	/* 전체뼈를 순회하면서 모든 뼈의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
}

void CModel::Play_Animation_Rotation_SeparationBone(_float fTimeDelta, string strBoneName, _uint iDir, _float fRadian, _bool isRoot, _bool isLoop, CAnim* pAnim)
{
	// 모델이 가지고 있는 애니메이션 리스트에서 업데이트한다.
	if (nullptr == pAnim)
	{
		if (1 > m_Animations.size()) return;

		if (0.0 == m_ChangeInterval)
			m_Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_AnimDesc.isLoop, isRoot);
		else
		{
			if (m_Animations[m_AnimDesc.iAnimIndex]->Get_Changed())
				m_Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_AnimDesc.isLoop, isRoot);
			else
				m_Animations[m_AnimDesc.iAnimIndex]->Update_Change_Animation(fTimeDelta, m_Bones, m_Animations[m_iPrevAnimIndex], m_ChangeInterval, isRoot);
		}
	}
	else
	{
		//애니메이션 목록 전달하기 ;
		vector<CAnimation*> Animations = pAnim->Get_Animations();

		if (1 > Animations.size()) return;

		pAnim->Set_CurrentAnimIndex(m_AnimDesc.iAnimIndex);

		if (0.0 == m_ChangeInterval)
			Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, isLoop, isRoot);
		else
		{
			if (Animations[m_AnimDesc.iAnimIndex]->Get_Changed())
				Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, isLoop, isRoot);
			else
			{
				Animations[m_AnimDesc.iAnimIndex]->Update_Change_Animation(fTimeDelta, m_Bones, Animations[m_iPrevAnimIndex], m_ChangeInterval, isRoot);
			}
		}
	}

	//_matrix RotationMatrix = XMMatrixRotationAxis
	/* 전체뼈를 순회하면서 모든 뼈의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		// 만약 지금뼈가 넘겨받은 뼈 이름이랑 같다면, 계산된 컴바인드 상태에서 한번 더 회전행렬을 적용시킨다.
		if (!strcmp(pBone->Get_Name(), strBoneName.c_str()))
		{
			auto& vTemp = pBone->Get_TransformationMatrix()->m[iDir][0];
			_vector vBoneDir;
			memcpy(&vBoneDir, &vTemp, sizeof(_vector));

			vBoneDir = XMVector3Normalize(vBoneDir);

			_matrix RotationMatrix = XMMatrixRotationAxis(vBoneDir, fRadian);
			_matrix BoneTransformationMatrix = XMLoadFloat4x4(pBone->Get_TransformationMatrix());

			_matrix ValueMat = BoneTransformationMatrix * RotationMatrix;
			ValueMat.r[CTransform::STATE_POSITION] = BoneTransformationMatrix.r[CTransform::STATE_POSITION];

			pBone->Set_TransformationMatrix(ValueMat);
			pBone->Set_CustomRotationMatrix(RotationMatrix);			// 얼마나 돌렸는지를 저장
		}

		// 컴바인드행렬을 애니메이션대로 우선 갱신 시키고
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}
}

void CModel::Set_AnimationIndex(const ANIMATION_DESC& AnimDesc, _double ChangeInterval)
{
	if (AnimDesc.iAnimIndex >= m_Animations.size()) return;

	if (m_AnimDesc.iAnimIndex == AnimDesc.iAnimIndex && AnimDesc.isLoop == m_AnimDesc.isLoop)
		return;

	m_iPrevAnimIndex = m_AnimDesc.iAnimIndex;
	m_AnimDesc = AnimDesc;
	m_ChangeInterval = ChangeInterval;
	m_Animations[m_AnimDesc.iAnimIndex]->Reset();
}

_bool CModel::Set_AnimationIndex(_uint iAnimIndex, _double ChangeInterval)
{
	if (iAnimIndex >= m_Animations.size()) return false;

	if (m_AnimDesc.iAnimIndex == iAnimIndex && m_AnimLoops[iAnimIndex] == m_AnimDesc.isLoop)
		return false;

	m_iPrevAnimIndex = m_AnimDesc.iAnimIndex;
	m_AnimDesc = { iAnimIndex, m_AnimLoops[iAnimIndex] };
	m_ChangeInterval = ChangeInterval;
	m_Animations[m_AnimDesc.iAnimIndex]->Reset();

	return true;
}

_bool CModel::Set_AnimationIndex(_uint iAnimIndex, vector<class CAnimation*> Animations, _double ChangeInterval)
{
	if (iAnimIndex >= Animations.size()) return false;

	if (m_AnimDesc.iAnimIndex == iAnimIndex)
		return false;

	m_iPrevAnimIndex = m_AnimDesc.iAnimIndex;
	m_AnimDesc = { iAnimIndex, false };
	m_ChangeInterval = ChangeInterval;
	Animations[m_AnimDesc.iAnimIndex]->Reset();

	return true;
}

void CModel::Reset_Animation(const ANIMATION_DESC& AnimDesc)
{
	m_Animations[m_AnimDesc.iAnimIndex]->Reset();
}

void CModel::Reset_Animation(_uint iAnimIndex)
{
	m_Animations[iAnimIndex]->Reset();
}

void CModel::Set_Separation_ParentBone(string strBoneName, _int iAnimType)
{
	_bool isChild = false;

	for (auto& pBone : m_Bones)
	{
		if (pBone->Compare_Name(strBoneName.c_str()))
		{
			pBone->Set_Separation(iAnimType);

			isChild = true;
		}

		// 부모뼈가 -1이라면 건너뛰기
		if (pBone->Get_ParentBoneIndex() == -1)
			continue; 

		// 현재 뼈의 부모뼈를 구해서, 그 부모뼈가 제외된 뼌지 확인한다.
		// 부모가 제외되었다면 본인도 제외한다.
		// 부모의 부모뼈도 다 봐야한다!! 재귀함수임
		// 설정한 뼈의 자식뼈들의 경우만 상위부모를 체크한다.
		if(isChild)
			Check_Separation_Parents(pBone, iAnimType);
	}
}

void CModel::Set_Separation_SingleBone(string strBoneName, _int iAnimType)
{
	for (auto& pBone : m_Bones)
	{
		if (pBone->Compare_Name(strBoneName.c_str()))
		{
			pBone->Set_Separation(iAnimType);
		}
	}
}

const vector<_uint>* CModel::Get_CurrentKeyFrameIndices(string strAnimationName)
{
	if(strAnimationName == "")
		return m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentKeyFrameIndices();
	else
	{
		for (auto& pAnimation : m_Animations)
		{
			if ((pAnimation->Get_AnimName() == strAnimationName))
			{
				return pAnimation->Get_CurrentKeyFrameIndices();
			}
		}
		
	}
}

const vector<_uint>* CModel::Get_CurrentKeyFrameIndices(_uint iAnimIndex)
{
	return m_Animations[iAnimIndex]->Get_CurrentKeyFrameIndices();
}

_float CModel::Get_FoV(string strAnimationName, _uint iKeyFrameIndex)
{
	for (auto& Fov : m_FovAnimation)
	{
		std::string originalString = strAnimationName;
		std::string toRemove = "Camera|";

		// 문자열에서 특정 패턴 제거
		size_t pos = originalString.find(toRemove);
		if (pos != std::string::npos) {
			originalString.erase(pos, toRemove.length());
		}

		if (Fov.iKeyFrameIndex == iKeyFrameIndex && Fov.strAnimationName == originalString)
		{
			return Fov.fFov;
		}
	}

	// 따로 값이 없다면 기존에 사용하던 기본값 리턴
	return XMConvertToRadians(60.0f);
}

_bool CModel::Get_AnimFinished() const
{
	return m_Animations[m_AnimDesc.iAnimIndex]->Get_Finished();
}

_bool CModel::Get_AnimFinished(_uint iIndex) const
{
	return m_Animations[iIndex]->Get_Finished();
}

_bool CModel::Get_AnimChanged() const
{
	return m_Animations[m_AnimDesc.iAnimIndex]->Get_Changed();
}

_bool CModel::Get_AnimRestart() const
{
	// 루프가 아닐경우 무조건 Restart 는 false를 반환하게한다
	return m_AnimLoops[m_AnimDesc.iAnimIndex] && m_Animations[m_AnimDesc.iAnimIndex]->Get_Restrat();
}

_bool CModel::Get_AnimLerp() const
{
	return m_ChangeInterval == 0.f ? false : true;
}

const _float4x4* CModel::Get_BoneCombinedTransformationMatrix(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			return pBone->Compare_Name(pBoneName);
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationMatrix();
}

const _float4x4* CModel::Get_BoneCombinedTransformationMatrix_AtIndex(_uint iBoneIndex) const
{
	if (m_Bones.size() <= iBoneIndex) return nullptr;
	auto	pBone = m_Bones[iBoneIndex];

	return pBone->Get_CombinedTransformationMatrix();
}

const _float4x4* CModel::Get_BoneTransformationMatrix(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			return pBone->Compare_Name(pBoneName);
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_TransformationMatrix();
}

const _char* CModel::Get_AnimationName(_uint iAnimIndex)
{
	return m_Animations[iAnimIndex]->Get_AnimName();
}

const _double* CModel::Get_AnimationCurrentPosition(CAnim* pAnim, _int iAnimIndex)
{
	if (nullptr == pAnim)
		return m_Animations[iAnimIndex == -1 ? m_AnimDesc.iAnimIndex : iAnimIndex]->Get_CurrentPosition();
	else
		return pAnim->Get_AnimPosition();
}

const _double* CModel::Get_AnimationDuration(CAnim* pAnim, _int iAnimIndex)
{
	if (nullptr == pAnim)
		return m_Animations[iAnimIndex == -1 ? m_AnimDesc.iAnimIndex : iAnimIndex]->Get_Duration();
	else
		return pAnim->Get_AnimDuration();
}

const _float3* CModel::Get_AnimationCenterMove(CAnim* pAnim, _int iAnimIndex)
{
	if(nullptr == pAnim)
		return m_Animations[iAnimIndex == -1 ? m_AnimDesc.iAnimIndex : iAnimIndex]->Get_CenterMoveValue();
	else
		return pAnim->Get_AnimationCenterMove();
}

const _float4* CModel::Get_AnimationCenterRotation(CAnim* pAnim, _int iAnimIndex)
{
	if (nullptr == pAnim)
		return m_Animations[iAnimIndex == -1 ? m_AnimDesc.iAnimIndex : iAnimIndex]->Get_CenterRotationValue();
	else
		return pAnim->Get_AnimationCenterRotation();
}

const _float3* CModel::Get_LastKeyframe_Position(string strChannelName, CAnim* pAnim, _int iAnimIndex)
{
	if (nullptr == pAnim)
		return m_Animations[iAnimIndex == -1 ? m_AnimDesc.iAnimIndex : iAnimIndex]->Get_LastKeyframe_Position(strChannelName);
	else
		return pAnim->Get_LastKeyframe_Position(iAnimIndex, strChannelName);
}

const _float3* CModel::Get_FirstKeyframe_Position(string strChannelName, CAnim* pAnim, _int iAnimIndex)
{
	if (nullptr == pAnim)
		return m_Animations[iAnimIndex == -1 ? m_AnimDesc.iAnimIndex : iAnimIndex]->Get_FirstKeyframe_Position(strChannelName);
	else
		return pAnim->Get_FirstKeyframe_Position(iAnimIndex, strChannelName);
}

const _float4* CModel::Get_LastKeyframe_Rotation(string strChannelName, CAnim* pAnim, _int iAnimIndex)
{
	if (nullptr == pAnim)
		return m_Animations[iAnimIndex == -1 ? m_AnimDesc.iAnimIndex : iAnimIndex]->Get_LastKeyframe_Rotation(strChannelName);
	else
		return pAnim->Get_LastKeyframe_Rotation(iAnimIndex, strChannelName);
}

const _float4* CModel::Get_FirstKeyframe_Rotation(string strChannelName, CAnim* pAnim, _int iAnimIndex)
{
	if (nullptr == pAnim)
		return m_Animations[iAnimIndex == -1 ? m_AnimDesc.iAnimIndex : iAnimIndex]->Get_FirstKeyframe_Rotation(strChannelName);
	else
		return pAnim->Get_FirstKeyframe_Rotation(iAnimIndex, strChannelName);
}

_float4x4 CModel::Get_LocalMatrix()
{
	if (0 == m_Meshes.size())
	{
		//예외처리
		_float4x4	localMatrix;
		XMStoreFloat4x4(&localMatrix, XMMatrixIdentity());
		return localMatrix;
	}
	else
	{
		return m_Meshes[0]->Get_LocalMatrix();
	}
	
}

void CModel::Copy_DecalMaterial(vector<DECAL_DESC>* pDecals)
{
	for (int i = 0; i < m_vDecalMaterials.size(); i++)
	{
		DECAL_DESC		decalDesc;
		memcpy(&decalDesc, &m_vDecalMaterials[i], sizeof(DECAL_DESC));
		pDecals->push_back(decalDesc);
	}
}

CTexture* CModel::Copy_DecalTexture(int iMaterialNum)
{
	return m_Materials[iMaterialNum].pMaterialTextures[aiTextureType_EMISSIVE];
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, _bool isExported, _bool isTool)
 {
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PreTransformMatrix, isExported, isTool)))
	{
		MSG_BOX("Failed To Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	for (auto& MaterialDesc : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(MaterialDesc.pMaterialTextures[i]);
	}

	m_Materials.clear();

	if (true == m_bOrigin)
	{
		for (auto& iter : m_vDecalMaterials)
		{
			Safe_Delete(iter.pMeshIndices);
		}
	}
	m_vDecalMaterials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();

	m_Importer.FreeScene();
}