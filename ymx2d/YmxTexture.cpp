#include "stdafx.h"
#include "ymx2d.h"


LPYMXTEXTURE YmxTexture::LoadTexture(PCTSTR fileName)
{
	LPDIRECT3DTEXTURE9 lpTexture;
	D3DXIMAGE_INFO imageInfo;
	HRESULT hr = D3DXCreateTextureFromFileEx(
		device, 
		fileName,  
		D3DX_DEFAULT,  
		D3DX_DEFAULT, D3DX_DEFAULT, 
		0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
		D3DX_FILTER_NONE, D3DX_DEFAULT, 
		0, &imageInfo, NULL, &lpTexture);
	if(FAILED(hr))
	{
		MessageBox(hwnd, TEXT("Cant load texture"), NULL, NULL);
		return NULL;
	}

	LPYMXTEXTURE lpYmxTexture = new YMXTEXTURE;
	lpYmxTexture->originalWidth = imageInfo.Width;
	lpYmxTexture->originalHeight = imageInfo.Height;
	lpYmxTexture->lpTexture = (LPVOID)lpTexture;

	D3DSURFACE_DESC desc;
	lpTexture->GetLevelDesc(0, &desc);
	lpYmxTexture->memWidth = desc.Width;
	lpYmxTexture->memHeight = desc.Height;
	lpYmxTexture->memWidthRecip = 1.0f / lpYmxTexture->memWidth;
	lpYmxTexture->memHeightRecip = 1.0f / lpYmxTexture->memHeight;
	lpYmxTexture->texCoordX = lpYmxTexture->originalWidth * lpYmxTexture->memWidthRecip;
	lpYmxTexture->texCoordY = lpYmxTexture->originalHeight * lpYmxTexture->memHeightRecip;

	return lpYmxTexture;
}

void YmxTexture::SetAlpha(float a)
{
	int alpha = int(a * 255);
	alpha = max(1, min(alpha, 255));

	D3DSURFACE_DESC desc;
	((LPDIRECT3DTEXTURE9)lpTexture)->GetLevelDesc(0, &desc);

	D3DFORMAT format = desc.Format;
	if(format != D3DFMT_A8R8G8B8 && format != D3DFMT_A8B8G8R8)
	{
		return;
	}

	int mask = (alpha << 24);
	RECT rect = {0, 0, originalWidth, originalHeight};
	D3DLOCKED_RECT lockedRect;

	if(FAILED(((LPDIRECT3DTEXTURE9)lpTexture)->LockRect(0, &lockedRect, &rect, 0)))
	{
		MessageBox(hwnd, TEXT("Texture Lock Failed!"), NULL, NULL);
		return;
	}
	DWORD* pBits = (DWORD*)lockedRect.pBits;
	int pitch = lockedRect.Pitch >> 2;

	int width = rect.right - rect.left;
	int i, j;
	for (j = 0; j < GetHeight(); j++)
	{
		for (i = 0; i < GetWidth(); i++)
		{
			DWORD value = pBits[j * pitch + i];
			if(value & 0xFF000000)
			{
				pBits[j * pitch + i] = (value & 0x00FFFFFF) | mask;
			}
		}
	}

	((LPDIRECT3DTEXTURE9)lpTexture)->UnlockRect(0);
}

void YmxTexture::Release()
{
	if(lpTexture != NULL)
	{
		((LPDIRECT3DTEXTURE9)lpTexture)->Release();
		lpTexture = NULL;
	}
	delete this;
}

float YmxTexture::GetWidth()
{
	return originalWidth;
}

float YmxTexture::GetHeight()
{
	return originalHeight;
}

float YmxTexture::GetMemWidth()
{
	return memWidth;
}

float YmxTexture::GetMemHeight()
{
	return memHeight;
}

float YmxTexture::_GetMemWidthRecip()
{
	return memWidthRecip;
}

float YmxTexture::_GetMemHeightRecip()
{
	return memHeightRecip;
}

float YmxTexture::_GetTexCoordX()
{
	return texCoordX;
}

float YmxTexture::_GetTexCoordY()
{
	return texCoordY;
}
