#define WIN32_LEAN_AND_MEAN

#include<Windows.h>
#include<wrl/client.h>
#include<d3d11.h>
#include<sstream>

#pragma comment(lib, "d3d11.lib")

using namespace std;

const wchar_t gClassName[]{ L"MyWindowClass" };
const wchar_t gTitle[]{ L"Direct3D" };
const int WINDOW_WIDTH{ 800 };
const int WINDOW_HEIGHT{ 600 };

//Swap chain = front, back buffers
Microsoft::WRL::ComPtr<IDXGISwapChain> gspSwapChain{ };
//Device = Display adaptor, Graphic card // fn = control device and administrate
Microsoft::WRL::ComPtr<ID3D11Device> gspDevice{ };
//info - GPU and memory control // Rendering ���
Microsoft::WRL::ComPtr<ID3D11DeviceContext> gspDeviceContext{ };

Microsoft::WRL::ComPtr<ID3D11Texture2D> gspRenderTarget{ };
Microsoft::WRL::ComPtr<ID3D11Texture2D> gspDepthStencil{ };
Microsoft::WRL::ComPtr<ID3D11RenderTargetView> gspRenderTargetView{ };
Microsoft::WRL::ComPtr<ID3D11DepthStencilView> gspDepthStencilView{ };


void InitD3D();
void DestroyD3D();
void RenderFrame();

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

	//1. WindowClass ���
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	gInstance = hInstance;

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

	RECT wr{ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	gHwnd = CreateWindowEx(
		NULL,					//Tpye : DWORD, name : dwExStyle //Description : �߰� Style 
		gClassName,				//Description : ������ Ŭ���� �̸� ����, RegisterClassEX������ �̸��� ����
		L"DirectX practcie",	//Title Bar name
		WS_OVERLAPPEDWINDOW,	//Type : DWORD / name : dwStyle / Description : ������ ��Ÿ��(�⺻��)
		CW_USEDEFAULT,			//Type : int / Description : x��ġ, ����Ʈ ��, OS�� �ڵ�����
		CW_USEDEFAULT,			//Type : int / Description : y��ġ
		wr.right = wr.left,			//Type : int / Window Width Size
		wr.bottom - wr.top,			//Type : int / Window Height Size
		NULL,					//Type : HWND / Description : �θ� �����츦 ����, ���Ӱ����� ������
		NULL,					//Type : HMENU / Description : �޴��� �ڵ�
		hInstance,
		NULL);					//Type : LPVOID / name : lpParam / dsc : ������ ������ �߰� ����

	if (gHwnd == nullptr)
	{
		MessageBox(nullptr, L"Failed to create window class", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	//hwnd�� ȣ��, nShowCmd = ȭ�� option(SW_SHOWNORMAL, SW_MINMIZE, SW_MAXIMAZE) 
	ShowWindow(gHwnd, nShowCmd);
	//window���� ���ÿ� ���� ���� ����ǰ� ���� Ȯ���� ������, Ȱ��ȭ ���� DirectX�� �۵� ���� ���ɼ�����
	//Foreground â ���� �۾�(Alt + Tap �Ǵ� Alt + ������ â ����) �ش� ������ ȣ�� ����
	SetForegroundWindow(gHwnd); 
	//Ű���� �Է��� ���� â
	SetFocus(gHwnd);
	//ReDrawing(update) Window(hWnd)
	UpdateWindow(gHwnd);

	InitD3D();

	MSG msg;
	//GetMessage(lpMsg : MSG ����ü ������, hwnd, wMsgFilterMin, Max : Min~Max ��ü �޽��� Ȯ��)
	//GetMessage�� true��, window ����� false
	//PeekMessage�� �� �����Ӹ��� �Լ��� ����ǰ� �޽����� �ֵ� ���� �׻� 1�� ��ȯ. 
	//�ݸ� Getmeesgae�� queue�� Message�� ������ �׻� ���

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
		else
		{
			//loop
		}
	}
	DestroyD3D();

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

void InitD3D()
{
	//swap chain�ʱ�ȭ�� � ������� �ʱ�ȭ �� ���ΰ�?
	//DirectX Graphics Infrastructure_Swap_chain_Desciption
	DXGI_SWAP_CHAIN_DESC scd{ };

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	//Back buffer Count (Front - 1 / Count - Back) = 2
	scd.BufferCount = 1;
	//Display mode / �ϴ��� ����ǥ����
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//�ĸ� ���� ���� �������
	//DXGI_USAGE_RENDER_TARGET_OUTPUT = �ĸ� ���ۿ� ���� �׸��� �׸�.
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//�׷����� �׷��� ȭ�� = gHwnd;
	scd.OutputWindow = gHwnd;
	scd.SampleDesc.Count = 1;
	//Window mode = true;
	scd.Windowed = TRUE;

	//�� �״�� �׷��� ī��� ����ü���� �������ִ� �Լ�
	D3D11CreateDeviceAndSwapChain(
		NULL,						//IDXGIAdapter* �׷���ī�� �����ϴ� ������, NULL = �⺻
		D3D_DRIVER_TYPE_HARDWARE,	//�ϵ���� or ����Ʈ���� �ɼ� ����, �ַ� �ϵ���� ����
		NULL,						//dynamic-link library (�� ��)
		NULL,						//DirectX3D ���࿡ ���� �÷��� ����(�� ��)
		NULL,						//��� ���� ���� version up ex) 11.1 �� �ƴ϶�� null
		NULL,						//������ null�̸� ���⵵ null
		D3D11_SDK_VERSION,			//SDK-version
		&scd,						//swapchain����ü ������ ��
		gspSwapChain.ReleaseAndGetAddressOf(),		//Swap Chain, ���� �� �ּ� ��ȯ
		gspDevice.ReleaseAndGetAddressOf(),			//����̽� ������
		NULL,	//��� ���� �迭 ��ȯ
		gspDeviceContext.ReleaseAndGetAddressOf()	//����̽� ���ؽ�Ʈ ������ ������ ��ȯ
	);

	gspSwapChain->GetBuffer(
		0,
		IID_PPV_ARGS(gspRenderTarget.ReleaseAndGetAddressOf())
	);

	gspDevice->CreateRenderTargetView(
		gspRenderTarget.Get(),
		nullptr,
		gspRenderTargetView.GetAddressOf()
	);

	CD3D11_TEXTURE2D_DESC dsd(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		1,
		1,
		D3D11_BIND_DEPTH_STENCIL
	);

	gspDevice->CreateTexture2D(
		&dsd,
		nullptr,
		gspDepthStencil.ReleaseAndGetAddressOf()
	);

	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(D3D11_DSV_DIMENSION_TEXTURE2D);
	gspDevice->CreateDepthStencilView(
		gspDepthStencil.Get(),
		&dsvd,
		gspDepthStencilView.ReleaseAndGetAddressOf()
	);

	gspDeviceContext->OMSetRenderTargets(
		1,
		gspRenderTargetView.GetAddressOf(),
		gspDepthStencilView.Get()
	);

	CD3D11_VIEWPORT viewport(
		0.0f,
		0.0f,
		static_cast<float>(WINDOW_WIDTH),
		static_cast<float>(WINDOW_HEIGHT)
	);

	gspDeviceContext->RSSetViewports(1, &viewport);


}

void DestroyD3D()
{	//Microsoft::Comptr delete
	gspDepthStencil.Reset();
	gspDepthStencilView.Reset();
	gspRenderTarget.Reset();
	gspRenderTargetView.Reset();
	gspSwapChain.Reset();
	gspDevice.Reset();
	gspDeviceContext.Reset();
	//������ ���Ḧ ������ �� �� �� ����
	DestroyWindow(gHwnd);
	//������ Ŭ���� ����
	UnregisterClass(gClassName, gInstance);
}

void RenderFrame()
{
	float clear_color[4]{ 0.f, 0.2f, 0.4f, 1.0f };

	gspDeviceContext->ClearRenderTargetView(gspRenderTargetView.Get(), clear_color);
	gspDeviceContext->ClearDepthStencilView(
		gspDepthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);
	gspSwapChain->Present(0, 0);
}