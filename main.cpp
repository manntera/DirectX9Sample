#include<windows.h>
#include<d3dx9.h>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

//�������ʓ|�����炨�܂��Ȃ����Ď��ŏڂ�����ggr������//
LPDIRECT3D9		     g_pD3D			= NULL;

//�yDirectX9�̃f�o�C�X�z
//LPDIRECT3DDEVICE9�́ADirectX9�̋@�\���g���ׂ̃n���h���I�ȓz
//DirectX9�̋@�\�������ɋl�܂��Ă�̂ŊԈ���ď����Ȃ��l�ɁB
//�������f�o�C�X������//
LPDIRECT3DDEVICE9 g_d3dDevice	= NULL;

//���������_�t�H�[�}�b�g������//�@/*��Ő�������*/
#define FVF_TAG_VERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

//�������|���S�����\�z���钸�_��񁥁���//
typedef struct{
	/*���W���*/
	float x,y,z;				//D3DFVF_XYZRHW��XYZ�̕���
	/*���܂��Ȃ��ł���*/
	float rhw;				//�ˉe�ϊ������t���O D3DFVF_XYZRHW��RHW�̕���
	/*�|���S���̐F���*/
	D3DCOLOR deffuse;	//D3DFVF_DIFFUSE
	/*�e�N�X�`�����W*/
	float tu,tv;				//D3DFVF_TEX1
}TAG_VERTEX;


LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
//�������������֐�������//
HRESULT			InitD3D(HWND);
//�������`��֐�������//
void				DrawD3D(void);
//�������I�������֐�������//
void             DestroyD3D(void);

//��������������������������������//
//�������y�G���g���|�C���g�z������//
//��������������������������������//
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
		MessageBox(NULL,"�E�B���h�E�N���X�̍\���̂̏������G���[","",MB_OK);
	}
		hWnd = CreateWindow(wc.lpszClassName,
							"DirectX9�@�T���v���v���O����",
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

	//�����������ŏ�����������//
	if (FAILED(InitD3D(hWnd))) return -1;
	//�����������ŕ`�恥����//  /*���C�����[�v���ɕ`����Ăяo���ׂ������A�����ł͉���̈׈�񂾂��Ăяo��*/
	DrawD3D();

	//���C�����[�v
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

	//DirectX�I�������֐�
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

//����������������������������������//
//�������yDirectX9�̏������z ������//
//����������������������������������//
HRESULT InitD3D(HWND hWnd)
{
	//�������ǂ��DirectX9����肽���́H������//
	//�������Ԃ����Ⴏ���܂��Ȃ�������//
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
	//�������f�o�C�X�̐���������//
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

//������������������������������//
//�������y��ʂɊG��`���z������//
//������������������������������//
void DrawD3D(void)
{
	//��������ʑS�̂�F�Ƀ��Z�b�g���遥����//
	g_d3dDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,255),1.0f,0);
	//�������e�N�X�`���̃n���h���I�ȃA��������//
	LPDIRECT3DTEXTURE9 texture;
	//�������e�N�X�`����ǂݍ��ށ�����// /*�e�N�X�`���̓ǂݍ��ݏ����͋S�̗l�ȏd���Ȃ̂ŁA�������ł���Ă������Ƃ��]�܂���*/
	D3DXCreateTextureFromFile(g_d3dDevice,"tex.jpg",&texture);

	//���������_���̔z�񁥁���//
	TAG_VERTEX vertex[4];
	//���������܂��Ȃ�������//
	vertex[0].rhw=vertex[1].rhw=vertex[2].rhw=vertex[3].rhw=1.0f;
	//������2D�`��ɂ���̂Ő[�x��0�Ł�����//
	vertex[0].z=vertex[1].z=vertex[2].z=vertex[3].z=0.0f;
	//���������_�̍��W��ݒ聥����//
	vertex[0].x=0.0f;
	vertex[0].y=300.0f;
	vertex[1].x=0.0f;
	vertex[1].y=0.0f;
	vertex[2].x=300.0f;
	vertex[2].y=0.0f;
	vertex[3].x=300.0f;
	vertex[3].y=300.0f;
	//�������e�N�X�`�����W��ݒ聥����//
	vertex[0].tu = 0.0f;
	vertex[0].tv = 1.0f;
	vertex[1].tu = 0.0f;
	vertex[1].tv = 0.0f;
	vertex[2].tu = 1.0f;
	vertex[2].tv = 0.0f;
	vertex[3].tu = 1.0f;
	vertex[3].tv = 1.0f;
	//���������_�̐F��ݒ聥����//
	vertex[0].deffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	vertex[1].deffuse=D3DCOLOR_RGBA(255,255,255,255);
	vertex[2].deffuse=D3DCOLOR_RGBA(255,255,255,255);
	vertex[3].deffuse=D3DCOLOR_RGBA(255,255,255,255);

	//���������_���̑��o���J�n������//
	if(SUCCEEDED(g_d3dDevice->BeginScene()))
	{
		//���������_�t�H�[�}�b�g�̌��聥����//
		g_d3dDevice->SetFVF(FVF_TAG_VERTEX);
		//���������܂��Ȃ�������//
		g_d3dDevice->SetRenderState(
					D3DRS_CULLMODE,
					D3DCULL_CCW);
		//�������ǂ̃e�N�X�`����ݒ肷�邩���聥����//
		g_d3dDevice->SetTexture(0,texture);
		//���������_�f�[�^��GPU�ɑ��o���遥����//
		g_d3dDevice->DrawPrimitiveUP(
										D3DPT_TRIANGLEFAN,	//���_���W���ǂ�����Ďg�������w��
										2,								//�|���S����
										vertex,						//���_���
										sizeof(TAG_VERTEX)		//���_�t�H�[�}�b�g�̗e��
										);
		//���������_���̑��o���I��������//
		g_d3dDevice->EndScene();
	}
	//���������o�������_�������Ɏ��ۂɉ�ʂɃ|���S�����������ށ�����//
	g_d3dDevice->Present(NULL, NULL, NULL, NULL);
}

//��������������������������������//
//�������yDirectX�I�������z ������//
//��������������������������������//
void DestroyD3D(void)
{
	//�������f�o�C�X�����遥����//
	if(g_d3dDevice !=NULL)
	{
		g_d3dDevice->Release();
	}
	//���������܂��Ȃ������遥����//
	if(g_pD3D !=NULL)
	{
		g_pD3D->Release();
	}
}



#if 0
�����_�t�H�[�}�b�g���ĂȂ񂼂�H
	�����_�ɂǂ�ȏ����������邩���w�肷��d�g�݁B

	D3DFVF�̑g�ݍ��킹�ŐF�X�Ȓ��_�����������鎖���\
	�����āA���̃t�H�[�}�b�g�ɑΉ��������_�\���̂������Ő������A
	���̍\���̂�GPU�֓]�����鎖�Œ��_����`���鎖���o����B

	�Ⴆ�΁AD3DFVF_XYZ�́A���_��XYZ�̍��W��t�^����B

	D3DFVF_XYZRHW�́A���_���ƁA�ˉe�ϊ��t���O��t�^����B
	�ˉe�ϊ��t���O��2D�`�������Ԃ�1��ݒ肷��B
	�ˉe�ϊ��t���O��3D�`����s�����ɏd�v�ƂȂ�B

	D3DFVF_DIFFUSE�͒��_�ɐF�����������鎖���o����B
	����������鎖�Œ��_�̐F���ς��B

	���X�A�l�X�ȃt�H�[�}�b�g������̂Ŏ����Œ��ׂĂ݂悤
#endif