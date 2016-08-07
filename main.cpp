#include<windows.h>
#include<d3dx9.h>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

//▼▼▼面倒だからおまじないって事で詳しくはggr▼▼▼//
LPDIRECT3D9		     g_pD3D			= NULL;

//【DirectX9のデバイス】
//LPDIRECT3DDEVICE9は、DirectX9の機能を使う為のハンドル的な奴
//DirectX9の機能がここに詰まってるので間違って消さない様に。
//▼▼▼デバイス▼▼▼//
LPDIRECT3DDEVICE9 g_d3dDevice	= NULL;

//▼▼▼頂点フォーマット▼▼▼//　/*後で説明する*/
#define FVF_TAG_VERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

//▼▼▼ポリゴンを構築する頂点情報▼▼▼//
typedef struct{
	/*座標情報*/
	float x,y,z;				//D3DFVF_XYZRHWのXYZの部分
	/*おまじないでおｋ*/
	float rhw;				//射影変換完了フラグ D3DFVF_XYZRHWのRHWの部分
	/*ポリゴンの色情報*/
	D3DCOLOR deffuse;	//D3DFVF_DIFFUSE
	/*テクスチャ座標*/
	float tu,tv;				//D3DFVF_TEX1
}TAG_VERTEX;


LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
//▼▼▼初期化関数▼▼▼//
HRESULT			InitD3D(HWND);
//▼▼▼描画関数▼▼▼//
void				DrawD3D(void);
//▼▼▼終了処理関数▼▼▼//
void             DestroyD3D(void);

//■■■■■■■■■■■■■■■■//
//■■■【エントリポイント】■■■//
//■■■■■■■■■■■■■■■■//
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hWnd;
	MSG msg;

	wc.cbSize=sizeof(WNDCLASSEX);
	wc.style=CS_HREDRAW|CS_VREDRAW;
	wc.hInstance=hInstance;
	wc.lpszClassName="DX21";
	wc.lpfnWndProc=(WNDPROC)WndProc;
	wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wc.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName=NULL;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL,"ウィンドウクラスの構造体の初期化エラー","",MB_OK);
	}
		hWnd = CreateWindow(wc.lpszClassName,
							"DirectX9　サンプルプログラム",
							WS_CAPTION | WS_SYSMENU,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							640,
							480,
							NULL,
							NULL,
							hInstance,
							NULL);
	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);

	//▼▼▼ここで初期化▼▼▼//
	if (FAILED(InitD3D(hWnd))) return -1;
	//▼▼▼ここで描画▼▼▼//  /*メインループ毎に描画を呼び出すべきだが、ここでは解説の為一回だけ呼び出す*/
	DrawD3D();

	//メインループ
	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(GetMessage(&msg,NULL,0,0)==0)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	//DirectX終了処理関数
	DestroyD3D();
	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message){
		case WM_DESTROY:
			PostQuitMessage(0);
			g_d3dDevice->Release();
			break;

		default:
			return DefWindowProc(hWnd,message,wParam,lParam);
	}
	return 0;
}

//■■■■■■■■■■■■■■■■■//
//■■■【DirectX9の初期化】 ■■■//
//■■■■■■■■■■■■■■■■■//
HRESULT InitD3D(HWND hWnd)
{
	//▼▼▼どんなDirectX9を作りたいの？▼▼▼//
	//▼▼▼ぶっちゃけおまじない▼▼▼//
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (NULL == g_pD3D)
	{
		return E_FAIL;
	}
	//▼▼▼デバイスの生成▼▼▼//
	HRESULT hr=g_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&g_d3dDevice);
	if(FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;
}

//■■■■■■■■■■■■■■■//
//■■■【画面に絵を描く】■■■//
//■■■■■■■■■■■■■■■//
void DrawD3D(void)
{
	//▼▼▼画面全体を青色にリセットする▼▼▼//
	g_d3dDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,255),1.0f,0);
	//▼▼▼テクスチャのハンドル的なアレ▼▼▼//
	LPDIRECT3DTEXTURE9 texture;
	//▼▼▼テクスチャを読み込む▼▼▼// /*テクスチャの読み込み処理は鬼の様な重さなので、初期化でやっておくことが望ましい*/
	D3DXCreateTextureFromFile(g_d3dDevice,"tex.jpg",&texture);

	//▼▼▼頂点情報の配列▼▼▼//
	TAG_VERTEX vertex[4];
	//▼▼▼おまじない▼▼▼//
	vertex[0].rhw=vertex[1].rhw=vertex[2].rhw=vertex[3].rhw=1.0f;
	//▼▼▼2D描画にするので深度は0で▼▼▼//
	vertex[0].z=vertex[1].z=vertex[2].z=vertex[3].z=0.0f;
	//▼▼▼頂点の座標を設定▼▼▼//
	vertex[0].x=0.0f;
	vertex[0].y=300.0f;
	vertex[1].x=0.0f;
	vertex[1].y=0.0f;
	vertex[2].x=300.0f;
	vertex[2].y=0.0f;
	vertex[3].x=300.0f;
	vertex[3].y=300.0f;
	//▼▼▼テクスチャ座標を設定▼▼▼//
	vertex[0].tu = 0.0f;
	vertex[0].tv = 1.0f;
	vertex[1].tu = 0.0f;
	vertex[1].tv = 0.0f;
	vertex[2].tu = 1.0f;
	vertex[2].tv = 0.0f;
	vertex[3].tu = 1.0f;
	vertex[3].tv = 1.0f;
	//▼▼▼頂点の色を設定▼▼▼//
	vertex[0].deffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	vertex[1].deffuse=D3DCOLOR_RGBA(255,255,255,255);
	vertex[2].deffuse=D3DCOLOR_RGBA(255,255,255,255);
	vertex[3].deffuse=D3DCOLOR_RGBA(255,255,255,255);

	//▼▼▼頂点情報の送出を開始▼▼▼//
	if(SUCCEEDED(g_d3dDevice->BeginScene()))
	{
		//▼▼▼頂点フォーマットの決定▼▼▼//
		g_d3dDevice->SetFVF(FVF_TAG_VERTEX);
		//▼▼▼おまじない▼▼▼//
		g_d3dDevice->SetRenderState(
					D3DRS_CULLMODE,
					D3DCULL_CCW);
		//▼▼▼どのテクスチャを設定するか決定▼▼▼//
		g_d3dDevice->SetTexture(0,texture);
		//▼▼▼頂点データをGPUに送出する▼▼▼//
		g_d3dDevice->DrawPrimitiveUP(
										D3DPT_TRIANGLEFAN,	//頂点座標をどうやって使うかを指定
										2,								//ポリゴン数
										vertex,						//頂点情報
										sizeof(TAG_VERTEX)		//頂点フォーマットの容量
										);
		//▼▼▼頂点情報の送出を終了▼▼▼//
		g_d3dDevice->EndScene();
	}
	//▼▼▼送出した頂点情報を元に実際に画面にポリゴンを書き込む▼▼▼//
	g_d3dDevice->Present(NULL, NULL, NULL, NULL);
}

//■■■■■■■■■■■■■■■■//
//■■■【DirectX終了処理】 ■■■//
//■■■■■■■■■■■■■■■■//
void DestroyD3D(void)
{
	//▼▼▼デバイス消える▼▼▼//
	if(g_d3dDevice !=NULL)
	{
		g_d3dDevice->Release();
	}
	//▼▼▼おまじない消える▼▼▼//
	if(g_pD3D !=NULL)
	{
		g_pD3D->Release();
	}
}



#if 0
◎頂点フォーマットってなんぞや？
	→頂点にどんな情報を持たせるかを指定する仕組み。

	D3DFVFの組み合わせで色々な頂点情報を持たせる事が可能
	そして、そのフォーマットに対応した頂点構造体を自分で生成し、
	その構造体をGPUへ転送する事で頂点情報を伝える事が出来る。

	例えば、D3DFVF_XYZは、頂点にXYZの座標を付与する。

	D3DFVF_XYZRHWは、頂点情報と、射影変換フラグを付与する。
	射影変換フラグは2D描画をする間は1を設定する。
	射影変換フラグは3D描画を行う時に重要となる。

	D3DFVF_DIFFUSEは頂点に色情報を持たせる事が出来る。
	これをいじる事で頂点の色が変わる。

	等々、様々なフォーマットがあるので自分で調べてみよう
#endif