#include <stdio.h>
#include<Windows.h>
#include<d3d9.h>
#pragma comment(lib,"d3d9.lib")
#include <Mmsystem.h>
#pragma comment( lib,"winmm.lib" )
#include <d3dx9math.h>
#pragma comment(lib,"d3dx9.lib")
const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
const D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
const D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
const D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
const D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
const D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));

//
// Lights
//

D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);

//
// Materials
//

D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);

const D3DMATERIAL9 WHITE_MTRL = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
const D3DMATERIAL9 RED_MTRL = InitMtrl(RED, RED, RED, BLACK, 2.0f);
const D3DMATERIAL9 GREEN_MTRL = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
const D3DMATERIAL9 BLUE_MTRL = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);
D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = a;
	mtrl.Diffuse = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power = p;
	return mtrl;
}


namespace d3d
{
	bool InitD3D(
		HINSTANCE hInstance,       // [in] Application instance.
		int width, int height,     // [in] Backbuffer dimensions.
		bool windowed,             // [in] Windowed (true)or full screen (false).
		D3DDEVTYPE deviceType,     // [in] HAL or REF
		IDirect3DDevice9** device);// [out]The created device.

	int EnterMsgLoop(
		bool(*ptr_display)(float timeDelta));

	LRESULT CALLBACK WndProc(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam);

	template<class T> void Release(T t)
	{
		if (t)
		{
			t->Release();
			t = 0;
		}
	}

	template<class T> void Delete(T t)
	{
		if (t)
		{
			delete t;
			t = 0;
		}
	}
}

//
// Globals
//
D3DXVECTOR3 position(0, 0, -10);
D3DXVECTOR3 target(0, 0, 0);
D3DXVECTOR3 up(0, 1, 0);
D3DMATERIAL9 mat = WHITE_MTRL;
D3DMATERIAL9 mat1 = RED_MTRL;
D3DXMATRIX V;
IDirect3DDevice9* Device = 0;
IDirect3DVertexBuffer9 *ve;
IDirect3DIndexBuffer9 *ind;
ID3DXMesh* Teapot = 0;
ID3DXMesh* DText = 0;
IDirect3DTexture9 *tex1;
IDirect3DTexture9 *tex2;
IDirect3DTexture9 *wall;
LPD3DXFONT font;
DWORD framecnt = 0;
float timeElapsed = 0;
float FPS = 0;
struct vertex
{
	vertex(float a, float b, float c, D3DVECTOR co)
	{
		x = a;
		y = b;
		z = c;
		ve = co;
	}
	vertex(float a, float b, float c, float f, float g, float h, float d, float e)
	{
		x = a;
		y = b;
		z = c;
		ve.x = f;
		ve.y = g;
		ve.z = h;
		u = d;
		v = e;
	}
	float x, y, z;
	D3DVECTOR ve;
	float u, v;
	static const DWORD FVF;
};
const DWORD vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
//
// Framework Functions
//
void calcfps(float timedelta);
bool Setup()
{
	HRESULT st;
	Device->SetRenderState(D3DRS_LIGHTING, true);
	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse = WHITE;
	dir.Specular = WHITE * 0.2f;
	dir.Ambient = WHITE * 0.6f;
	dir.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);
	Device->SetLight(0, &dir);
	Device->LightEnable(0, true);
	// Nothing to setup in this sample.
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);
	D3DXCreateTeapot(Device, &Teapot, 0);
	Device->CreateVertexBuffer(16 * sizeof(vertex), D3DUSAGE_WRITEONLY, vertex::FVF, D3DPOOL_MANAGED, &ve, 0);
	Device->CreateIndexBuffer(66 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &ind, 0);
	vertex *tex;
	ve->Lock(0, 0, (void**)&tex, 0);
	tex[0] = vertex(-1, -1, -1, 0.0f, 1.0f, 0.0f, 0, 1);
	tex[1] = vertex(-1, 1, -1, 0, 1, 0, 0, 0);
	tex[2] = vertex(1, 1, -1, 0, 1, 0, 1, 0);
	tex[3] = vertex(1, -1, -1, 0, 1, 0, 1, 1);
	tex[4] = vertex(-1, -1, 1, 0, 1, 0, 1, 1);
	tex[5] = vertex(-1, 1, 1, 0, 1, 0, 1, 0);
	tex[6] = vertex(1, 1, 1, 0, 1, 0, 0, 0);
	tex[7] = vertex(1, -1, 1, 0, 1, 0, 0, 1);
	tex[8] = vertex(10, -1, 10, 0, 1, 0, 0, 1);
	tex[9] = vertex(-10, -1, 10, 0, 1, 0, 0, 0);
	tex[10] = vertex(-10, -1, -10, 0, 1, 0, 1, 0);
	tex[11] = vertex(10, -1, -10, 0, 1, 0, 1, 1);
	tex[12] = vertex(10, 5, 10, 0, 1, 0, 0, 0);
	tex[13] = vertex(-10, 5, 10, 0, 1, 0, 0, 1);
	tex[14] = vertex(-10, 5, -10, 0, 1, 0, 1, 1);
	tex[15] = vertex(10, 5, -10, 0, 1, 0, 1, 0);
	ve->Unlock();
	WORD *indices;
	ind->Lock(0, 0, (void**)&indices, 0);
	// front side
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;

	// back side
	indices[6] = 4; indices[7] = 6; indices[8] = 5;
	indices[9] = 4; indices[10] = 7; indices[11] = 6;
	
	// left side
	indices[12] = 4; indices[13] = 5; indices[14] = 1;
	indices[15] = 4; indices[16] = 1; indices[17] = 0;

	// right side
	indices[18] = 3; indices[19] = 2; indices[20] = 6;
	indices[21] = 3; indices[22] = 6; indices[23] = 7;

	// top
	indices[24] = 1; indices[25] = 5; indices[26] = 6;
	indices[27] = 1; indices[28] = 6; indices[29] = 2;

	// bottom
	indices[30] = 4; indices[31] = 0; indices[32] = 3;
	indices[33] = 4; indices[34] = 3; indices[35] = 7;
	indices[36] = 9; indices[37] = 8; indices[38] = 10;
	indices[39] = 10; indices[40] = 8; indices[41] = 11;
	indices[42] = 15; indices[43] = 10; indices[44] = 11;
	indices[45] = 14; indices[46] = 10; indices[47] = 15;
	indices[48] = 9; indices[49] = 10; indices[50] = 14;
	indices[51] = 13; indices[52] = 9; indices[53] = 14;
	indices[54] = 12; indices[55] = 8; indices[56] = 9;
	indices[57] = 9; indices[58] = 13; indices[59] = 12;
	indices[60] = 15; indices[61] = 11; indices[62] = 8;
	indices[63] = 12; indices[64] = 15; indices[65] = 8;
	Device->SetTransform(D3DTS_VIEW, &V);
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveLH(&proj, D3DX_PI / 2, 1920 / 1080, 1, 1000);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	ind->Unlock();
	st = D3DXCreateTextureFromFile(Device, TEXT("201311140304289111.jpg"), &tex1);
	st = D3DXCreateTextureFromFile(Device, TEXT("crate.jpg"), &tex2);
	st = D3DXCreateTextureFromFile(Device, TEXT("wall.jpg"), &wall);
	D3DXFONT_DESC ds;
	ds.Weight = 500;
	ds.Height = 50;
	ds.Width = 10;
	ds.MipLevels = D3DX_DEFAULT;
	ds.Italic = true;
	ds.CharSet = DEFAULT_CHARSET;
	ds.OutputPrecision = 0;
	ds.Quality = 0;
	ds.PitchAndFamily = 0;
	wcscpy_s(ds.FaceName, 32, TEXT("Times New Roman"));
	D3DXCreateFontIndirect(Device, &ds, &font);

	

	LOGFONT lf;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfClipPrecision = 0;
	lf.lfEscapement = 0;
	lf.lfItalic = false;
	lf.lfOrientation = 0;
	wcscpy_s(lf.lfFaceName, 32, TEXT("Times New Roman"));
	lf.lfHeight = 30;
	lf.lfOutPrecision = 0;
	lf.lfPitchAndFamily = 0;
	lf.lfStrikeOut = false;
	lf.lfUnderline = false;
	lf.lfQuality = 0;
	lf.lfWidth = 12;
	lf.lfWeight = 500;
	HFONT fon= CreateFontIndirect(&lf);
	HDC hdc = CreateCompatibleDC(0);
	HGDIOBJ old = SelectObject(hdc, fon);
	st=D3DXCreateText(Device, hdc, TEXT("这姥姥,简直就是在搞笑"), 0.001f, 0.4f, &DText, 0, 0);
	SelectObject(hdc, old);
	DeleteObject(fon);
	DeleteDC(hdc);
	return true;
}
void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(ve);
	d3d::Release<IDirect3DIndexBuffer9*>(ind);
	// Nothing to cleanup in this sample.
}

bool Display(float timeDelta)
{
	if (Device) // Only use Device methods if we have a valid device.
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		RECT rec = { 0, 0, 200, 200 };
		calcfps(timeDelta);
		TCHAR *fps = (TCHAR*)malloc(50);
		swprintf(fps,50,TEXT("FPS:%f"), FPS);
		font->DrawText(NULL, fps, -1, &rec, DT_CENTER, D3DCOLOR_XRGB(255, 0, 0));
		free(fps);
		static float z = 0.001f;
		static int flag1 = 1;
		static int flag2 = 1;
		position.x = position.x + z*flag1;
		position.z = position.z + z*flag2;
		if (position.x >= 10.0f)
		{
			flag1 = -1;
		}
		if (position.x <= -10.0f)
		{
			flag1 = 1;
		}
		if (position.z >= 10.0f)
		{
			flag2 = -1;
		}
		if (position.z <= -10.0f)
		{
			flag2 = 1;
		}
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &V);
		
		
		
		D3DXMATRIX rx, ry, rz;
		D3DXMatrixRotationX(&rx, 0);
		static float y = 0;
		static float y1 = 0;
		D3DXMatrixRotationY(&ry, y * 3);
		D3DXMatrixTranslation(&rz, 2, 0, 0);
		/* y+= timeDelta;
		if (y >= 6.28f)
		{
		y = 0;
		}*/
		D3DXMATRIX world = rx*ry*rz;
		Device->SetTransform(D3DTS_WORLD, &world);
		Device->BeginScene();
		Device->SetStreamSource(0, ve, 0, sizeof(vertex));
		Device->SetIndices(ind);
		Device->SetFVF(vertex::FVF);
		Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		// Draw cube.
		Device->SetTexture(0, tex2);
		Device->SetMaterial(&mat);
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
		D3DXMatrixTranslation(&rz, 5, 0, 0);
		world = rz;
		Device->SetTransform(D3DTS_WORLD, &world);
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
		D3DXMatrixTranslation(&rz, 0, 0, 0);
		world = rz;
		Device->SetTransform(D3DTS_WORLD, &world);
		Device->SetTexture(0, tex1);
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 8, 4, 36, 2);
		D3DXMatrixTranslation(&rz, 0, 0, 0);
		world = rz;
		Device->SetTransform(D3DTS_WORLD, &world);
		Device->SetTexture(0, wall);
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 12, 4, 42, 8);
		static float tea = 0;
		static float x = 1;
		tea = tea + 0.001f*x;
		if (tea > 5)
		{
			x = -1;
		}
		if (tea < -5)
		{
			x = 1;
		}
		D3DXMatrixTranslation(&rz, -2, 0, tea);
		D3DXMatrixRotationY(&ry, y1*-3);
		D3DXMATRIX world1 = rx*ry*rz;
		y1 += timeDelta;
		if (y1 >= 6.28f)
		{
			y1 = 0;
		}
		Device->SetTransform(D3DTS_WORLD, &world1);
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		Device->SetMaterial(&mat1);
		Teapot->DrawSubset(0);
		D3DXMatrixTranslation(&rz, -3, 2, 0);
		world = rz;
		Device->SetTransform(D3DTS_WORLD, &world);
		Device->SetTexture(0, tex1);
		DText->DrawSubset(0);
		Device->EndScene();
		// Swap the back and front buffers.
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	if (!d3d::InitD3D(hinstance,
		800, 600, true, D3DDEVTYPE_HAL, &Device))
	{
		MessageBox(0, TEXT("InitD3D() - FAILED"), 0, 0);
		return 0;
	}

	if (!Setup())
	{
		::MessageBox(0, TEXT("Setup() - FAILED"), 0, 0);
		return 0;
	}
	d3d::EnterMsgLoop(Display);

	Cleanup();

	Device->Release();

	return 0;
}
bool d3d::InitD3D(
	HINSTANCE hInstance,
	int width, int height,
	bool windowed,
	D3DDEVTYPE deviceType,
	IDirect3DDevice9** device)
{
	//
	// Create the main application window.
	//

	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)d3d::WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = TEXT("Direct3D9App");

	if (!RegisterClass(&wc))
	{
		MessageBox(0, TEXT("RegisterClass() - FAILED"), 0, 0);
		return false;
	}

	HWND hwnd = 0;
	hwnd = CreateWindow(TEXT("Direct3D9App"), TEXT("Direct3D9App"),
		WS_EX_TOPMOST,
		0, 0, width, height,
		0 /*parent hwnd*/, 0 /* menu */, hInstance, 0 /*extra*/);

	if (!hwnd)
	{
		MessageBox(0, TEXT("CreateWindow() - FAILED"), 0, 0);
		return false;
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	
	//
	// Init D3D: 
	//

	HRESULT hr = 0;

	// Step 1: Create the IDirect3D9 object.

	IDirect3D9* d3d9 = 0;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	
	if (!d3d9)
	{
		::MessageBox(0, TEXT("Direct3DCreate9() - FAILED"), 0, 0);
		return false;
	}

	// Step 2: Check for hardware vp.

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);
	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.

	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = windowed;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.
	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		deviceType,         // device type
		hwnd,               // window associated with device
		vp,                 // vertex processing
		&d3dpp,             // present parameters
		device);            // return created device

	if (FAILED(hr))
	{
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			deviceType,
			hwnd,
			vp,
			&d3dpp,
			device);

		if (FAILED(hr))
		{
			d3d9->Release(); // done with d3d9 object
			::MessageBox(0, TEXT("CreateDevice() - FAILED"), 0, 0);
			return false;
		}
	}
	d3d9->Release(); // done with d3d9 object
	return true;
}

int d3d::EnterMsgLoop(bool(*ptr_display)(float timeDelta))
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime();

	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001f;

			ptr_display(timeDelta);

			lastTime = currTime;
		}
	}
	return msg.wParam;
}
void calcfps(float timedelta)
{
	framecnt++;
	timeElapsed += timedelta;
	if (timeElapsed >= 1.0f)
	{
		FPS = (float)framecnt / timedelta;
		framecnt = 0;
		timeElapsed = 0;
	}
}
