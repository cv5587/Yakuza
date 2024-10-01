#include "Calculator.h"

CCalculator::CCalculator()
{
}

HRESULT CCalculator::Initialize()
{
	return S_OK;
}

_float CCalculator::Lerp(_float Start, _float End, _float Factor)
{
	return Start + (End - Start) * Factor;
}

_uint CCalculator::Lerp(_uint Start, _uint End, _float Factor)
{
	return Start + (End - Start) * Factor;
}

vector<_int> CCalculator::Compute_LPS(const string& pattern) {
    _int m = pattern.size();
    vector<_int> lps(m, 0);
    _int len = 0; // 이전 LPS 길이
    _int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        }
        else {
            if (len != 0) {
                len = lps[len - 1]; // 이전 LPS 길이로 돌아가기
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }

    return lps;
}

// KMP 알고리즘을 사용하여 문자열 검색
_bool CCalculator::Find_String(const string& text, const string& pattern) {
    _int n = text.size();
    _int m = pattern.size();
    vector<_int> lps = Compute_LPS(pattern);
    _int i = 0; // text의 인덱스
    _int j = 0; // pattern의 인덱스

    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }

        if (j == m) {
            return true; // 패턴이 텍스트에 포함됨
        }
        else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1]; // LPS 배열 사용
            }
            else {
                i++;
            }
        }
    }

    return false; // 패턴이 텍스트에 포함되지 않음
}

CCalculator* CCalculator::Create()
{
	CCalculator* pInstance = new CCalculator();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CCalculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCalculator::Free()
{
}
