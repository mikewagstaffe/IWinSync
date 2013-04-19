#include "stdafx.h"
#include "ConflictResult.h"


CConflictResult::CConflictResult(CONFLICT_RESOLUTION uResolution, LPCWSTR *ppszConflictedFile, LPCWSTR *ppszNewFile)
{
	//Save the Resolution
	m_uResolution = uResolution;
	
	//Save the confilcetd path
	size_t PathSize =  (wcslen (*ppszConflictedFile) + 1) * sizeof(wchar_t);
	m_pszConflictedFile = (LPCWSTR) malloc(PathSize);
	if (m_pszConflictedFile != NULL) 
	{
		memcpy((void *)m_pszConflictedFile,*ppszConflictedFile,PathSize);
	}

	//save the path of the copied file
	PathSize =  (wcslen (*ppszNewFile) + 1) * sizeof(wchar_t);
	m_pszNewFile = (LPCWSTR) malloc(PathSize);
	if (m_pszNewFile != NULL) 
	{
		memcpy((void *)m_pszNewFile,*ppszNewFile,PathSize);
	}
}


CConflictResult::~CConflictResult(void)
{
	if (m_pszConflictedFile != NULL)
	{
		free((void*)m_pszConflictedFile);
	}

	if (m_pszNewFile != NULL)
	{
			free((void*)m_pszNewFile);
	}
}

void CConflictResult::ConflictedFile(LPCWSTR* ppszConflictedFile) 
{
	if (m_pszConflictedFile != NULL)
	{
		size_t PathSize =  (wcslen (m_pszConflictedFile) + 1) * sizeof(wchar_t);
		*ppszConflictedFile = (LPCWSTR) malloc(PathSize);
		if (*ppszConflictedFile != NULL) 
		{
			memcpy((void *)*ppszConflictedFile,m_pszConflictedFile,PathSize);
		}
	}
}

void CConflictResult::NewFileFile(LPCWSTR *ppszNewFile)
{
	if (m_pszNewFile != NULL)
	{
		size_t PathSize =  (wcslen (m_pszNewFile) + 1) * sizeof(wchar_t);
		*ppszNewFile = (LPCWSTR) malloc(PathSize);
		if (*ppszNewFile != NULL) 
		{
			memcpy((void *)*ppszNewFile,m_pszConflictedFile,PathSize);
		}
	}
}

CONFLICT_RESOLUTION  CConflictResult::ConflictResolved()
{
	return m_uResolution;
}
