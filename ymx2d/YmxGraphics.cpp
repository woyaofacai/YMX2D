#include "stdafx.h"
#include "ymx2d.h"
#include "ymxvertex.h"
#include <Strsafe.h>

#define VB_CAPACITY 1000
#define IB_CAPACTIY 1500
#define PVB_CAPACITY 1000
#define PVB_BATCH_SIZE 200

const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
const DWORD ConciseVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
const DWORD ParticleVertex::FVF = D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE;

static inline DWORD FtoDW(float x)
{
	return *((DWORD*)&x);
}

YmxGraphics::YmxGraphics()
{
	m_CurrVertex = 0;
	m_CurrIndice = 0;
	m_CurrTexture = NULL;

	m_pvbOffset = 0;
	m_vertexColor = YMX_ARGB(255, 255, 255, 255);

	m_bFastParticle = false;
	m_CurBatch = 0;
}

YmxGraphics::~YmxGraphics()
{
	if(VB != NULL) {
		VB->Release();
		VB = NULL;
	}

	if(IB != NULL) {
		IB->Release();
		IB = NULL;
	}

	if(PVB != NULL) {
		PVB->Release();
		PVB = NULL;
	}

	if(PVB2 != NULL) {
		PVB2->Release();
		PVB2 = NULL;
	}


}

bool YmxGraphics::_D3D_Initialize()
{
	HRESULT hr;
	IDirect3D9* d3d9 = 0;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	int vp = 0;
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	}else {
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	if(caps.FVFCaps & D3DFVFCAPS_PSIZE) {
		m_bSupportPSize = true;
	}
	else {
		m_bSupportPSize = false;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth            = (int)m_screenWidth;
	d3dpp.BackBufferHeight           = (int)m_screenHeight;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.hDeviceWindow              = hwnd;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	if(m_game->m_bFullScreen)
	{
		d3dpp.SwapEffect  = D3DSWAPEFFECT_FLIP; 
		d3dpp.Windowed    = false;
	}
	else
	{
		d3dpp.SwapEffect  = D3DSWAPEFFECT_COPY; 
		d3dpp.Windowed    = true;
	}

	d3d9->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		vp,
		&d3dpp,
		&device
		);

	d3d9->Release();

	if(!device) 
	{
		RecordError(TEXT("DirectX Error"), TEXT("CreateDevice failed!"));
		return false;
	}

	hr = device->CreateVertexBuffer(
		VB_CAPACITY * sizeof(Vertex),
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_DEFAULT,
		&VB,
		0 
		);
	if(FAILED(hr))
	{
		RecordError(TEXT("DirectX Error"), TEXT("Create Vertex Buffer Failed!"));
		return false;
	}

	hr = device->CreateIndexBuffer(
		IB_CAPACTIY * sizeof(WORD),
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_DEFAULT,
		&IB,
		0
		);

	if(FAILED(hr))
	{
		RecordError(TEXT("DirectX Error"), TEXT("Create Index Buffer Failed!"));
		return false;
	}

	if(m_bFastParticle)
	{
		if(m_bSupportPSize) {
			hr = device->CreateVertexBuffer(
				PVB_CAPACITY * sizeof(ParticleVertex),
				D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
				ParticleVertex::FVF,
				D3DPOOL_DEFAULT,
				&PVB,
				0);

			if(FAILED(hr))
			{
				RecordError(TEXT("DirectX Error"), TEXT("Create Particle Vertex Buffer Failed!"));
				return false;
			}
		}

		hr = device->CreateVertexBuffer(
			PVB_CAPACITY * sizeof(ConciseVertex),
			D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
			ConciseVertex::FVF,
			D3DPOOL_DEFAULT,
			&PVB2,
			0);

		if(FAILED(hr))
		{
			RecordError(TEXT("DirectX Error"), TEXT("Create Particle Vertex Buffer Failed"));
			return false;
		}
	}
	else {
		m_bSupportPSize = true;
	}

	device->SetFVF(Vertex::FVF);
	hr = device->SetStreamSource(0, VB, 0, sizeof(Vertex));
	if(FAILED(hr))
	{
		RecordError(TEXT("D3D Error"), TEXT("Set Vertex Buffer as Stream Source Failed!"));
		return false;
	}
	hr = device->SetIndices(IB);
	if(FAILED(hr))
	{
		RecordError(TEXT("D3D Error"), TEXT("Set Indices Failed!"));
		return false;
	}

	device->SetRenderState(D3DRS_LIGHTING, false);
	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	device->SetRenderState(D3DRS_ALPHATESTENABLE, true);
	device->SetRenderState(D3DRS_ALPHAREF, 0x00000000);
	device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	//device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//for particles
	device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	device->SetRenderState(D3DRS_POINTSCALEENABLE, false);

	// Set World Matrix, View Matrix and Projection Matrix
	_PipelineTransform();

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	return true;
}

void YmxGraphics::_PipelineTransform()
{
	D3DXMATRIX worldMatrix;
	D3DXMATRIX m;
	D3DXMatrixScaling(&worldMatrix, 1.0f, -1.0f, 1.0f);
	D3DXMatrixTranslation(&m, -m_screenWidth * 0.5f, m_screenHeight * 0.5f, 0.0f);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &m);
	device->SetTransform(D3DTS_WORLD, &worldMatrix); 

	D3DXVECTOR3 pEye(0, 0, -m_screenWidth * 0.5f);
	D3DXVECTOR3 pAt(0, 0, 1.0f);
	D3DXVECTOR3 pUp(0, 1.0f, 0);


	D3DXMATRIX viewTranMatrix;
	D3DXMatrixLookAtLH(&viewTranMatrix, &pEye, &pAt, &pUp);	
	device->SetTransform(D3DTS_VIEW, &viewTranMatrix);


	D3DXMATRIX proj;
	//D3DXMatrixOrthoOffCenterLH(&proj, -screenWidth * 0.5f, screenWidth * 0.5f, -screenHeight * 0.5f, screenHeight * 0.5f, -1.0f, 100.0f);
	//D3DXMatrixOrthoLH(&proj, screenWidth, screenHeight, 0.0f, 1.0f);
	D3DXMatrixPerspectiveLH(&proj, m_screenWidth, m_screenHeight, m_screenWidth * 0.5f - 0.01f, m_screenWidth * 0.5f + 1.0f + 0.01f);
	device->SetTransform(D3DTS_PROJECTION, &proj);
}

void YmxGraphics::_Render_Begin()
{

	m_CurrVertex = 0;
	m_CurrIndice = 0;
	m_CurrTexture = NULL;
	//m_CurBatch = (m_CurBatch + 1) % VB_BATCH_NUM;

	device->SetTexture(0, NULL);


	//device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	device->BeginScene();

	//VB->Lock(m_CurBatch * VB_BATCH_SIZE * sizeof(Vertex), VB_BATCH_SIZE * sizeof(Vertex), (void**)&m_vertexs, 0);
	//IB->Lock(m_CurBatch * IB_BATCH_SIZE * sizeof(WORD), IB_BATCH_SIZE * sizeof(WORD), (void**)&m_indices, 0);

	VB->Lock(0, VB_CAPACITY * sizeof(Vertex), (void**)&m_vertexs, 0);
	IB->Lock(0, IB_CAPACTIY * sizeof(WORD), (void**)&m_indices, 0);
}

void YmxGraphics::Clear(YMXCOLOR color)
{
	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
}

void YmxGraphics::_Render_End()
{
	_Render_Batch(true);

	device->EndScene();
	device->Present(0, 0, 0, 0);
}

void YmxGraphics::_Render_Batch(bool end)
{
	//device->SetTexture(0, (LPDIRECT3DTEXTURE9)m_CurrTexture->lpTexture);
	VB->Unlock(); 
	IB->Unlock();
	//device->DrawIndexedPrimitive(
	//	D3DPT_TRIANGLELIST, m_CurBatch * VB_BATCH_SIZE, 
	//	0, m_CurrVertex, m_CurBatch * IB_BATCH_SIZE, m_CurrIndice / 3);

	device->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, 0, 
		0, m_CurrVertex, 0, m_CurrIndice / 3);

	if(!end)
	{
		m_CurrVertex = 0;
		m_CurrIndice = 0;
		//m_CurrTexture = NULL;
		//m_CurBatch = (m_CurBatch + 1) % VB_BATCH_NUM;

		//VB->Lock(m_CurBatch * VB_BATCH_SIZE * sizeof(Vertex), VB_BATCH_SIZE * sizeof(Vertex), (void**)&m_vertexs, 0);
		//IB->Lock(m_CurBatch * IB_BATCH_SIZE * sizeof(DWORD), IB_BATCH_SIZE * sizeof(WORD), (void**)&m_indices, 0);
		VB->Lock(0, VB_CAPACITY * sizeof(Vertex), (void**)&m_vertexs, 0);
		IB->Lock(0, IB_CAPACTIY * sizeof(WORD), (void**)&m_indices, 0);
	}
}

YMXCOLOR YmxGraphics::SetVertexColor(YMXCOLOR vertexColor)
{
	YMXCOLOR prevColor = m_vertexColor;
	m_vertexColor = vertexColor;
	return prevColor;
}


void YmxGraphics::_Check_Change_Batch(LPYMXTEXTURE lpTex, int vertexCount, int indiceCount)
{
	if(m_CurrVertex + vertexCount >= VB_CAPACITY || m_CurrIndice + indiceCount >= IB_CAPACTIY)
	{
		_Render_Batch(false); 
	}

	if(m_CurrTexture != lpTex)
	{
		if(m_CurrIndice > 0)
		{
			_Render_Batch(false);
		}
		m_CurrTexture = lpTex;
		if(lpTex == NULL)
			device->SetTexture(0, NULL);
		else 
			device->SetTexture(0, (LPDIRECT3DTEXTURE9)lpTex->lpTexture);

	}
}

void YmxGraphics::RenderTriangle(float x0, float y0, float x1, float y1, float x2, float y2, YMXCOLOR color, float z /*= 0.0f*/)
{
	_Check_Change_Batch(NULL, 3, 3);
	m_vertexs[m_CurrVertex]     = Vertex(x0, y0, RESTRICT_Z(z), color);
	m_vertexs[m_CurrVertex + 1] = Vertex(x1, y1, RESTRICT_Z(z), color);
	m_vertexs[m_CurrVertex + 2] = Vertex(x2, y2, RESTRICT_Z(z), color);

	m_indices[m_CurrIndice++] = m_CurrVertex++;
	m_indices[m_CurrIndice++] = m_CurrVertex++;
	m_indices[m_CurrIndice++] = m_CurrVertex++;
}

void YmxGraphics::RenderTriangle(YmxVertex* vs, YMXCOLOR color, float z /* = 0.0f */)
{
	_Check_Change_Batch(NULL, 3, 3);
	m_vertexs[m_CurrVertex]     = Vertex(vs[0].x, vs[0].y, RESTRICT_Z(z), color);
	m_vertexs[m_CurrVertex + 1] = Vertex(vs[1].x, vs[1].y, RESTRICT_Z(z), color);
	m_vertexs[m_CurrVertex + 2] = Vertex(vs[2].x, vs[2].y, RESTRICT_Z(z), color);

	m_indices[m_CurrIndice++] = m_CurrVertex++;
	m_indices[m_CurrIndice++] = m_CurrVertex++;
	m_indices[m_CurrIndice++] = m_CurrVertex++;
}

void YmxGraphics::RenderTexture(LPYMXTEXTURE lpTex, float x, float y, float z /*= 0.0f*/)
{
	_Check_Change_Batch(lpTex, 4, 6);

	float tx = lpTex->_GetTexCoordX();
	float ty = lpTex->_GetTexCoordY();

	m_vertexs[m_CurrVertex]     = Vertex(x, y, RESTRICT_Z(z), m_vertexColor, 0, 0);
	m_vertexs[m_CurrVertex + 1] = Vertex(x + lpTex->GetWidth(), y, RESTRICT_Z(z), m_vertexColor, tx, 0);
	m_vertexs[m_CurrVertex + 2] = Vertex(x + lpTex->GetWidth(), y + lpTex->GetHeight(), RESTRICT_Z(z), m_vertexColor, tx, ty);
	m_vertexs[m_CurrVertex + 3] = Vertex(x, y + lpTex->GetHeight(), RESTRICT_Z(z), m_vertexColor, 0, ty);

	m_indices[m_CurrIndice++] = m_CurrVertex;
	m_indices[m_CurrIndice++] = m_CurrVertex + 1;
	m_indices[m_CurrIndice++] = m_CurrVertex + 2;

	m_indices[m_CurrIndice++] = m_CurrVertex;
	m_indices[m_CurrIndice++] = m_CurrVertex + 2;
	m_indices[m_CurrIndice++] = m_CurrVertex + 3;

	m_CurrVertex += 4;
}

void YmxGraphics::RenderTexture(LPYMXTEXTURE lpTex, float x, float y, YmxRect* srcRect, float z /*=0.0f*/)
{
	_Check_Change_Batch(lpTex, 4, 6);

	float su = srcRect->left * lpTex->_GetMemWidthRecip();
	float eu = srcRect->right * lpTex->_GetMemWidthRecip();
	float sv = srcRect->top * lpTex->_GetMemHeightRecip();
	float ev = srcRect->bottom * lpTex->_GetMemHeightRecip(); 

	float w = srcRect->right - srcRect->left;
	float h = srcRect->bottom - srcRect->top;

	m_vertexs[m_CurrVertex]     = Vertex(x, y, RESTRICT_Z(z), m_vertexColor, su, sv);
	m_vertexs[m_CurrVertex + 1] = Vertex(x + w, y, RESTRICT_Z(z), m_vertexColor, eu, sv);
	m_vertexs[m_CurrVertex + 2] = Vertex(x + w, y + h, RESTRICT_Z(z), m_vertexColor, eu, ev);
	m_vertexs[m_CurrVertex + 3] = Vertex(x, y + h, RESTRICT_Z(z), m_vertexColor, su, ev);

	m_indices[m_CurrIndice++] = m_CurrVertex;
	m_indices[m_CurrIndice++] = m_CurrVertex + 1;
	m_indices[m_CurrIndice++] = m_CurrVertex + 2;

	m_indices[m_CurrIndice++] = m_CurrVertex;
	m_indices[m_CurrIndice++] = m_CurrVertex + 2;
	m_indices[m_CurrIndice++] = m_CurrVertex + 3;

	m_CurrVertex += 4;
}

void YmxGraphics::RenderRectangle(YmxRect* rect, YMXCOLOR color, float z/*=0.0f*/)
{
	this->RenderTriangle(rect->left, rect->top, rect->right, rect->top, rect->right, rect->bottom, color, z);
	this->RenderTriangle(rect->left, rect->top, rect->right, rect->bottom, rect->left, rect->bottom, color, z);
}

void YmxGraphics::RenderTexture(LPYMXTEXTURE lpTex, YmxRect* dstRect, YmxRect* srcRect, float z /* = 0.0f */)
{
	_Check_Change_Batch(lpTex, 4, 6);

	float su = srcRect->left * lpTex->_GetMemWidthRecip();
	float eu = srcRect->right * lpTex->_GetMemWidthRecip();
	float sv = srcRect->top * lpTex->_GetMemHeightRecip();
	float ev = srcRect->bottom * lpTex->_GetMemHeightRecip(); 

	m_vertexs[m_CurrVertex]     = Vertex(dstRect->left, dstRect->top, RESTRICT_Z(z), m_vertexColor, su, sv);
	m_vertexs[m_CurrVertex + 1] = Vertex(dstRect->right, dstRect->top, RESTRICT_Z(z), m_vertexColor, eu, sv);
	m_vertexs[m_CurrVertex + 2] = Vertex(dstRect->right, dstRect->bottom, RESTRICT_Z(z), m_vertexColor, eu, ev);
	m_vertexs[m_CurrVertex + 3] = Vertex(dstRect->left, dstRect->bottom, RESTRICT_Z(z), m_vertexColor, su, ev);

	m_indices[m_CurrIndice++] = m_CurrVertex;
	m_indices[m_CurrIndice++] = m_CurrVertex + 1;
	m_indices[m_CurrIndice++] = m_CurrVertex + 2;

	m_indices[m_CurrIndice++] = m_CurrVertex;
	m_indices[m_CurrIndice++] = m_CurrVertex + 2;
	m_indices[m_CurrIndice++] = m_CurrVertex + 3;

	m_CurrVertex += 4;
}

void YmxGraphics::RenderTexture(LPYMXTEXTURE lpTex, float xDst, float yDst, YmxRect* srcRect, float z, float rotateTheta, YmxVertex* rotateOrigin)
{
	_Check_Change_Batch(lpTex, 4, 6);

	float su = srcRect->left * lpTex->_GetMemWidthRecip();
	float eu = srcRect->right * lpTex->_GetMemWidthRecip();
	float sv = srcRect->top * lpTex->_GetMemHeightRecip();
	float ev = srcRect->bottom * lpTex->_GetMemHeightRecip(); 

	YmxVertex v[4] = {srcRect->left, srcRect->top, srcRect->right, srcRect->top,
		srcRect->right, srcRect->bottom, srcRect->left, srcRect->bottom};
	if(rotateTheta != 0.0f)
	{
		RotateVertex(&v[0], rotateOrigin, rotateTheta);
		RotateVertex(&v[1], rotateOrigin, rotateTheta);
		RotateVertex(&v[2], rotateOrigin, rotateTheta);
		RotateVertex(&v[3], rotateOrigin, rotateTheta);
	}
	m_vertexs[m_CurrVertex]     = Vertex(xDst + v[0].x, yDst + v[0].y, RESTRICT_Z(z), m_vertexColor, su, sv);
	m_vertexs[m_CurrVertex + 1] = Vertex(xDst + v[1].x, yDst + v[1].y, RESTRICT_Z(z), m_vertexColor, eu, sv);
	m_vertexs[m_CurrVertex + 2] = Vertex(xDst + v[2].x, yDst + v[2].y, RESTRICT_Z(z), m_vertexColor, eu, ev);
	m_vertexs[m_CurrVertex + 3] = Vertex(xDst + v[3].x, yDst + v[3].y, RESTRICT_Z(z), m_vertexColor, su, ev);

	m_indices[m_CurrIndice++] = m_CurrVertex;
	m_indices[m_CurrIndice++] = m_CurrVertex + 1;
	m_indices[m_CurrIndice++] = m_CurrVertex + 2;

	m_indices[m_CurrIndice++] = m_CurrVertex;
	m_indices[m_CurrIndice++] = m_CurrVertex + 2;
	m_indices[m_CurrIndice++] = m_CurrVertex + 3;

	m_CurrVertex += 4;
}

void YmxGraphics::RenderPolygon(YmxVertex* vs, int vcount, YMXCOLOR color, float z /* = 0.0f */)
{
	int triangleCount = vcount - 2;
	_Check_Change_Batch(NULL, vcount, triangleCount * 3);

	int i;
	int firstVertexIndex = m_CurrVertex; 
	z = RESTRICT_Z(z);

	for (i = 0; i < vcount; i++)
	{
		m_vertexs[m_CurrVertex++] = Vertex(vs[i].x, vs[i].y, z, color);
	}

	for (i = 0; i < triangleCount; i++)
	{
		m_indices[m_CurrIndice++] = firstVertexIndex;
		m_indices[m_CurrIndice++] = firstVertexIndex + i + 1;
		m_indices[m_CurrIndice++] = firstVertexIndex + i + 2;
	}
}

void YmxGraphics::_RenderTexture_Polygon(LPYMXTEXTURE lpTex, LPYMXPOINT points, LPYMXPOINT texCoords, int count, float z)
{
	int triangleCount = count - 2;
	_Check_Change_Batch(lpTex, count, triangleCount * 3);

	int i;
	int firstVertexIndex = m_CurrVertex; 
	z = RESTRICT_Z(z);

	for (i = 0; i < count; i++)
	{
		m_vertexs[m_CurrVertex++] = Vertex(points[i].x, points[i].y, z, m_vertexColor, texCoords[i].x * lpTex->_GetMemWidthRecip(), texCoords[i].y * lpTex->_GetMemHeightRecip());
	}

	for (i = 0; i < triangleCount; i++)
	{
		m_indices[m_CurrIndice++] = firstVertexIndex;
		m_indices[m_CurrIndice++] = firstVertexIndex + i + 1;
		m_indices[m_CurrIndice++] = firstVertexIndex + i + 2;
	}
}

float YmxGraphics::GetWidth()
{
	return m_screenWidth;
}

float YmxGraphics::GetHeight()
{
	return m_screenHeight;
}

void YmxGraphics::_PreRender_Particles(LPYMXTEXTURE pTex, bool bContainPSizeInVertex)
{
	if(pTex == NULL)
	{
		device->SetTexture(0, NULL);
	}
	else {
		device->SetTexture(0, (LPDIRECT3DTEXTURE9)pTex->lpTexture);
	}

	if(bContainPSizeInVertex) {
		device->SetStreamSource(0, PVB, 0, sizeof(ParticleVertex));
		device->SetFVF(ParticleVertex::FVF);
	}
	else {
		device->SetStreamSource(0, PVB2, 0, sizeof(ConciseVertex));
		device->SetFVF(ConciseVertex::FVF);
	}
}

void YmxGraphics::_PostRender_Particles()
{
	device->SetStreamSource(0, VB, 0, sizeof(Vertex));
	device->SetFVF(Vertex::FVF);
	if(m_CurrTexture != 0)
		device->SetTexture(0, (LPDIRECT3DTEXTURE9)m_CurrTexture->lpTexture);
}

void YmxGraphics::_Render_Particles(LPYMXTEXTURE pTex, ParticleVertex* v, int count, bool highlight)
{
	if(m_bFastParticle)
	{
		if(highlight)
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		_PreRender_Particles(pTex, true);
		device->SetRenderState(D3DRS_POINTSIZE, FtoDW(3.0f));
		device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.0f));
		device->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(300.0f));

		int batchCount = count / PVB_BATCH_SIZE; 

		ParticleVertex *pv;
		int i;
		for (i = 0; i < batchCount; i++)
		{
			PVB->Lock(m_pvbOffset * sizeof(ParticleVertex), 
				PVB_BATCH_SIZE * sizeof(ParticleVertex),
				(void**)&pv, 
				m_pvbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

			memcpy((void*)pv, (void*)v, PVB_BATCH_SIZE * sizeof(ParticleVertex));

			PVB->Unlock();

			device->DrawPrimitive(D3DPT_POINTLIST, m_pvbOffset, PVB_BATCH_SIZE);

			v += PVB_BATCH_SIZE;
			m_pvbOffset += PVB_BATCH_SIZE;
			if(m_pvbOffset >= PVB_CAPACITY) m_pvbOffset = 0;
		}

		int leftCount = count - batchCount * PVB_BATCH_SIZE;
		if(leftCount != 0)
		{
			PVB->Lock(m_pvbOffset * sizeof(ParticleVertex), 
				leftCount * sizeof(ParticleVertex),
				(void**)&pv, 
				m_pvbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

			memcpy((void*)pv, (void*)v, leftCount * sizeof(ParticleVertex));
			PVB->Unlock();
			device->DrawPrimitive(D3DPT_POINTLIST, m_pvbOffset, leftCount);

			m_pvbOffset += PVB_BATCH_SIZE;
			if(m_pvbOffset >= PVB_CAPACITY) m_pvbOffset = 0;
		}

		_PostRender_Particles();
		if(highlight)
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	else
	{
		if(highlight)
		{
			if(m_CurrIndice > 0)
				_Render_Batch(false);
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		}

		if(m_CurrTexture != pTex)
		{
			if(m_CurrIndice > 0)
			{
				_Render_Batch(false);
			}
			m_CurrTexture = pTex;
			if(pTex == NULL)
				device->SetTexture(0, NULL);
			else 
				device->SetTexture(0, (LPDIRECT3DTEXTURE9)pTex->lpTexture);
		}

		float tx = pTex->_GetTexCoordX();
		float ty = pTex->_GetTexCoordY();
		float halfSize;

		int leftParticleToRender = count;
		if(m_CurrIndice > 0)
		{
			int leftSpaceForParticle = min((VB_CAPACITY - m_CurrVertex) / 4, (IB_CAPACTIY - m_CurrIndice) / 6);
			int parNumToRender = min(leftSpaceForParticle, count);
			for (int i = 0; i < parNumToRender; i++)
			{
				halfSize = v[i].size * 0.5f;
				m_vertexs[m_CurrVertex]     = Vertex(v[i].x - halfSize, v[i].y - halfSize, v[i].z, v[i].color, 0, 0);
				m_vertexs[m_CurrVertex + 1] = Vertex(v[i].x + halfSize, v[i].y - halfSize, v[i].z, v[i].color, tx, 0);
				m_vertexs[m_CurrVertex + 2] = Vertex(v[i].x + halfSize, v[i].y + halfSize, v[i].z, v[i].color, tx, ty);
				m_vertexs[m_CurrVertex + 3] = Vertex(v[i].x - halfSize, v[i].y + halfSize, v[i].z, v[i].color, 0, ty);

				m_indices[m_CurrIndice++] = m_CurrVertex;
				m_indices[m_CurrIndice++] = m_CurrVertex + 1;
				m_indices[m_CurrIndice++] = m_CurrVertex + 2;

				m_indices[m_CurrIndice++] = m_CurrVertex;
				m_indices[m_CurrIndice++] = m_CurrVertex + 2;
				m_indices[m_CurrIndice++] = m_CurrVertex + 3;

				m_CurrVertex += 4;
			}
			leftParticleToRender -=  parNumToRender;
		}

		if(leftParticleToRender > 0)
		{
			if(m_CurrIndice > 0)
				_Render_Batch(false);

			int batchNum = leftParticleToRender * 4 / VB_CAPACITY;
			for (int j = 0; j < batchNum; j++)
			{
				for (int i = 0; i < VB_CAPACITY / 4; i++)
				{
					halfSize = v[i].size * 0.5f;
					m_vertexs[m_CurrVertex]     = Vertex(v[i].x - halfSize, v[i].y - halfSize, v[i].z, v[i].color, 0, 0);
					m_vertexs[m_CurrVertex + 1] = Vertex(v[i].x + halfSize, v[i].y - halfSize, v[i].z, v[i].color, tx, 0);
					m_vertexs[m_CurrVertex + 2] = Vertex(v[i].x + halfSize, v[i].y + halfSize, v[i].z, v[i].color, tx, ty);
					m_vertexs[m_CurrVertex + 3] = Vertex(v[i].x - halfSize, v[i].y + halfSize, v[i].z, v[i].color, 0, ty);

					m_indices[m_CurrIndice++] = m_CurrVertex;
					m_indices[m_CurrIndice++] = m_CurrVertex + 1;
					m_indices[m_CurrIndice++] = m_CurrVertex + 2;

					m_indices[m_CurrIndice++] = m_CurrVertex;
					m_indices[m_CurrIndice++] = m_CurrVertex + 2;
					m_indices[m_CurrIndice++] = m_CurrVertex + 3;

					m_CurrVertex += 4;
				}
				_Render_Batch(false);
			}

			if(batchNum * VB_CAPACITY != leftParticleToRender * 4)
			{
				leftParticleToRender = leftParticleToRender - batchNum * VB_CAPACITY / 4;
				for (int i = 0; i < leftParticleToRender; i++)
				{
					halfSize = v[i].size * 0.5f;
					m_vertexs[m_CurrVertex]     = Vertex(v[i].x - halfSize, v[i].y - halfSize, v[i].z, v[i].color, 0, 0);
					m_vertexs[m_CurrVertex + 1] = Vertex(v[i].x + halfSize, v[i].y - halfSize, v[i].z, v[i].color, tx, 0);
					m_vertexs[m_CurrVertex + 2] = Vertex(v[i].x + halfSize, v[i].y + halfSize, v[i].z, v[i].color, tx, ty);
					m_vertexs[m_CurrVertex + 3] = Vertex(v[i].x - halfSize, v[i].y + halfSize, v[i].z, v[i].color, 0, ty);

					m_indices[m_CurrIndice++] = m_CurrVertex;
					m_indices[m_CurrIndice++] = m_CurrVertex + 1;
					m_indices[m_CurrIndice++] = m_CurrVertex + 2;

					m_indices[m_CurrIndice++] = m_CurrVertex;
					m_indices[m_CurrIndice++] = m_CurrVertex + 2;
					m_indices[m_CurrIndice++] = m_CurrVertex + 3;

					m_CurrVertex += 4;
				}
			}
		}

		if(highlight)
		{
			if(m_CurrIndice > 0)
				_Render_Batch(false);
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
	}
}

void YmxGraphics::_Render_Particles(LPYMXTEXTURE pTex, ConciseVertex* v, int count, float psize, bool highlight)
{
	if(m_bFastParticle)
	{
		if(highlight)
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		_PreRender_Particles(pTex, false);

		device->SetRenderState(D3DRS_POINTSIZE, FtoDW(psize));
		device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.0f));
		device->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(psize));

		int batchCount = count / PVB_BATCH_SIZE; 

		ConciseVertex *pv;
		int i;
		for (i = 0; i < batchCount; i++)
		{
			PVB2->Lock(m_pvbOffset * sizeof(ConciseVertex), 
				PVB_BATCH_SIZE * sizeof(ConciseVertex),
				(void**)&pv, 
				m_pvbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

			memcpy((void*)pv, (void*)v, PVB_BATCH_SIZE * sizeof(ConciseVertex));

			PVB2->Unlock();

			device->DrawPrimitive(D3DPT_POINTLIST, m_pvbOffset, PVB_BATCH_SIZE);

			v += PVB_BATCH_SIZE;
			m_pvbOffset += PVB_BATCH_SIZE;
			if(m_pvbOffset >= PVB_CAPACITY) m_pvbOffset = 0;
		}

		int leftCount = count - batchCount * PVB_BATCH_SIZE;
		if(leftCount != 0)
		{
			PVB2->Lock(m_pvbOffset * sizeof(ConciseVertex), 
				leftCount * sizeof(ConciseVertex),
				(void**)&pv, 
				m_pvbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

			memcpy((void*)pv, (void*)v, leftCount * sizeof(ConciseVertex));
			PVB2->Unlock();
			device->DrawPrimitive(D3DPT_POINTLIST, m_pvbOffset, leftCount);

			m_pvbOffset += PVB_BATCH_SIZE;
			if(m_pvbOffset >= PVB_CAPACITY) m_pvbOffset = 0;
		}
		_PostRender_Particles();
		if(highlight)
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	else {

	}	
}

bool YmxGraphics::_IsSupporFVFPSize()
{
	return m_bSupportPSize;
}

void YmxGraphics::_Render_Particles(LPYMXTEXTURE pTex, YmxParticle* particles, float z, float psize, bool highlight)
{
	if(m_bFastParticle)
	{
		if(highlight)
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		_PreRender_Particles(pTex, m_bSupportPSize);

		LPYMXPARTICLE p = particles;
		if(m_bSupportPSize)
		{
			ParticleVertex* pv;
			PVB->Lock(m_pvbOffset * sizeof(ParticleVertex), 
				PVB_BATCH_SIZE * sizeof(ParticleVertex),
				(void**)&pv, 
				m_pvbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

			DWORD numParticlesInBatch = 0;
			while (p != NULL)
			{
				pv->x = p->x;
				pv->y = p->y;
				pv->z = z;
				pv->size = p->size;
				pv->color = p->color;

				// move to next particle
				pv++;
				p = p->next;

				numParticlesInBatch++;

				if(numParticlesInBatch == PVB_BATCH_SIZE)
				{
					PVB->Unlock();
					device->DrawPrimitive(D3DPT_POINTLIST, m_pvbOffset, PVB_BATCH_SIZE);

					m_pvbOffset += PVB_BATCH_SIZE;
					if(m_pvbOffset >= PVB_CAPACITY)
						m_pvbOffset = 0;

					PVB->Lock(m_pvbOffset * sizeof(ParticleVertex), 
						PVB_BATCH_SIZE * sizeof(ParticleVertex),
						(void**)&pv, 
						m_pvbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0;
				}
			}

			if(numParticlesInBatch != 0)
			{
				PVB->Unlock();
				device->DrawPrimitive(D3DPT_POINTLIST, m_pvbOffset, numParticlesInBatch);

				m_pvbOffset += PVB_BATCH_SIZE;
				if(m_pvbOffset >= PVB_CAPACITY) m_pvbOffset = 0;
			}
		}
		else {
			device->SetRenderState(D3DRS_POINTSIZE, FtoDW(psize));
			ConciseVertex* pv;
			PVB2->Lock(m_pvbOffset * sizeof(ConciseVertex), 
				PVB_BATCH_SIZE * sizeof(ConciseVertex),
				(void**)&pv, 
				m_pvbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

			DWORD numParticlesInBatch = 0;
			while (p != NULL)
			{
				pv->x = p->x;
				pv->y = p->y;
				pv->z = z;
				pv->color = p->color;

				// move to next particle
				pv++;
				p = p->next;

				numParticlesInBatch++;

				if(numParticlesInBatch == PVB_BATCH_SIZE)
				{
					PVB2->Unlock();
					device->DrawPrimitive(D3DPT_POINTLIST, m_pvbOffset, PVB_BATCH_SIZE);

					m_pvbOffset += PVB_BATCH_SIZE;
					if(m_pvbOffset >= PVB_CAPACITY)
						m_pvbOffset = 0;

					PVB2->Lock(m_pvbOffset * sizeof(ConciseVertex), 
						PVB_BATCH_SIZE * sizeof(ConciseVertex),
						(void**)&pv, 
						m_pvbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0;
				}
			}

			if(numParticlesInBatch != 0)
			{
				PVB2->Unlock();
				device->DrawPrimitive(D3DPT_POINTLIST, m_pvbOffset, numParticlesInBatch);

				m_pvbOffset += PVB_BATCH_SIZE;
				if(m_pvbOffset >= PVB_CAPACITY) m_pvbOffset = 0;
			}
		}

		_PostRender_Particles();
		if(highlight)
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	else {

		if(highlight)
		{
			if(m_CurrIndice > 0)
				_Render_Batch(false);
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		}

		if(m_CurrTexture != pTex)
		{
			if(m_CurrIndice > 0)
			{
				_Render_Batch(false);
			}
			m_CurrTexture = pTex;
			if(pTex == NULL)
				device->SetTexture(0, NULL);
			else 
				device->SetTexture(0, (LPDIRECT3DTEXTURE9)pTex->lpTexture);
		}

		YmxParticle* p = particles;
		float tx = pTex->_GetTexCoordX();
		float ty = pTex->_GetTexCoordY();
		float halfSize;
		while(p != NULL)
		{
			if(m_CurrVertex + 4 >= VB_CAPACITY || m_CurrIndice + 6 >= IB_CAPACTIY)
			{
				_Render_Batch(false);
			}

			halfSize = p->size * 0.5f;

			m_vertexs[m_CurrVertex]     = Vertex(p->x - halfSize, p->y - halfSize, RESTRICT_Z(z), p->color, 0, 0);
			m_vertexs[m_CurrVertex + 1] = Vertex(p->x + halfSize, p->y - halfSize, RESTRICT_Z(z), p->color, tx, 0);
			m_vertexs[m_CurrVertex + 2] = Vertex(p->x + halfSize, p->y + halfSize, RESTRICT_Z(z), p->color, tx, ty);
			m_vertexs[m_CurrVertex + 3] = Vertex(p->x - halfSize, p->y + halfSize, RESTRICT_Z(z), p->color, 0, ty);

			m_indices[m_CurrIndice++] = m_CurrVertex;
			m_indices[m_CurrIndice++] = m_CurrVertex + 1;
			m_indices[m_CurrIndice++] = m_CurrVertex + 2;

			m_indices[m_CurrIndice++] = m_CurrVertex;
			m_indices[m_CurrIndice++] = m_CurrVertex + 2;
			m_indices[m_CurrIndice++] = m_CurrVertex + 3;

			m_CurrVertex += 4;

			p = p->next;
		}

		if(highlight)
		{
			if(m_CurrIndice > 0)
				_Render_Batch(false);
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
	}
}