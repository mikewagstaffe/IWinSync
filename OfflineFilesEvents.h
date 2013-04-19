#pragma once

#include <cscobj.h>

class COfflineFilesEvents : IOfflineFilesEvents//, IOfflineFilesEventsFilter
{
public:
	COfflineFilesEvents(void);
	~COfflineFilesEvents(void);

	// IUnknown Metehods
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	// End IUnknown Metehods

	// IOfflineFileEvents Methods
	virtual HRESULT STDMETHODCALLTYPE CacheMoved( LPCWSTR pszOldPath,  LPCWSTR pszNewPath);       
    virtual HRESULT STDMETHODCALLTYPE CacheIsFull(void);  
    virtual HRESULT STDMETHODCALLTYPE CacheIsCorrupted( void);
	virtual HRESULT STDMETHODCALLTYPE Enabled( BOOL bEnabled);
	virtual HRESULT STDMETHODCALLTYPE EncryptionChanged( BOOL bWasEncrypted, BOOL bWasPartial, BOOL bIsEncrypted, BOOL bIsPartial);
	virtual HRESULT STDMETHODCALLTYPE SyncBegin(REFGUID rSyncId);   
    virtual HRESULT STDMETHODCALLTYPE SyncFileResult(REFGUID rSyncId, LPCWSTR pszFile, HRESULT hrResult);
    virtual HRESULT STDMETHODCALLTYPE SyncConflictRecAdded( LPCWSTR pszConflictPath,  const FILETIME *pftConflictDateTime, OFFLINEFILES_SYNC_STATE ConflictSyncState);
    virtual HRESULT STDMETHODCALLTYPE SyncConflictRecRemoved(  LPCWSTR pszConflictPath, const FILETIME *pftConflictDateTime,  OFFLINEFILES_SYNC_STATE ConflictSyncState);
	virtual HRESULT STDMETHODCALLTYPE SyncConflictRecUpdated( LPCWSTR pszConflictPath, const FILETIME *pftConflictDateTime, OFFLINEFILES_SYNC_STATE ConflictSyncState);
    virtual HRESULT STDMETHODCALLTYPE SyncEnd(  REFGUID rSyncId, HRESULT hrResult); 
    virtual HRESULT STDMETHODCALLTYPE NetTransportArrived( void);
    virtual HRESULT STDMETHODCALLTYPE NoNetTransports( void);
	virtual HRESULT STDMETHODCALLTYPE ItemDisconnected( LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType);
	virtual HRESULT STDMETHODCALLTYPE ItemReconnected( LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType);
	virtual HRESULT STDMETHODCALLTYPE ItemAvailableOffline( LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType);
	virtual HRESULT STDMETHODCALLTYPE ItemNotAvailableOffline(  LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType);
	virtual HRESULT STDMETHODCALLTYPE ItemPinned( LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType);
	virtual HRESULT STDMETHODCALLTYPE ItemNotPinned(  LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType);
	virtual HRESULT STDMETHODCALLTYPE ItemModified(  LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType, BOOL bModifiedData, BOOL bModifiedAttributes);
	virtual HRESULT STDMETHODCALLTYPE ItemAddedToCache( LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType);
    virtual HRESULT STDMETHODCALLTYPE ItemDeletedFromCache( LPCWSTR pszPath, OFFLINEFILES_ITEM_TYPE ItemType);
    virtual HRESULT STDMETHODCALLTYPE ItemRenamed( LPCWSTR pszOldPath, LPCWSTR pszNewPath, OFFLINEFILES_ITEM_TYPE ItemType);
    virtual HRESULT STDMETHODCALLTYPE DataLost( void);
	virtual HRESULT STDMETHODCALLTYPE Ping( void);

	
	
	// End IOfflineFileEvents Methods

	//IOfflineFileEventFilters Methods
	/*virtual HRESULT STDMETHODCALLTYPE GetPathFilter( LPWSTR *ppszFilter, OFFLINEFILES_PATHFILTER_MATCH *pMatch); 
	virtual HRESULT STDMETHODCALLTYPE GetIncludedEvents( ULONG cElements, OFFLINEFILES_EVENTS *prgEvents, ULONG *pcEvents);
	virtual HRESULT STDMETHODCALLTYPE GetExcludedEvents( ULONG cElements, OFFLINEFILES_EVENTS *prgEvents, ULONG *pcEvents);
	*/

	private:
		DWORD m_dwRefCount;
};

