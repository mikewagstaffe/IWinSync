#pragma once


enum CONFLICT_RESOLUTION
{
	CONFLICT_RESOLUTION_RESOLVED,
	CONFLICT_RESOLUTION_POLICY_RESOLVED,
	CONFLICT_RESOLUTION_NOT_RESOLVED,
};


class CConflictResult
{
public:
	CConflictResult(CONFLICT_RESOLUTION uResolution, LPCWSTR *ppszConflictedFile, LPCWSTR *ppszNewFile);
	~CConflictResult(void);

	void ConflictedFile(LPCWSTR *ppszConflictedFile); 
	void NewFileFile(LPCWSTR *ppszNewFile);
	CONFLICT_RESOLUTION ConflictResolved();

public:
	CONFLICT_RESOLUTION m_uResolution;
	LPCWSTR m_pszConflictedFile;
	LPCWSTR m_pszNewFile;
};

