
#include "../Public/Graphic_Device.h"

IMPLEMENT_SINGLETON(CGraphic_Device)

CGraphic_Device::CGraphic_Device()
	: m_p3D(nullptr) , m_pDevice(nullptr), m_pSprite(nullptr), m_pFont(nullptr)
{

}

HRESULT CGraphic_Device::InitDevice(const GRAPHIC_DESC& GraphicDesc, LPDIRECT3DDEVICE9* ppOut)
{
	m_GraphicDesc = GraphicDesc;

	// HRESULT : 함수가 호출 이후 문제가 없이 종료될 경우 양수값을
	// 문제가 있는 상태로 종료될 경우 음수값을 반환시키기 위한 자료형

	// (그리기)장치 초기화 : 장치를 조사할 객체 생성(m_p3D) -> 장치 조사 -> 장치 제어 객체 생성(m_pDevice)

	// Direct3DCreate9 : LPDIRECT3D9 객체를 SDK 버전에 맞게 생성하는 함수
	m_p3D = Direct3DCreate9(D3D_SDK_VERSION);

	D3DCAPS9			DeviceCaps;
	// 3D렌더링에 필요한 기본적인 연산을 지원하는지 조사하여 각종 정보를 보관하기 위한 구조체
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	// FAILED 매크로 :hresult 반환타입이 음수일 경우 true
	// GetDeviceCaps : 장치 성능을 조사
	// D3DADAPTER_DEFAULT :정보를 얻으려는 기본 그래픽 카드를 의미
	// D3DDEVTYPE_HAL : HAL(hardware abstraction layer)에 저장되어 있는 장치의 필수 정보를 얻어오기 위해 넣어주는 인자값
	if (FAILED(m_p3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps)))
	{
		ERR_MSG(L"GetDeviceCaps Failed"); // mfc에서만 사용가능한 메세지 박스 함수
		return E_FAIL;
	}

	DWORD	vp = 0;		// 버텍스 프로세싱 : 정점 연산 + 조명 처리

	// HAL 을 통해 조사한 장치가 하드웨어 버텍스 프로세싱을 지원하는지 판단
	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;		// 그래픽 카드

	else
		vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;		// 직접 연산(CPU)

	D3DPRESENT_PARAMETERS		d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	SetParameters(d3dpp);

	if (FAILED(m_p3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_GraphicDesc.hWnd, vp, &d3dpp, &m_pDevice)))
	{
		ERR_MSG(L"CreateDevice Failed"); // mfc에서만 사용가능한 메세지 박스 함수
		return E_FAIL;
	}

	if (FAILED(D3DXCreateSprite(m_pDevice, &m_pSprite)))
	{
		ERR_MSG(L"m_pSprite Failed");
		return E_FAIL;
	}

	D3DXFONT_DESCW	tFontInfo;
	ZeroMemory(&tFontInfo, sizeof(D3DXFONT_DESCW));

	tFontInfo.Height = 20;
	tFontInfo.Width = 10;
	tFontInfo.Weight = FW_HEAVY;
	tFontInfo.CharSet = HANGEUL_CHARSET;
	lstrcpy(tFontInfo.FaceName, L"궁서");
	
	if (FAILED(D3DXCreateFontIndirect(m_pDevice, &tFontInfo, &m_pFont)))
	{
		ERR_MSG(L"m_pFont Failed");
		return E_FAIL;
	}

	*ppOut = m_pDevice;

	Safe_AddRef(m_pDevice);

	return S_OK;
}

void CGraphic_Device::SetParameters(D3DPRESENT_PARAMETERS & d3dpp)
{
	d3dpp.BackBufferWidth = m_GraphicDesc.iWinSizeX;
	d3dpp.BackBufferHeight = m_GraphicDesc.iWinSizeY;

	// 후면 버퍼의 픽셀 포맷 , 32비트 포맷 사용
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	
	// 후면 버퍼의 개수
	d3dpp.BackBufferCount = 1;

	// 픽셀이 모여 발생하는 계단현상을 뭉개서 부드럽게 연출하는 기법(안티 엘리어싱)을 지원하는지 여부를 묻는 인자값
	// 자연스럽게 연출은 되지만 성능저하가 발생하기 때문에 사용하지 않을 예정
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	// 복사가 아닌 교체 방식을 통해 그리기와 지우기를 하게끔 만들어준다.
	
	// D3DSWAPEFFECT_DISCARD : 스왑 체인 방식
	// D3DSWAPEFFECT_FLIP : 버퍼 하나를 뒤집으면서 사용하는 방식
	// D3DSWAPEFFECT_COPY : 더블 버퍼링과 유사하게 복사하여 사용하는 방식

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = m_GraphicDesc.hWnd;	// 장치를 사용할 윈도우 핸들 설정
	
	d3dpp.Windowed = m_GraphicDesc.eWinMode; // TRUE인 경우 창모드, FALSE 전체 화면 모드

	// 스텐실 버퍼 : 반사와 같은 물체를 표현하는 버퍼
	// 깊이 버퍼(Z버퍼) : 물체의 깊이 값을 저장하여 원근감을 표현하기 위한 버퍼
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	// 전체화면 시에 사용하는 인자값
	// 창 모드 일 경우 장치가 아닌 운영체제가 재생률을 관리함
	// 전체 화면으로 변경 시 모니터 재생율에 대한 설정
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	
	// 재생율과 시연의 간격 설정
	// D3DPRESENT_INTERVAL_IMMEDIATE : 즉시 시연
	// D3DPRESENT_INTERVAL_DEFAULT : 적절한 간격을 DIRECTX 알아서 결정, 보통 모니터 재생율을 따라감
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}

void CGraphic_Device::Render_Begin(void)
{
	// 렌더링 과정 : 후면 버퍼를 먼저 비운다(지운다)-> 후면 버퍼에 그린다-> 후면버퍼에 연결된 서피스를 전면버퍼에 연결된 서피스와 교체하여 출력한다.

	// 1인자 : 지우고자 하는 렉트의 개수
	// 2인자 : 지우고자 하는 렉트 배열의 이름(nullptr인 경우 전체 화면 영역에 해당하는 렉터라는 의미)

	m_pDevice->Clear(0,
		nullptr,
		D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_ARGB(255, 0, 0, 255),	// 백버퍼 색상
		1.f, // z버퍼의 초기화 값
		0);	 // 스텐실 버퍼의 초기화 값

	//여기서부터 후면버퍼에 그리기 시점을 알린다.
	m_pDevice->BeginScene();


}

void CGraphic_Device::Render_End(HWND hWnd)
{
	
	m_pDevice->EndScene();

	// 후면버퍼에 연결되어 있는 서피스와 전면 버퍼에 연결된 서피스를 교환하는 과정
	m_pDevice->Present(nullptr, nullptr, hWnd, nullptr);
	// 1,2인자 : 스왑체인 기법이 D3DSWAPEFFECT_COPY가 아닌 이상 NULL
	// 3인자 : 출력 대상 윈도우 핸들, 기본 nullptr인 경우 d3dpp.hDeviceWindow을 참조
	// 4인자 :  스왑체인 기법이 D3DSWAPEFFECT_COPY가 아닌 이상 NULL

}

void CGraphic_Device::Free()
{
	Safe_Release(m_pFont);
	Safe_Release(m_pSprite);

	unsigned long		RefCnt = 0;
	if (RefCnt = Safe_Release(m_pDevice))
		ERR_MSG(TEXT("Failed to Release : IDirect3DDevice9"));
	
	Safe_Release(m_p3D);
}
