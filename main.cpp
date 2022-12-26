#define WIN32_LEAN_AND_MEAN

#include<Windows.h>

const wchar_t gClassName[]{ L"MyWindowClass" };
const wchar_t gTitle[]{ L"Direct3D" };
const int WINDOW_WIDTH{ 800 };
const int WINDOW_HEIGHT{ 600 };


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	HWND hwnd;
	WNDCLASSEX wc;

	//1. WindowClass 등록
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
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

	hwnd = CreateWindowEx(
		NULL,					//Tpye : DWORD, name : dwExStyle //Description : 추가 Style 
		gClassName,				//Description : 윈도우 클래스 이름 지정, RegisterClassEX지정한 이름만 가능
		L"Hello window",		//Title Bar name
		WS_OVERLAPPEDWINDOW,	//Type : DWORD / name : dwStyle / Description : 윈도우 스타일(기본형)
		CW_USEDEFAULT,			//Type : int / Description : x위치, 디폴트 값, OS가 자동지정
		CW_USEDEFAULT,			//Type : int / Description : y위치
		WINDOW_WIDTH,			//Type : int / Window Width Size
		WINDOW_HEIGHT,			//Type : int / Window Height Size
		NULL,					//Type : HWND / Description : 부모 윈도우를 지정, 종속관계의 윈도우
		NULL,					//Type : HMENU / Description : 메뉴바 핸들
		hInstance,
		NULL);					//Type : LPVOID / name : lpParam / dsc : 윈도우 생성시 추가 정보

	if (hwnd == nullptr)
	{
		MessageBox(nullptr, L"Failed to create window class", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	//hwnd를 호출, nShowCmd = 화면 option(SW_SHOWNORMAL, SW_MINMIZE, SW_MAXIMAZE) 
	ShowWindow(hwnd, nShowCmd);
	//ReDrawing(update) Window(hWnd)
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CLOSE: 
			DestroyWindow(hWnd);
			break; 
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}