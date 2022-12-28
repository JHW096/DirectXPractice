#define WIN32_LEAN_AND_MEAN

#include<Windows.h>
#include<wrl/client.h>
#include<d3d11.h>
#include<sstream>

#pragma commnet(lib, "d3d11.lib");

using namespace std;

const wchar_t gClassName[]{ L"MyWindowClass" };
const wchar_t gTitle[]{ L"Direct3D" };
const int WINDOW_WIDTH{ 800 };
const int WINDOW_HEIGHT{ 600 };

//Swap chain = front, back buffers
Microsoft::WRL::ComPtr<IDXGISwapChain> gspSwapChain{ };
//Device = Display adaptor, Graphic card // fn = control device and administrate
Microsoft::WRL::ComPtr<ID3D11Device> gspDevice{ };
//info - GPU and memory control // Rendering 방식
Microsoft::WRL::ComPtr<ID3D11DeviceContext> gspDeviceContext{ };

void InitD3D();
void DestroyD3D();

HWND gHwnd{ };
HINSTANCE gInstance{ };

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	WNDCLASSEX wc;

	//1. WindowClass 등록
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	gInstance = hInstance;

	//Class Style_ HorizontalRedraw, VerticalRedraw; 
	//클라이언트를 수평 및 수직으로 움직이거나 사이즈를 바꿀 때 다시 그린다.
	wc.style = CS_HREDRAW | CS_VREDRAW;
	//long pointer to string Zero_terminated '\0'으로 끝나는 문자열 포인터
	wc.lpszClassName = gClassName;
	wc.hInstance = hInstance;
	//LoadCursor(Null = 기본 운영체제 마우스, Arrow 기본 화살표 커서)
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	//long pointer function window procedure
	//callback 함수 지정
	wc.lpfnWndProc = WindowProc;
	//count of byte 필수항목 구조체 사이즈 지정. 오버플로우 오류 방지위함
	wc.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(
			nullptr, L"Failed to register window class", L"Error", MB_ICONEXCLAMATION | MB_OK
		);
		return 0;
	}

	RECT wr{ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	gHwnd = CreateWindowEx(
		NULL,					//Tpye : DWORD, name : dwExStyle //Description : 추가 Style 
		gClassName,				//Description : 윈도우 클래스 이름 지정, RegisterClassEX지정한 이름만 가능
		L"DirectX practcie",	//Title Bar name
		WS_OVERLAPPEDWINDOW,	//Type : DWORD / name : dwStyle / Description : 윈도우 스타일(기본형)
		CW_USEDEFAULT,			//Type : int / Description : x위치, 디폴트 값, OS가 자동지정
		CW_USEDEFAULT,			//Type : int / Description : y위치
		wr.right = wr.left,			//Type : int / Window Width Size
		wr.bottom - wr.top,			//Type : int / Window Height Size
		NULL,					//Type : HWND / Description : 부모 윈도우를 지정, 종속관계의 윈도우
		NULL,					//Type : HMENU / Description : 메뉴바 핸들
		hInstance,
		NULL);					//Type : LPVOID / name : lpParam / dsc : 윈도우 생성시 추가 정보

	if (gHwnd == nullptr)
	{
		MessageBox(nullptr, L"Failed to create window class", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	//hwnd를 호출, nShowCmd = 화면 option(SW_SHOWNORMAL, SW_MINMIZE, SW_MAXIMAZE) 
	ShowWindow(gHwnd, nShowCmd);
	//window에서 동시에 여러 앱이 실행되고 있을 확률이 높으며, 활성화 없이 DirectX가 작동 안할 가능성있음
	//Foreground 창 변경 작업(Alt + Tap 또는 Alt + 윈도우 창 변경) 해당 윈도우 호출 가능
	SetForegroundWindow(gHwnd); 
	//키보드 입력을 받을 창
	SetFocus(gHwnd);
	//ReDrawing(update) Window(hWnd)
	UpdateWindow(gHwnd);

	MSG msg;
	//GetMessage(lpMsg : MSG 구조체 포인터, hwnd, wMsgFilterMin, Max : Min~Max 전체 메시지 확인)
	//GetMessage는 true값, window 종료시 false
	//PeekMessage는 매 프레임마다 함수가 실행되고 메시지가 있든 없든 항상 1을 반환. 
	//반면 Getmeesgae는 queue에 Message가 없으면 항상 대기
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		//입력 값을 가상 키 값으로 처리
		TranslateMessage(&msg);
		//WindowProc에게 전달
		DispatchMessage(&msg);
	}
	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//윈도우 닫기 버튼 혹은 Alt + F4 종료의 경우
		case WM_CLOSE: 
			//해당 윈도우를 파괴, 윈도우 생성에 사용된 정보를 메모리에서 해제
			DestroyWindow(hWnd);
			break; 
		//윈도우 파괴시 즉, DestroyWindow함수 작동시 메모리 해제와 같은 종료시 처리
		case WM_DESTROY:
			//PostQuitMessage(0) 은 GetMessage가 WM_QUIT메시지를 받아 False로 변환
			PostQuitMessage(0);
			break;
		//Mouse Left Button Down
		case WM_LBUTTONDOWN:
		{
			//wm_button의 매개변수가 wchar기 때문에 wostringstream
			wostringstream oss;
			//lParam's tpye = double word(32bit) = HIWORD(16, y) + LOWORD(16, x)
			oss << "X : " << LOWORD(lParam) << ", Y : " << HIWORD(lParam) << endl;
			OutputDebugString(oss.str().c_str());
			break;
		}
		//keyboard down
		case WM_KEYDOWN: 
		{
			wostringstream oss;
			//wParam = virtual key value
			oss << "Virtual Key : " << wParam << endl;
			OutputDebugString(oss.str().c_str());
			break;
		}
		default:
			//case로 지정한 메시지를 제외한 메시지를 기본 메시지 프로시저를 통해 처리
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void InitD3D()
{
	//swap chain초기화를 어떤 방법으로 초기화 할 것인가?
	//DirectX Graphics Infrastructure_Swap_chain_Desciption
	DXGI_SWAP_CHAIN_DESC scd{ };

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	//Back buffer Count (Front - 1 / Count - Back) = 2
	scd.BufferCount = 1;
	//Display mode / 일단은 생상표현만
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//후면 버퍼 접근 방식지정
	//DXGI_USAGE_RENDER_TARGET_OUTPUT = 후면 버퍼에 직접 그림을 그림.
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//그래픽이 그려질 화면 = gHwnd;
	scd.OutputWindow = gHwnd;
	scd.SampleDesc.Count = 1;
	//Window mode = true;
	scd.Windowed = TRUE;

	//말 그대로 그래픽 카드와 스왑체인을 생성해주는 함수
	D3D11CreateDeviceAndSwapChain(
		NULL,						//IDXGIAdapter* 그래픽카드 지정하는 포인터, NULL = 기본
		D3D_DRIVER_TYPE_HARDWARE,	//하드웨어 or 소프트웨어 옵션 설정, 주로 하드웨어 지정
		NULL,						//dynamic-link library (잘 모름)
		NULL,						//DirectX3D 실행에 대한 플래그 지정(잘 모름)
		NULL,						//기능 수준 지정 version up ex) 11.1 이 아니라면 null
		NULL,						//위에도 null이면 여기도 null
		D3D11_SDK_VERSION,			//SDK-version
		&scd,						//swapchain구조체 넣으면 됨
		gspSwapChain.ReleaseAndGetAddressOf(),		//Swap Chain, 해제 후 주소 반환
		gspDevice.ReleaseAndGetAddressOf(),			//디바이스 포인터
		NULL,	//기능 수준 배열 반환
		gspDeviceContext.ReleaseAndGetAddressOf()	//디바이스 컨텍스트 포인터 해제후 반환
	);
}

void DestroyD3D()
{	//Microsoft::Comptr delete
	gspSwapChain.Reset();
	gspDevice.Reset();
	gspDeviceContext.Reset();
	//비정상 종료를 예상한 한 번 더 해제
	DestroyWindow(gHwnd);
	//윈도우 클래스 해제
	UnregisterClass(gClassName, gInstance);
}