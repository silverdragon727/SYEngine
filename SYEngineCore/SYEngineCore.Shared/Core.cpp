#include "Core.h"

HStringReference _stmHandlerName(L"CoreMFSource.HDCoreByteStreamHandler");
HStringReference _urlHandlerName(L"MultipartStreamMatroska.UrlHandler");

using namespace SYEngineCore;

void Core::Initialize()
{
	if (_pMediaExtensionManager != nullptr)
		return;
	
	{
		auto p = ref new Windows::Media::MediaExtensionManager();
		_pMediaExtensionManager = reinterpret_cast<decltype(_pMediaExtensionManager.Get())>(p);
	}

	_pMediaExtensionManager->RegisterSchemeHandler(
		_urlHandlerName.Get(),
		HStringReference(L"plist:").Get());

	_pMediaExtensionManager->RegisterByteStreamHandler(
		_stmHandlerName.Get(),
		HStringReference(L".mka").Get(), HStringReference(L"video/force-network-stream").Get());

	_pMediaExtensionManager->RegisterByteStreamHandler(
		_stmHandlerName.Get(),
		HStringReference(L".flv").Get(), HStringReference(L"video/x-flv").Get());
	_pMediaExtensionManager->RegisterByteStreamHandler(
		_stmHandlerName.Get(),
		HStringReference(L".mkv").Get(), HStringReference(L"video/x-matroska").Get());
}