#define WIN32_LEAN_AND_MEAN

#include<Windows.h>
#include<sstream>

using namespace std;

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

	//1. WindowClass ���
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	//Class Style_ HorizontalRedraw, VerticalRedraw; 
	//Ŭ���̾�Ʈ�� ���� �� �������� �����̰ų� ����� �ٲ� �� �ٽ� �׸���.
	wc.style = CS_HREDRAW | CS_VREDRAW;
	//long pointer to string Zero_terminated '\0'���� ������ ���ڿ� ������
	wc.lpszClassName = gClassName;
	wc.hInstance = hInstance;
	//LoadCursor(Null = �⺻ �ü�� ���콺, Arrow �⺻ ȭ��ǥ Ŀ��)
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	//long pointer function window procedure
	//callback �Լ� ����
	wc.lpfnWndProc = WindowProc;
	//count of byte �ʼ��׸� ����ü ������ ����. �����÷ο� ���� ��������
	wc.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(
			nullptr, L"Failed to register window class", L"Error", MB_ICONEXCLAMATION | MB_OK
		);
		return 0;
	}

	hwnd = CreateWindowEx(
		NULL,					//Tpye : DWORD, name : dwExStyle //Description : �߰� Style 
		gClassName,				//Description : ������ Ŭ���� �̸� ����, RegisterClassEX������ �̸��� ����
		L"Hello window",		//Title Bar name
		WS_OVERLAPPEDWINDOW,	//Type : DWORD / name : dwStyle / Description : ������ ��Ÿ��(�⺻��)
		CW_USEDEFAULT,			//Type : int / Description : x��ġ, ����Ʈ ��, OS�� �ڵ�����
		CW_USEDEFAULT,			//Type : int / Description : y��ġ
		WINDOW_WIDTH,			//Type : int / Window Width Size
		WINDOW_HEIGHT,			//Type : int / Window Height Size
		NULL,					//Type : HWND / Description : �θ� �����츦 ����, ���Ӱ����� ������
		NULL,					//Type : HMENU / Description : �޴��� �ڵ�
		hInstance,
		NULL);					//Type : LPVOID / name : lpParam / dsc : ������ ������ �߰� ����

	if (hwnd == nullptr)
	{
		MessageBox(nullptr, L"Failed to create window class", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	//hwnd�� ȣ��, nShowCmd = ȭ�� option(SW_SHOWNORMAL, SW_MINMIZE, SW_MAXIMAZE) 
	ShowWindow(hwnd, nShowCmd);
	//ReDrawing(update) Window(hWnd)
	UpdateWindow(hwnd);

	MSG msg;
	//GetMessage(lpMsg : MSG ����ü ������, hwnd, wMsgFilterMin, Max : Min~Max ��ü �޽��� Ȯ��)
	//GetMessage�� true��, window ����� false
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//�Է� ���� ���� Ű ������ ó��
		TranslateMessage(&msg);
		//WindowProc���� ����
		DispatchMessage(&msg);
	}
	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//������ �ݱ� ��ư Ȥ�� Alt + F4 ������ ���
		case WM_CLOSE: 
			//�ش� �����츦 �ı�, ������ ������ ���� ������ �޸𸮿��� ����
			DestroyWindow(hWnd);
			break; 
		//������ �ı��� ��, DestroyWindow�Լ� �۵��� �޸� ������ ���� ����� ó��
		case WM_DESTROY:
			//PostQuitMessage(0) �� GetMessage�� WM_QUIT�޽����� �޾� False�� ��ȯ
			PostQuitMessage(0);
			break;
		//Mouse Left Button Down
		case WM_LBUTTONDOWN:
		{
			//wm_button�� �Ű������� wchar�� ������ wostringstream
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
			//case�� ������ �޽����� ������ �޽����� �⺻ �޽��� ���ν����� ���� ó��
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}