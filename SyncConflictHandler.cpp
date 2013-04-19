#include "stdafx.h"
#include "SyncConflictHandler.h"
#include "ConflictResult.h"

CSyncConflictHandler::CSyncConflictHandler(UINT uWMSyncConflict, HWND hwndApplication)
{
	m_uWMSyncConflict = uWMSyncConflict;
	m_hwndApplication = hwndApplication;
	m_cRef = 0;
}


CSyncConflictHandler::~CSyncConflictHandler(void)
{
}

HRESULT STDMETHODCALLTYPE CSyncConflictHandler::ResolveConflict(LPCWSTR pszPath, DWORD fStateKnown, OFFLINEFILES_SYNC_STATE state, DWORD fChangeDetails, OFFLINEFILES_SYNC_CONFLICT_RESOLVE *pConflictResolution,LPWSTR *ppszNewName)
{
	CConflictResult *pConflictResult = NULL;
	
	switch (state)
	{
		case OFFLINEFILES_SYNC_STATE_Stable:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_NONE;
			break;
		case OFFLINEFILES_SYNC_STATE_FileOnClient_DirOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_FileOnClient_NoServerCopy:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_DirOnClient_FileOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_DirOnClient_FileChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_DirOnClient_NoServerCopy:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_FileCreatedOnClient_NoServerCopy:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_FileCreatedOnClient_FileChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_FileCreatedOnClient_DirChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_FileCreatedOnClient_FileOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_FileCreatedOnClient_DirOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_FileCreatedOnClient_DeletedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_FileChangedOnClient_ChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_FileChangedOnClient_DirOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_FileChangedOnClient_DirChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_FileChangedOnClient_DeletedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_NONE;
			break;
		case OFFLINEFILES_SYNC_STATE_FileSparseOnClient_ChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_NONE;
			break;
		case OFFLINEFILES_SYNC_STATE_FileSparseOnClient_DeletedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_NONE;
			break;
		case OFFLINEFILES_SYNC_STATE_FileSparseOnClient_DirOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_NONE;
			break;
		case OFFLINEFILES_SYNC_STATE_FileSparseOnClient_DirChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_NONE;
			break;
		case OFFLINEFILES_SYNC_STATE_DirCreatedOnClient_NoServerCopy:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_DirCreatedOnClient_DirOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_LOG;
			break;
		case OFFLINEFILES_SYNC_STATE_DirCreatedOnClient_FileOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_DirCreatedOnClient_FileChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_DirCreatedOnClient_DirChangedOnServer:	
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_LOG;
			break;
		case OFFLINEFILES_SYNC_STATE_DirCreatedOnClient_DeletedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_DirChangedOnClient_FileOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_DirChangedOnClient_FileChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_DirChangedOnClient_ChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_LOG;    
			break;
		case OFFLINEFILES_SYNC_STATE_DirChangedOnClient_DeletedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_NoClientCopy_FileOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPREMOTE;
			break;
		case OFFLINEFILES_SYNC_STATE_NoClientCopy_DirOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPREMOTE;
			break;
		case OFFLINEFILES_SYNC_STATE_NoClientCopy_FileChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPREMOTE;
			break;
		case OFFLINEFILES_SYNC_STATE_NoClientCopy_DirChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPREMOTE;
			break;
		case OFFLINEFILES_SYNC_STATE_DeletedOnClient_FileOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_DeletedOnClient_DirOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_DeletedOnClient_FileChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPREMOTE;
			break;
		case OFFLINEFILES_SYNC_STATE_DeletedOnClient_DirChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPREMOTE;
			break;
		case OFFLINEFILES_SYNC_STATE_FileSparseOnClient:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_NONE;
			break;
		case OFFLINEFILES_SYNC_STATE_FileChangedOnClient:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_FileRenamedOnClient:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_DirSparseOnClient:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_NONE;
			break;
		case OFFLINEFILES_SYNC_STATE_DirChangedOnClient:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_DirRenamedOnClient:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_FileChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPREMOTE;
			break;
		case OFFLINEFILES_SYNC_STATE_FileRenamedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPREMOTE;
			break;
		case OFFLINEFILES_SYNC_STATE_FileDeletedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPREMOTE;
			break;
		case OFFLINEFILES_SYNC_STATE_DirChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPREMOTE;
			break;
		case OFFLINEFILES_SYNC_STATE_DirRenamedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPREMOTE;
			break;
		case OFFLINEFILES_SYNC_STATE_DirDeletedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPREMOTE;
			break;
		case OFFLINEFILES_SYNC_STATE_FileReplacedAndDeletedOnClient_FileOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPLOCAL;
			break;
		case OFFLINEFILES_SYNC_STATE_FileReplacedAndDeletedOnClient_FileChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_FileReplacedAndDeletedOnClient_DirOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
		case OFFLINEFILES_SYNC_STATE_FileReplacedAndDeletedOnClient_DirChangedOnServer:
			*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES;
			break;
	}

	
	if (*pConflictResolution == OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPALLCHANGES)
	{
		//Find the last \ in the path name
		//if there are characters after the \ then find the extension (.)
		const wchar_t * pszFileName = wcsrchr(pszPath,L'\\');
		const wchar_t * pszFileNameExt = NULL;
		wchar_t *pszUsername = NULL;

		if (pszFileName != NULL)
		{   
			pszFileName++;
			pszFileNameExt = wcsrchr(pszFileName,L'.');
		}

		if ( !ConvertUsername(&pszUsername))
		{
			pszUsername = NULL;
		}

		wchar_t *pszTempPath = (wchar_t *) malloc(MAX_PATH*sizeof(wchar_t));
		wchar_t *pszNewFile = NULL;

		if (pszFileName != NULL && pszFileNameExt != NULL && pszUsername !=NULL)
		{
			//All the components exist to create a new filename
			wcsncpy_s ( pszTempPath,MAX_PATH, pszPath, (pszFileNameExt-pszPath)); //Copy the first part of the path and filename
						//Construct the last part of the file

			*(pszTempPath+(pszFileNameExt-pszPath)+1) = L'\0'; //Add the nul termination
			

			wchar_t *pszFilePostFix = (wchar_t *) malloc(MAX_PATH*sizeof(wchar_t));
			
			unsigned int i = 1;
			for(i=1 ; i < UINT_MAX; i++)
			{
				int len = swprintf_s( pszFilePostFix, MAX_PATH, L"%s_(%s V%u)%s", pszTempPath,pszUsername,i,pszFileNameExt);
				if(len == MAX_PATH)
				{ //We have probably truncated this so fail
					i = UINT_MAX;
					break;
				}
				if (!PathFileExists(pszFilePostFix))
				{
					wchar_t *pszTempNewFile = (wchar_t *) malloc(wcslen (pszFileName)*sizeof(wchar_t));
					wcsncpy_s ( pszTempNewFile,wcslen (pszFileName), pszFileName, (pszFileNameExt-pszFileName)); //Copy the first part of the path and filename

					*(pszTempNewFile+(pszFileNameExt-pszFileName)+1) = L'\0'; //Add the nul termination
					size_t filenameSize = wcslen (pszFileName) + wcslen(pszUsername) + 100;
					pszNewFile = (wchar_t *) malloc(filenameSize*sizeof(wchar_t));
					swprintf_s( pszNewFile, filenameSize, L"%s_(%s V%u)%s", pszTempNewFile,pszUsername,i,pszFileNameExt);
					free(pszTempNewFile);
					pszTempNewFile=NULL;
					break;
				}
			}

			if (i < UINT_MAX)
			{
				if(CopyFile(pszPath,pszFilePostFix,FALSE) != 0)
				{
					pConflictResult = new CConflictResult(CONFLICT_RESOLUTION_RESOLVED,&pszPath,(LPCWSTR *)&pszFilePostFix);
				}

				// pszTempPath contains a valid path copy it to the return paramter
				//size_t PathSize =  (wcslen (pszNewFile) + 1) * sizeof(wchar_t);
				//*ppszNewName = (LPWSTR) CoTaskMemAlloc(PathSize);
				//memcpy(*ppszNewName,pszNewFile,PathSize);
				*pConflictResolution =OFFLINEFILES_SYNC_CONFLICT_RESOLVE_KEEPREMOTE;
			}
			else
			{
				*pConflictResolution = OFFLINEFILES_SYNC_CONFLICT_RESOLVE_LOG; //something has gone wrong so just log the conflict
			}

			free(pszFilePostFix);
			pszFilePostFix = NULL;
		}

		
		if (pszNewFile != NULL)
		{
			free(pszNewFile);
		}
		pszNewFile = NULL;

		if (pszTempPath != NULL)
		{
			free(pszTempPath);
		}
		pszTempPath = NULL;

		pszFileName = NULL;
		pszFileNameExt = NULL;
		
		if (pszUsername != NULL)
		{
			free(pszUsername);
		}
		pszUsername = NULL;
	}
	return S_OK;
}

BOOL CSyncConflictHandler::ConvertUsername(wchar_t **pUsername)
{
	if (m_szUsername == NULL)
	{
		return FALSE;
	}
	size_t UsernameLen = _tcslen(m_szUsername);
	*pUsername = (wchar_t *) malloc((UsernameLen +1) * sizeof(wchar_t));
	if (*pUsername == NULL)
	{
		return FALSE;
	}
#ifdef UNICODE
			// The username is defined as unicode /so we can jjust copy the bytes
			memcpy((void *)*pUsername,m_szUsername,((UsernameLen +1) * sizeof(wchar_t)));
#else
			//The username is defined as standard
		MultiByteToWideChar(CP_UTF8, 0,m_szUsername,-1,*pUsername,((UsernameLen +1));
#endif
		return TRUE;
}
HRESULT STDMETHODCALLTYPE CSyncConflictHandler::QueryInterface (REFIID   riid, LPVOID * ppvObj)
{
    // Always set out parameter to NULL, validating it first.
    if (!ppvObj)
	{
        return E_INVALIDARG;
	}
	*ppvObj = NULL;
    if (riid == IID_IUnknown || riid == IID_IOfflineFilesSyncConflictHandler)
    {
        // Increment the reference count and return the pointer.
        *ppvObj = (LPVOID)this;
        AddRef();
        return NOERROR;
    }
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CSyncConflictHandler::AddRef()
{
    InterlockedIncrement(&m_cRef);
    return m_cRef;
}
ULONG STDMETHODCALLTYPE CSyncConflictHandler::Release()
{
    // Decrement the object's internal counter.
    ULONG ulRefCount = InterlockedDecrement(&m_cRef);
    if (0 == m_cRef)
    {
        delete this;
    }
    return ulRefCount;
}