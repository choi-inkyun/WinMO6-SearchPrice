#ifndef __STATUSCALLBACK_H__
#define __STATUSCALLBACK_H__

#include <windows.h>
#include <urlmon.h>
#include <string>

//////////////////////////////////////////////////////////////////////////////
/// \class CCallback
/// \brief 인터넷 익스플로러를 이용해 HTTP로 파일을 다운로드받기 위한 
/// COM 인터페이스 클래스.
//////////////////////////////////////////////////////////////////////////////

class CCallback : public IBindStatusCallback
{
private:
	std::wstring m_URL;     ///< 다운로드받을 파일의 URL
	DWORD       m_Timeout; ///< 타임아웃

public:
	/// \brief 생성자
	CCallback(const std::wstring& url, DWORD timeout=INFINITE);

	/// \brief 소멸자
	~CCallback();

public:
	STDMETHOD(OnStartBinding)(
		/* [in] */ DWORD dwReserved,
		/* [in] */ IBinding __RPC_FAR *pib)
	{ return E_NOTIMPL; }

	STDMETHOD(GetPriority)(
		/* [out] */ LONG __RPC_FAR *pnPriority)
	{ return E_NOTIMPL; }

	STDMETHOD(OnLowResource)(	
		/* [in] */ DWORD reserved)
	{ return E_NOTIMPL; }

	STDMETHOD(OnProgress)(
		/* [in] */ ULONG ulProgress,
		/* [in] */ ULONG ulProgressMax,
		/* [in] */ ULONG ulStatusCode,
		/* [in] */ LPCWSTR wszStatusText);

	STDMETHOD(OnStopBinding)(
		/* [in] */ HRESULT hresult,
		/* [unique][in] */ LPCWSTR szError)
	{ return E_NOTIMPL; }

	STDMETHOD(GetBindInfo)(
		/* [out] */ DWORD __RPC_FAR *grfBINDF,
		/* [unique][out][in] */ BINDINFO __RPC_FAR *pbindinfo)
	{ return E_NOTIMPL; }

	STDMETHOD(OnDataAvailable)(
		/* [in] */ DWORD grfBSCF,
		/* [in] */ DWORD dwSize,
		/* [in] */ FORMATETC __RPC_FAR *pformatetc,
		/* [in] */ STGMEDIUM __RPC_FAR *pstgmed)
	{ return E_NOTIMPL; }

	STDMETHOD(OnObjectAvailable)(
		/* [in] */ REFIID riid,
		/* [iid_is][in] */ IUnknown __RPC_FAR *punk)
	{ return E_NOTIMPL; }

	STDMETHOD_(ULONG, AddRef)() { return 0; }
	STDMETHOD_(ULONG, Release)() { return 0; }

	STDMETHOD(QueryInterface)(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
	{ return E_NOTIMPL; }
};

#endif //__BINDSTATUSCALLBACK_H__

