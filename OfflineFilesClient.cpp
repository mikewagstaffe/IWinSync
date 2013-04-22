#include "stdafx.h"
#include "OfflineFilesClient.h"

#define SYNC_OPTIONS OFFLINEFILES_SYNC_CONTROL_FLAG_SYNCIN \
					+ OFFLINEFILES_SYNC_CONTROL_FLAG_SYNCOUT \
					+ OFFLINEFILES_SYNC_CONTROL_FLAG_ASYNCPROGRESS \
					+ OFFLINEFILES_SYNC_CONTROL_FLAG_PINNEWFILES \
					+ OFFLINEFILES_SYNC_CONTROL_FLAG_PINFORUSER

COfflineFilesClient::COfflineFilesClient(UINT const uWMSyncComplete, UINT const uWMSyncConflict, HWND hwndApplication)
{
	m_bIsOfflineFilesActive = FALSE;
	m_bIsOfflineFilesEnabled = FALSE;
	m_bStartupReebotRequired = FALSE;
	m_bCacheable=FALSE;
	m_pOfflineFilesCache = NULL;
	m_pOfflineFilesItem = NULL;
	m_pOfflineFilesConnecionInfo = NULL;
	m_ShareCachingMode = OFFLINEFILES_CACHING_MODE_NONE;
	m_pIConnectionPoint = NULL;
	m_pOfflineFilesEvents = NULL;
	m_pSinkUnk = NULL;
	m_dwAdvise = 0;
	m_pszCachePath = NULL;
	m_bOfflineOnlineTranistionRequired = FALSE;

	//Save the main application signaling data
	 m_uWMSyncComplete = uWMSyncComplete;
	 m_uWMSyncConflict = uWMSyncConflict;
	 m_hwndApplication = hwndApplication; 
}


COfflineFilesClient::~COfflineFilesClient(void)
{
	Cleanup();
}

BOOL COfflineFilesClient::Init(void)
{
	HRESULT hr = CoInitializeEx(NULL,COINITBASE_MULTITHREADED); 
	if (hr != S_OK)
	{
		
		LOG(G2L_WARNING) << "Failed To Initialise Windows COM"; 
	}
	else //Co Initialise Succeded
	{
		LOG(G2L_DEBUG) << "COM Is Initialised";
		LOG(G2L_DEBUG) << "Getting status Of Offline Files";
		if ( OfflineFilesQueryStatus(&m_bIsOfflineFilesActive,&m_bIsOfflineFilesEnabled) != ERROR_SUCCESS)
		{
			LOG(G2L_WARNING) << "Failed To Get Offline Files Status :" << GetLastError();
		}
		else //Obtained the current status of Offline Files
		{
			LOG(G2L_DEBUG) << "Offline Files Is Active: " << (m_bIsOfflineFilesActive ? "TRUE" : "FALSE");
			LOG(G2L_DEBUG) << "Offline Files Is Enabled: " << (m_bIsOfflineFilesEnabled ? "TRUE" : "FALSE");

			if (!m_bIsOfflineFilesEnabled) //If not enabled Enable Offline Files try and enable it
			{
				DWORD dwResult = OfflineFilesEnable(TRUE, &m_bStartupReebotRequired);
				if (dwResult == ERROR_SUCCESS)
				{
					LOG(G2L_DEBUG) << "Offline Files Is Now Enabled, Is Reeboot required: " << (m_bStartupReebotRequired ? "TRUE" : "FALSE");
					if (!m_bStartupReebotRequired)
					{
						m_bIsOfflineFilesEnabled = TRUE;
					}
				}
				else //Failed To Enable Offline Files
				{
					LOG(G2L_WARNING) << "Failed To Enable Offline Files: " << dwResult;
				}
			}

			//If Offline Files is enabled but not Started start it up as long as we are not pending a reboot
			if (!m_bIsOfflineFilesActive && m_bIsOfflineFilesEnabled && !m_bStartupReebotRequired)
			{
				//Windows 7 Does not support starting of Offline Files
				LOG(G2L_INFO) << "Failed To Start Offline Files";
			}
		}
	}

	//Create an instance of the Offline Files Cache
	if( m_bIsOfflineFilesEnabled && m_bIsOfflineFilesActive)
	{
		LOG(G2L_DEBUG) << "Creating Offline Files Cache Instance";
		hr = CoCreateInstance(CLSID_OfflineFilesCache, NULL, CLSCTX_INPROC_SERVER, IID_IOfflineFilesCache, (void**) &m_pOfflineFilesCache); 
		if ( FAILED(hr) )
		{
			LOG(G2L_WARNING) << "Failed To Create offline Cache Object: " << hr;
			m_pOfflineFilesCache = NULL;
		}
	}

	if ( m_pOfflineFilesCache == NULL )
	{
		LOG(G2L_DEBUG) << "m_pOfflineFilesCache == NULL";
		CoUninitialize();
		return (FALSE);
	}

	LOG(G2L_DEBUG) << "Created OfflineFilesCache Instance";
	return (TRUE);
	
}

OFFLINEFILESCLIENT_INIT_STATE COfflineFilesClient::GetInitialiseState(void)
{
	if (!m_bIsOfflineFilesEnabled)
	{
		if (m_bStartupReebotRequired)
		{
			return OFFLINEFILESCLIENT_INIT_STATE_RESTART_PENDING;
		}
		return OFFLINEFILESCLIENT_INIT_STATE_NOT_ENABLED;
	}
	else
	{
		if (m_bIsOfflineFilesActive)
		{
			return OFFLINEFILESCLIENT_INIT_STATE_STARTED;
		}
		return OFFLINEFILESCLIENT_INIT_STATE_STARTED;
	}
}

BOOL COfflineFilesClient::InitCache(LPCWSTR *ppszCachePath)
{
	if (m_pOfflineFilesCache == NULL)
	{
		// The Cache Has Not been initialised
		LOG(G2L_DEBUG) << "InitCache Called While m_pOfflineFilesCache == NULL";
		return (FALSE);
	}

	//Save the path as a member variable
	size_t PathSize =  (wcslen (*ppszCachePath) + 1) * sizeof(wchar_t);
	m_pszCachePath = (LPCWSTR) malloc(PathSize);
	if (m_pszCachePath == NULL) 
	{
		LOG(G2L_WARNING) << "Failed To allocate Memory For Path Member";
		return (FALSE);
	}
	
	memcpy((void *)m_pszCachePath,*ppszCachePath,PathSize);
	
	//Print out data about Offline Files
	PrintLocation();
	PrintEncryptionStatus();
	PrintDiskSpaceInfo();
	PrintSettings();

	//Get The status of the Cache
	if ( !CacheStatus())
	{
		LOG(G2L_WARNING) << "The specified folder is not cachable";
		return (FALSE);
	}

	//Get The Current State Of The Cache
	m_pOfflineFilesCache->FindItem(m_pszCachePath,0,(IOfflineFilesItem**)&m_pOfflineFilesItem);
	if(m_pOfflineFilesItem != NULL)
	{
		m_pOfflineFilesItem->QueryInterface(IID_IOfflineFilesConnectionInfo,(void**)&m_pOfflineFilesConnecionInfo);
		if (m_pOfflineFilesConnecionInfo == NULL)
		{
			LOG(G2L_WARNING) << "Cannot Determine The State of path. ";
			return (FALSE);
		}
		m_pOfflineFilesConnecionInfo->GetConnectState(&m_CurentConnectState,&m_OfflineReason);
	}
	else
	{
		LOG(G2L_WARNING) << "Failed to get Interface To Sync Item";
		return(FALSE);
	}
	return (TRUE);
}

void COfflineFilesClient::PrintLocation(void)
{
	if (m_pOfflineFilesCache == NULL)
	{
		// The Cache Has Not been initialised
		return;
	}

	LOG(G2L_INFO) << "Getting Cache Location";
	LPWSTR szCacheLocation;
	if ( m_pOfflineFilesCache->GetLocation(&szCacheLocation) == S_OK)
	{
		LOG(G2L_INFO) << "Location Is:" << szCacheLocation;
	}
}

void COfflineFilesClient::PrintEncryptionStatus(void)
{
	if (m_pOfflineFilesCache == NULL)
	{
		// The Cache Has Not been initialised
		return;
	}

	//Log The Encryption Status
	LOG(G2L_INFO) << "Getting Cache Encryption Status";
	BOOL bEncryptionStatus = FALSE;
	BOOL bPartialEncryption = FALSE;
	if ( m_pOfflineFilesCache ->GetEncryptionStatus(&bEncryptionStatus,&bPartialEncryption) == S_OK)
	{
		LOG(G2L_INFO) << "Offline Files Encryption status: " << (bEncryptionStatus ? "Encrypted" : "UnEncrypted");
		LOG(G2L_INFO) << "Offline Files is Partialy Encrypted: " << (bPartialEncryption ? "True" : "False");
	}
}

void COfflineFilesClient::PrintDiskSpaceInfo(void)
{
	if (m_pOfflineFilesCache == NULL)
	{
		// The Cache Has Not been initialised
		return;
	}

	LOG(G2L_INFO) << "Getting Cache Disckpace Information";
	ULONGLONG cbVolumeTotal =0;
	ULONGLONG cbLimit =0;
	ULONGLONG cbUsed =0;
	ULONGLONG cbUnpinnedLimit=0;
	ULONGLONG cbUnpinnedUsed=0;
		
	if ( m_pOfflineFilesCache->GetDiskSpaceInformation(&cbVolumeTotal, &cbLimit, &cbUsed, &cbUnpinnedLimit, &cbUnpinnedUsed)  == S_OK)
	{
		LOGF(G2L_INFO,"Volume Size is: %ull bytes\r\n",cbVolumeTotal);
		LOGF(G2L_INFO,"Maximum Cache Size is: %ull bytes\r\n",cbLimit);
		LOGF(G2L_INFO,"Current Cache Size is: %ull bytes\r\n",cbUsed);
		LOGF(G2L_INFO,"Unpinned Cache Limit is: %ull bytes \r\n",cbUnpinnedLimit);
		LOGF(G2L_INFO,"Current Unpinned Cache Size is: %ull bytes\r\n",cbUnpinnedUsed);
	}
}

BOOL COfflineFilesClient::CacheStatus(void)
{
	if (m_pOfflineFilesCache == NULL)
	{
		// The Cache Has Not been initialised
		return(FALSE);
	}

	LOG(G2L_INFO) << "Getting Cache Path Status";
	
	if ( m_pOfflineFilesCache->IsPathCacheable(m_pszCachePath, &m_bCacheable,&m_ShareCachingMode) == S_OK)
	{
		LOG(G2L_INFO) << "Path Is Cachable: " << (m_bCacheable ? "True" : "False");
		LOG(G2L_INFO) << "Path Caching Mode is: " << m_ShareCachingMode;
	}
	return m_bCacheable;
}

void COfflineFilesClient::PrintSettings(void)
{
	if (m_pOfflineFilesCache == NULL)
	{
		// The Cache Has Not been initialised
		return;
	}

	//Get Setting Information
	LOG(G2L_INFO) <<  "Getting Settings For Offline Files";
	IEnumOfflineFilesSettings *pEnum = NULL;
	if (m_pOfflineFilesCache->EnumSettingObjects(&pEnum) == S_OK)
	{
		if ( pEnum !=NULL)
		{
			IOfflineFilesSetting *rgelt = NULL;
			ULONG celtFetched = 0;
			VARIANT varValue;
			BOOL bSetByPolicy;
			VariantInit(&varValue);

			while(pEnum->Next(1,&rgelt,&celtFetched)!=S_FALSE)
			{
				LPWSTR pszName = NULL;
				OFFLINEFILES_SETTING_VALUE_TYPE Type = OFFLINEFILES_SETTING_VALUE_UI4;
				rgelt->GetValueType(&Type);
				rgelt->GetName(&pszName);
				rgelt->GetValue(&varValue,&bSetByPolicy);
				if(Type == OFFLINEFILES_SETTING_VALUE_UI4)
				{
					LOG(G2L_INFO) << "Setting Name: " << pszName << " Value: " << varValue.intVal <<" Set By Policy:" <<(bSetByPolicy ? L"True" : L"False");
				}
				else
				{
					LOG(G2L_INFO) <<"Setting Name: " << pszName << " Value:Not Printable Set By Policy:" << (bSetByPolicy?  L"True" : L"False");
				}
			}
		}
	}
	else
	{
		LOG(G2L_INFO) << "Failed getting settings";
	}
	pEnum->Release();
	pEnum = NULL;
}


BOOL COfflineFilesClient::RegisterSyncEvents() 
{
	if (m_pOfflineFilesCache == NULL)
	{
		// The Cache Has Not been initialised
		return (FALSE);
	}

	IConnectionPointContainer * pIConnectionPointContainerTemp = NULL;
	m_pOfflineFilesCache -> QueryInterface (IID_IConnectionPointContainer,  (void**)&pIConnectionPointContainerTemp);
	if (pIConnectionPointContainerTemp)
	{
		printf("Interface supports connection points\r\n");
		HRESULT hr = pIConnectionPointContainerTemp -> FindConnectionPoint(IID_IOfflineFilesEvents, &m_pIConnectionPoint);
		if(FAILED(hr))
		{
			printf("failedTo Find connection point\r\n");
			m_pIConnectionPoint = NULL;
		}
		else
		{
			printf("found connection point\r\n");
		}
		pIConnectionPointContainerTemp -> Release();
		pIConnectionPointContainerTemp = NULL;
	}

	if ( m_pIConnectionPoint == NULL )
	{
		return (FALSE);
	}

	m_pOfflineFilesEvents = new COfflineFilesEvents;
		
	if (m_pOfflineFilesEvents == NULL)
	{
		printf("failed to create connection point\r\n");
	}

	BOOL bEventsRegistered = FALSE;
	if ( m_pIConnectionPoint != NULL && m_pOfflineFilesEvents != NULL)
	{
		HRESULT hr = m_pOfflineFilesEvents->QueryInterface (IID_IUnknown,(void **)&m_pSinkUnk);
		if ( hr == S_OK)
		{
			hr = m_pIConnectionPoint->Advise(m_pSinkUnk,&m_dwAdvise);
			if (hr == S_OK)
			{
				bEventsRegistered = TRUE;
			}
		}
		else
		{
			printf("failed Get Unknown Interface ID\r\n");
		}
		if (!bEventsRegistered && m_pSinkUnk!=NULL)
		{
			m_pSinkUnk->Release();
			m_pSinkUnk = NULL;
		}

		if (!bEventsRegistered && m_pIConnectionPoint!=NULL)
		{
			m_pIConnectionPoint->Release();
			m_pIConnectionPoint = NULL;
		}
		if (!bEventsRegistered && m_pOfflineFilesEvents!=NULL)
		{
			m_pOfflineFilesEvents->Release();
			m_pOfflineFilesEvents=NULL;
		}

		if(bEventsRegistered)
		{
			return (TRUE);
		}
	}
	return (FALSE);
}

void COfflineFilesClient::Cleanup(void)
{
	if ( m_pszCachePath != NULL)
	{
		free((void *)m_pszCachePath);
		m_pszCachePath = NULL;
	}
	
	
	if (m_pIConnectionPoint != NULL)
	{
		m_pIConnectionPoint->Unadvise(m_dwAdvise);
		m_pIConnectionPoint->Release();
		m_pIConnectionPoint = NULL;
	}
	
	if (m_pOfflineFilesEvents  != NULL)
	{
		m_pOfflineFilesEvents->Release();
		m_pOfflineFilesEvents=NULL;
	}

	if (m_pSinkUnk!=NULL)
	{
		m_pSinkUnk->Release();
		m_pSinkUnk=NULL;
	}

	if (m_pOfflineFilesCache != NULL)
	{
		m_pOfflineFilesCache->Release();
		m_pOfflineFilesCache = NULL;
	}

	CoUninitialize();
}

BOOL COfflineFilesClient::Synchronise()
{
	LOG(G2L_DEBUG) << "sync called";
	if (m_pOfflineFilesCache == NULL)
	{
		LOG(G2L_WARNING) << "Synchronise Called whilst m_pOfflineFilesCache == NULL";
		// The Cache Has Not been initialised
		SendMessage(m_hwndApplication,m_uWMSyncComplete,1,NULL);
		return (FALSE);
	}
	CSyncConflictHandler *pOfflineFilesConflictHandler = new CSyncConflictHandler(m_uWMSyncConflict,m_hwndApplication);
	CSyncProgressHandler *pOfflineFilesProgress = new CSyncProgressHandler;
	pOfflineFilesProgress->m_pCurentConnectState = &m_CurentConnectState;
	pOfflineFilesProgress->m_pbOfflineOnlineTranistionReq = &m_bOfflineOnlineTranistionRequired;

	//Get the current Logged in user - used for renaming files on conflict
	DWORD  bufCharCount = UNLEN;
	if(0 == GetUserName(pOfflineFilesConflictHandler->m_szUsername, &bufCharCount))
	{
		//If we did not get the user name then set it to user
		_tcscpy_s(pOfflineFilesConflictHandler->m_szUsername,TEXT("USER"));
	}
	LOG(G2L_DEBUG) << "starting sync";
	HRESULT hrResult = m_pOfflineFilesCache->Synchronize( NULL,		//No Window Handle
								&m_pszCachePath,					//The path of our folder
								1,									//1 folder in the list
								FALSE,								//Run Synchronously
								SYNC_OPTIONS,						//Sync Options
								pOfflineFilesConflictHandler,		//Conflict Handler
								pOfflineFilesProgress,				//Progress Handler
								NULL);								//Sync ID
								
								
	LOG(G2L_DEBUG) << "sync returned: " << hrResult;

	if (m_bOfflineOnlineTranistionRequired)
	{
		m_bOfflineOnlineTranistionRequired = FALSE;
		LOG(G2L_DEBUG) << "Need to run an Offline to online tranisition";

		//The synchronise function indicates we need to tranistion to online
		BOOL bTranistioncompleted = FALSE;
		if (m_CurentConnectState == OFFLINEFILES_CONNECT_STATE_OFFLINE && 
			(m_OfflineReason==OFFLINEFILES_OFFLINE_REASON_CONNECTION_FORCED || m_OfflineReason==OFFLINEFILES_OFFLINE_REASON_CONNECTION_SLOW )) //Check that we are offline and will be able to tranisition online
		{
			if(S_OK != m_pOfflineFilesConnecionInfo->TransitionOnline(NULL,0x0))
			{
				LOG(G2L_WARNING) << "Failed To Go Online";
			}
			
			//update the connection state
			m_pOfflineFilesConnecionInfo->GetConnectState(&m_CurentConnectState,&m_OfflineReason);
			
			if (m_CurentConnectState==OFFLINEFILES_CONNECT_STATE_ONLINE) //online so wait and then return to offline mode
			{
				Sleep(60000); //Wait 60 seconds to fetch all changes
				
			}
			BOOL bOpenFilesPreventedTransition = FALSE;
			if(S_OK != m_pOfflineFilesConnecionInfo->TransitionOffline(NULL,0x0,FALSE,&bOpenFilesPreventedTransition))
			{
				LOG(G2L_WARNING) << "Failed To Go Offline";
				if (bOpenFilesPreventedTransition)
				{
					LOG(G2L_WARNING) << "Failed because a file is open";
					Sleep(60000); //Wait another 60 seconds to fetch all changes
					if(S_OK != m_pOfflineFilesConnecionInfo->TransitionOffline(NULL,0x0,TRUE,&bOpenFilesPreventedTransition)) //This time forcibly close any connections
					{
						LOG(G2L_WARNING) << "Failed To Go Offline";
					}
				}
			}
			//Refresh the current connection state
			m_pOfflineFilesConnecionInfo->GetConnectState(&m_CurentConnectState,&m_OfflineReason);
			if(bTranistioncompleted && m_CurentConnectState!=OFFLINEFILES_CONNECT_STATE_OFFLINE)
			{
				//somthing went wrong we completed a tranistion but are not offline
				LOG(G2L_WARNING) << "Unknown Transition Error: Not Offline";
			}
		}		
	}
	SendMessage(m_hwndApplication,m_uWMSyncComplete,0,hrResult);
	return true;
}
	
