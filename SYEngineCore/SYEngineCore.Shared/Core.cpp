#include "Core.h"
#include "AutoLib.h"

#include <mfapi.h>

using namespace SYEngineCore;

HStringReference _stmHandlerName(L"CoreMFSource.HDCoreByteStreamHandler");
HStringReference _urlHandlerName(L"MultipartStreamMatroska.UrlHandler");

static HMODULE _hCoreMFSource = NULL;
static HMODULE _hMultipartStreamMatroska = NULL;

class MFWinRTActivate WrlSealed :
	public Microsoft::WRL::RuntimeClass<
	Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::ClassicCom>,
	IMFActivate>
{
public:
	virtual HRESULT STDMETHODCALLTYPE ActivateObject(REFIID riid, void **ppv) override
	{
		ComPtr<IActivationFactory> activationFactory;
		HRESULT hr = m_pfnGetActivationFactory(m_activatableClassId, &activationFactory);

		if (SUCCEEDED(hr))
		{
			hr = activationFactory->ActivateInstance(&m_instance);
		}

		if (SUCCEEDED(hr))
		{
			*ppv = (void*)static_cast<IUnknown*>(m_instance.Get());
		}

		return hr;
	}

	virtual HRESULT STDMETHODCALLTYPE ShutdownObject(void) override
	{
		m_instance.Reset();
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE DetachObject(void) override
	{
		m_instance.Detach();
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetItem(REFGUID guidKey, PROPVARIANT *pValue) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetItemType(REFGUID guidKey, MF_ATTRIBUTE_TYPE *pType) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE CompareItem(REFGUID guidKey,
												  REFPROPVARIANT Value, BOOL *pbResult) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Compare(IMFAttributes *pTheirs,
											  MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL *pbResult) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetUINT32(REFGUID guidKey, UINT32 *punValue) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetUINT64(REFGUID guidKey, UINT64 *punValue) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetDouble(REFGUID guidKey, double *pfValue) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetGUID(REFGUID guidKey, GUID *pguidValue) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetStringLength(REFGUID guidKey, UINT32 *pcchLength) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetString(REFGUID guidKey, LPWSTR pwszValue,
												UINT32 cchBufSize, UINT32 *pcchLength) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetAllocatedString(REFGUID guidKey,
														 LPWSTR *ppwszValue, UINT32 *pcchLength) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetBlobSize(REFGUID guidKey, UINT32 *pcbBlobSize) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetBlob(REFGUID guidKey, UINT8 *pBuf,
											  UINT32 cbBufSize, UINT32 *pcbBlobSize) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetAllocatedBlob(REFGUID guidKey,
													   UINT8 **ppBuf, UINT32 *pcbSize) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetUnknown(REFGUID guidKey, REFIID riid, LPVOID *ppv) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetItem(REFGUID guidKey, REFPROPVARIANT Value) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE DeleteItem(REFGUID guidKey) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE DeleteAllItems(void) override { return S_OK; }

	virtual HRESULT STDMETHODCALLTYPE SetUINT32(REFGUID guidKey, UINT32 unValue) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetUINT64(REFGUID guidKey, UINT64 unValue) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetDouble(REFGUID guidKey, double fValue) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetGUID(REFGUID guidKey, REFGUID guidValue) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetString(REFGUID guidKey, LPCWSTR wszValue) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetBlob(REFGUID guidKey, 
											  const UINT8 *pBuf, UINT32 cbBufSize) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetUnknown(REFGUID guidKey, IUnknown *pUnknown) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE LockStore(void) override { return S_OK; }

	virtual HRESULT STDMETHODCALLTYPE UnlockStore(void) override { return S_OK; }

	virtual HRESULT STDMETHODCALLTYPE GetCount(UINT32 *pcItems) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetItemByIndex(UINT32 unIndex,
													 GUID *pguidKey, PROPVARIANT *pValue) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE CopyAllItems(IMFAttributes *pDest)
	{
		return S_OK;
	}

public:
	HRESULT RuntimeClassInitialize(HSTRING activatableClassId,
								   PFNGETACTIVATIONFACTORY pfnGetActivationFactory)
	{
		m_activatableClassId = activatableClassId;
		m_pfnGetActivationFactory = pfnGetActivationFactory;
		return S_OK;
	}

private:
	HSTRING m_activatableClassId;
	PFNGETACTIVATIONFACTORY m_pfnGetActivationFactory;

	ComPtr<IInspectable> m_instance;
};

void Core::Initialize()
{
	MFStartup(MF_API_VERSION);

	AutoLibrary mfPlat("MFPlat.dll");

	auto _MFRegisterLocalSchemeHandler =
		mfPlat.GetProcAddr<HRESULT(WINAPI*)(PCWSTR, IMFActivate *)>("MFRegisterLocalSchemeHandler");
	auto _MFRegisterLocalByteStreamHandler =
		mfPlat.GetProcAddr<HRESULT(WINAPI*)(PCWSTR, PCWSTR, IMFActivate *)>("MFRegisterLocalByteStreamHandler");

	_hCoreMFSource = LoadPackagedLibrary(L"CoreMFSource.dll", NULL);
	_hMultipartStreamMatroska = LoadPackagedLibrary(L"MultipartStreamMatroska.dll", NULL);

	ComPtr<MFWinRTActivate> schemeHandler;
	MakeAndInitialize<MFWinRTActivate>(&schemeHandler, _urlHandlerName.Get(),
									   (PFNGETACTIVATIONFACTORY) GetProcAddress(_hMultipartStreamMatroska, "DllGetActivationFactory"));

	ComPtr<MFWinRTActivate> byteStreamHandler;
	MakeAndInitialize<MFWinRTActivate>(&byteStreamHandler, _stmHandlerName.Get(),
									   (PFNGETACTIVATIONFACTORY) GetProcAddress(_hCoreMFSource, "DllGetActivationFactory"));

	_MFRegisterLocalSchemeHandler(L"plist:", schemeHandler.Get());

	_MFRegisterLocalByteStreamHandler(L".mka", L"video/force-network-stream", byteStreamHandler.Get());
	_MFRegisterLocalByteStreamHandler(L".flv", L"video/x-flv", byteStreamHandler.Get());
	_MFRegisterLocalByteStreamHandler(L".mkv", L"video/x-matroska", byteStreamHandler.Get());

	atexit([] { if (_hCoreMFSource) MFShutdown(); });
}

void Core::Uninitialize()
{
	MFShutdown();

	if (_hCoreMFSource)
		FreeLibrary(_hCoreMFSource);

	if (_hMultipartStreamMatroska)
		FreeLibrary(_hMultipartStreamMatroska);
}