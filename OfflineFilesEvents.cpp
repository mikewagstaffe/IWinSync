#include "stdafx.h"
#include "OfflineFilesEvents.h"


COfflineFilesEvents::COfflineFilesEvents(void)
{
	m_dwRefCount =0;
}


COfflineFilesEvents::~COfflineFilesEvents(void)
{
}

HRESULT STDMETHODCALLTYPE COfflineFilesEvents::QueryInterface(REFIID iid, void **ppvObject)
{        
	if (iid == IID_IOfflineFilesEvents)
    {
		m_dwRefCount++;
        *ppvObject = (void *)this;
        return S_OK;
	}

    if (iid == IID_IUnknown)
    {
		m_dwRefCount++;            
        *ppvObject = (void *)this;
        return S_OK;
    }
	
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE COfflineFilesEvents::AddRef()
{
	m_dwRefCount++;
    return m_dwRefCount;
}
    
ULONG STDMETHODCALLTYPE COfflineFilesEvents::Release()
{
	ULONG l;
      
    l  = m_dwRefCount--;
	if ( 0 == m_dwRefCount)
    {
		delete this;
    }
        
    return l;
}

// End IUnknown Metehods

// IOfflineFileEvents Methods
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::CacheMoved( LPCWSTR pszOldPath,  LPCWSTR pszNewPath)
{
	printf("CacheMoved Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::CacheIsFull(void)
{
	printf("CacheIsFull Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::CacheIsCorrupted( void)
{
	printf("CacheIsCorrupted Event\r\n");
	return S_OK;
}
       
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::Enabled( BOOL bEnabled)
{
	printf("Enabled Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::EncryptionChanged( BOOL bWasEncrypted, BOOL bWasPartial, BOOL bIsEncrypted, BOOL bIsPartial)
{
	printf("EncryptionChanged Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::SyncBegin(REFGUID rSyncId)
{
	printf("SyncBegin Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::SyncFileResult(REFGUID rSyncId, LPCWSTR pszFile, HRESULT hrResult)
{
	printf("SyncFileResult Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::SyncConflictRecAdded( LPCWSTR pszConflictPath,  const FILETIME *pftConflictDateTime, OFFLINEFILES_SYNC_STATE ConflictSyncState)
{
	printf("SyncConflictRecAdded Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::SyncConflictRecUpdated( LPCWSTR pszConflictPath, const FILETIME *pftConflictDateTime, OFFLINEFILES_SYNC_STATE ConflictSyncState)
{
	printf("SyncConflictRecUpdated Event\r\n");
	return S_OK;
}
  
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::SyncConflictRecRemoved(  LPCWSTR pszConflictPath, const FILETIME *pftConflictDateTime,  OFFLINEFILES_SYNC_STATE ConflictSyncState)
{
	printf("SyncConflictRecRemoved Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::SyncEnd(  REFGUID rSyncId, HRESULT hrResult)
{
	printf("SyncEnd Event\r\n");
	return S_OK;
}

        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::NetTransportArrived( void)
{
	printf("NetTransportArrived Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::NoNetTransports( void)
{
	printf("NoNetTransports Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::ItemDisconnected( LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType)
{
	printf("ItemDisconnected Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::ItemReconnected( LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType)
{
	printf("ItemReconnected Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::ItemAvailableOffline( LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType)
{
	printf("ItemAvailableOffline Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::ItemNotAvailableOffline(  LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType)
{
	printf("ItemNotAvailableOffline Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::ItemPinned( LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType)
{
	printf("ItemPinned Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::ItemNotPinned(  LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType)
{
	printf("ItemNotPinned Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::ItemModified(  LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType, BOOL bModifiedData, BOOL bModifiedAttributes)
{
	printf("ItemModified Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::ItemAddedToCache( LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType)
{
	printf("ItemAddedToCache Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::ItemDeletedFromCache( LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType)
{
	printf("ItemDeletedFromCache Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::ItemRenamed( LPCWSTR pszOldPath, LPCWSTR pszNewPath, OFFLINEFILES_ITEM_TYPE ItemType)
{
	printf("ItemRenamed Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::DataLost( void)
{
	printf("DataLost Event\r\n");
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE COfflineFilesEvents::Ping( void)
{
	printf("Ping Event\r\n");
	return S_OK;
}

// End IOfflineFileEvents Methods

//IOfflineFileEventFilters Methods
/*virtual HRESULT STDMETHODCALLTYPE COfflineFilesEvents::GetPathFilter( LPWSTR *ppszFilter, OFFLINEFILES_PATHFILTER_MATCH *pMatch)
{
	return E_NOTIMPL;
}
        
virtual HRESULT STDMETHODCALLTYPE COfflineFilesEvents::GetIncludedEvents( ULONG cElements, OFFLINEFILES_EVENTS *prgEvents, ULONG *pcEvents)
{
	return E_NOTIMPL;
}
        
virtual HRESULT STDMETHODCALLTYPE COfflineFilesEvents::GetExcludedEvents( ULONG cElements, OFFLINEFILES_EVENTS *prgEvents, ULONG *pcEvents)
{
	return E_NOTIMPL;
}*/