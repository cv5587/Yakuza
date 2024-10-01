#include "NoteManager.h"
#include "GameInstance.h"

#include "NoteSingle.h"
#include "NoteLong.h"
#include "NoteBurstHold.h"

CNoteManager::CNoteManager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CNoteManager::Initialize(string strSongName)
{
	Load_NoteData(strSongName);
    return S_OK;
}


HRESULT CNoteManager::Load_NoteData(string strSongName)
{
	string pBinFilePath = "../Bin/DataFiles/KaraokeNoteData/" + strSongName + "_Note.csv";

	ifstream in(pBinFilePath, ios::binary);

	if (!in.is_open()) {

#ifdef _DEBUG
		MSG_BOX("Note 데이터 로드 실패");
#endif // _DEBUG
		return E_FAIL;
	}

	string line;
	// 헤더를 건너뜁니다.
	getline(in, line);

	while (getline(in, line)) {
		istringstream lineStream(line);
		string strNoteType, Button, TimePos_S, TimePos_E;

		/*
		*	0 일반노드
			1 유지 노드
			2 연타 노드 
		*/

		// 노트 타입
		getline(lineStream, strNoteType, ',');
		getline(lineStream, Button, ',');
		getline(lineStream, TimePos_S, ',');
		getline(lineStream, TimePos_E, ',');


		if (strNoteType == "0")//일반
		{
			// 넘겨주는 인자값을 파일에 저장해둔 값을 넘겨주고, 이니셜라이즈 할 때 저장해야한다.
			m_Notes.push_back(CNoteSingle::Create(stoi(strNoteType), stoi(Button), stof(TimePos_S)));
		}
		else if (strNoteType == "1")//유지
		{
			// 넘겨주는 인자값을 파일에 저장해둔 값을 넘겨주고, 이니셜라이즈 할 때 저장해야한다.
			m_Notes.push_back(CNoteLong::Create(stoi(strNoteType), stoi(Button), stof(TimePos_S), stof(TimePos_E)));
		}
		else if (strNoteType == "2")//연타
		{
			// 넘겨주는 인자값을 파일에 저장해둔 값을 넘겨주고, 이니셜라이즈 할 때 저장해야한다.
			m_Notes.push_back(CNoteBurstHold::Create(stoi(strNoteType), stoi(Button), stof(TimePos_S), stof(TimePos_E)));
		}

	}

	return S_OK;
}

CNoteManager* CNoteManager::Create(string strSongName)
{
	CNoteManager* pInstance = new CNoteManager;
	if (FAILED(pInstance->Initialize(strSongName)))
	{
		MSG_BOX("NoteManager make Failed");
		return nullptr;
	}
	return pInstance;
}

void CNoteManager::Free()
{
    Safe_Release(m_pGameInstance);

	for (auto& pNote : m_Notes)
		Safe_Release(pNote);
	m_Notes.clear();
}