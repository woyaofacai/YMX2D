// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the YMX2D_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// YMX2D_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.


#ifdef YMX2D_EXPORTS
#define YMX2D_API __declspec(dllexport)
#else
#define YMX2D_API __declspec(dllimport)
#endif

#include "ymxenum.h"

#ifndef YMX2D_FUNCTION_CLASS_H
#define YMX2D_FUNCTION_CLASS_H

YMX2D_API inline float SinFast(float rad);
YMX2D_API inline float CosFast(float rad);
YMX2D_API inline float SinFastByAngle(float ang);	//calculate by angle unit, not radian
YMX2D_API inline float CosFastByAngle(float ang);
YMX2D_API inline float GetRandomFloat(float, float);
YMX2D_API inline int GetRandomInteger(int x1, int x2);
YMX2D_API inline void MessageBoxPrintfA(const char* caption, const char* format, ...);
YMX2D_API inline void MessageBoxPrintfW(const wchar_t* caption, const wchar_t* format, ...);
YMX2D_API inline void RecordErrorA(const char* caption, const char* format, ...);
YMX2D_API inline void RecordErrorW(const wchar_t* caption, const wchar_t* format, ...);

#ifdef _UNICODE
#define MessageBoxPrintf MessageBoxPrintfW
#define RecordError RecordErrorW
#else
#define MessageBoxPrintf MessageBoxPrintfA
#define RecordError RecordErrorA
#endif

class YMX2D_API YmxGame;
class YMX2D_API YmxGraphics;
class YMX2D_API YmxFont;
class YMX2D_API YmxSprite;
class YMX2D_API YmxComponent;
class YMX2D_API YmxParticleSystem;
class YMX2D_API YmxButton;
class YMX2D_API YmxTexture;
class YMX2D_API YmxListView;
class YMX2D_API YmxEditText;
class YMX2D_API YmxLuaConfig;
class Vertex;
class ConciseVertex;
class ParticleVertex;
class YMX2D_API YmxParticle;
class YMX2D_API YmxSound;
class YmxSnowParticleSystem;
class YmxFlameParticleSystem;
class YmxExplodeParticleSystem;
class YmxCometParticleSystem;

typedef YmxFont YMXFONT, *LPYMXFONT;
typedef YmxGraphics YMXGRAPHICS, *LPYMXGRAPHICS;
typedef YmxSprite YMXSPRITE, *LPYMXSPRITE;
typedef YmxComponent YMXCOMPONENT, *LPYMXCOMPONENT;
typedef YmxParticleSystem  PARTICLEYMXTEM, *LPPARTICLEYMXTEM;
typedef YmxTexture YMXTEXTURE, *LPYMXTEXTURE;
typedef YmxParticle YMXPARTICLE, PARTICLE, *LPYMXPARTICLE, *LPPARTICLE;
typedef YmxSound *LPYMXSOUND;

typedef bool (*UPDATE_FUNCTION)(float);
typedef void (*RENDER_FUNCTION)(YmxGraphics*);
typedef DWORD YMXCOLOR;

#define YMX_ARGB(a,r,g,b) \
	((YMXCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define YMX_RGB(r,g,b) YMX_ARGB(0xff,r,g,b)

#define EPSILON 0.00001f
#define RESTRICT_Z(z) (max(0.0f, min(z, 1.0f)))
#define GET_ALPHA(color) (color >> 24)
#define GET_RED(color)   ((color & 0x00FF0000) >> 16)
#define GET_GREEN(color) ((color & 0x0000FF00) >> 8)
#define GET_BLUE(color)  ((color & 0x000000FF))
#define TWO_FLOATS_EQUAL(x1, x2) (((x1) - (x2)) > -EPSILON && ((x1) - (x2)) < EPSILON)
#define RESTRICT_RGB(x)  max(0, min(x, 255))

#define PI 3.1415926f
#define M_2_PI 6.2831852f
#define DIV_PI_180 0.017453f
#define DIV_180_PI 57.29578f

#define FToRGB(x) ((int)(max(0.0f, min(x, 1.0f)) * 255))

YMX2D_API YmxGame* CreateGame();
YMX2D_API inline YmxGame* GetGameObject();

typedef struct YMX2D_API YmxVertex
{
	float x;
	float y;
} YMXVERTEX, YMXPOINT, *LPYMXVERTEX, *LPYMXPOINT;

YMX2D_API void RotateVertex(YmxVertex* v, YmxVertex* pivot_v, float theta);

typedef struct YMX2D_API YmxVector
{
public:
	float x;
	float y;
	inline YmxVector();
	inline YmxVector(float x, float y);
	inline float GetLength();
	inline float GetLengthSq();
	inline void Normalize();
	inline YmxVector operator+(const YmxVector&) const; 
	inline YmxVector operator-(const YmxVector&) const;
	inline YmxVector operator*(float a) const;
	inline float operator*(const YmxVector&) const;
	inline YmxVector& operator+=(const YmxVector&);
	inline YmxVector& operator-=(const YmxVector&);
	inline YmxVector& operator*=(float a);
	inline YmxVector operator-() const;
	inline bool operator==(const YmxVector&) const;
	inline bool operator!=(const YmxVector&) const;

} YMXVECTOR, *LPYMXVECTOR;

typedef struct YMX2D_API YmxQuad
{
	YmxVertex v[4];
}YMXQUAD, *LPYMXQUAD;


typedef struct YMX2D_API YmxRect
{
	float left;
	float right;
	float top;
	float bottom;
	inline YmxRect();
	inline YmxRect(float left, float top, float right, float bottom);
	inline bool Intersect(const YmxRect& rect) const;
	inline bool IsPointIn(float x, float y) const;
} YMXRECT, *LPYMXRECT;

typedef struct YMX2D_API YmxParticleAttribute
{
	YMXVECTOR velocity;
	YMXVECTOR acceleration;
	float life;
};

struct YMX2D_API YmxTexture
{
	friend YmxGame;
	friend YmxGraphics;
	friend YmxButton;
private:
	LPVOID lpTexture;
	float originalWidth;
	float originalHeight;
	float memWidth;
	float memHeight;
	float memWidthRecip;
	float memHeightRecip;
	float texCoordX;
	float texCoordY;
	//	float alpha;
	YmxTexture(){};
	YmxTexture(YmxGame&){};
	static LPYMXTEXTURE LoadTexture(PCTSTR fileName);
	inline float _GetMemWidthRecip();
	inline float _GetMemHeightRecip();
	inline float _GetTexCoordX();
	inline float _GetTexCoordY();
public: 
	inline float GetMemWidth();
	inline float GetMemHeight();
	inline float GetWidth();
	inline float GetHeight();
	void SetAlpha(float a);
	void Release();
};

class YMX2D_API YmxGame  
{
	friend YmxGraphics;
	friend YmxGame* CreateGame();
	friend YmxGame* GetGameObject();
	friend void RecordErrorA(const char* caption, const char* format, ...);
	friend void RecordErrorW(const wchar_t* caption, const wchar_t* format, ...);
public:
	virtual ~YmxGame();
	bool Start();
	bool Initialize();
	
	void SetRenderFunction(RENDER_FUNCTION);
	void SetUpdateFunction(UPDATE_FUNCTION);
	void Release();
	inline void SetAttribute(YMXGAME_ATTRIBUTE attr, PCTSTR value);
	inline void SetAttribute(YMXGAME_ATTRIBUTE attr, int value);
	inline void SetAttribute(YMXGAME_ATTRIBUTE attr, bool value);
	inline float GetWindowWidth();
	inline float GetWindowHeight();
	LPYMXTEXTURE LoadTexture(PCTSTR fileName);
	LPYMXFONT CreateFont(PCTSTR fntFileName);
	LPYMXSPRITE CreateSprite(LPYMXTEXTURE pTexture);
	LPYMXSPRITE CreateSprite(LPYMXTEXTURE pTexture, YMXRECT& textureRect);
	LPPARTICLEYMXTEM CreateParticleSystem(PARTICLESYSTEM_TYPE pst, LPYMXTEXTURE pTexture, int maxCount);
	LPYMXSOUND CreateSound(PTSTR filename);
	inline void GetMousePos(float* x, float *y);
	inline void SetMousePos(float x, float y);
	inline bool KeyDown(int key);
	inline bool KeyUp(int key);
	inline bool GetKeyState(int key);
	inline int GetInputKey(); 
	inline TCHAR GetInputChar();
	bool AddComponent(LPYMXCOMPONENT pComponent, bool bInit = false);
	inline LPYMXCOMPONENT GetComponentById(int id);
	inline LPYMXCOMPONENT GetComponentByName(const char* name);
	inline bool _IsSupporFVFPSize();
	inline float GetUpdateDelta();
	inline YmxLuaConfig* GetLuaConfig();
	inline void Log(PCTSTR szFormat, ...);
	inline void LogA(const char* szFormat, ...);
	inline void LogW(const wchar_t* szFormat, ...);

	bool RemoveComponent(LPYMXCOMPONENT pComponent, bool release = true);
	bool RemoveComponent(int id, bool release = true);
	bool RemoveComponent(const char* name, bool release = true);
	bool AddLuaComponent(const char* name, bool bInit = false); 

private:
	YmxGame();
	YmxGame(YmxGame&);

	static LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
	void _Render();
	inline void _Input_Clear();
	inline bool _Handle_Input(UINT msg, WPARAM wParam, LPARAM lParam);
	inline static YmxGame* _GetInstance();
	
	UPDATE_FUNCTION updateFunction;
	RENDER_FUNCTION	renderFunction;

	//Graphics
	TCHAR m_szWindowTitle[201];
	TCHAR m_szLogFile[201];
	static YmxGraphics* graphics;

	HICON m_hIcon;
	HICON m_hCursor;

	bool m_bFullScreen;
	bool m_bActive;
	bool m_bLoadLuaComponents;
	bool m_bShowCursor;
	bool m_bLogError;

	int m_keys[256];
	int m_lastKey;
	TCHAR m_lastChar;
	float m_mousePosX;
	float m_mousePosY;

	float m_Delta;

	LPYMXCOMPONENT m_pRootComponent;	//the Proxy Component in YmxGame Object 
	bool m_bSupportLuaConfig;

	YmxLuaConfig* m_LuaConfig;
};

class YMX2D_API YmxGraphics  
{
	friend YmxGame;
	friend YmxParticleSystem;
	friend YmxSnowParticleSystem;
	friend YmxFlameParticleSystem;
	friend YmxExplodeParticleSystem;
	friend YmxCometParticleSystem;
	friend YmxSprite;
public:
	~YmxGraphics();
	inline float GetWidth();
	inline float GetHeight();
	inline void Clear(YMXCOLOR color);
	//inline static void RotateVertex(YmxVertex* v, YmxVertex* pivot_v, float theta);
	inline YMXCOLOR SetVertexColor(YMXCOLOR vColor);
	inline void RenderTriangle(float x0, float y0, float x1, float y1, float x2, float y2, YMXCOLOR, float z = 0.0f);
	inline void RenderTriangle(YmxVertex* vs, YMXCOLOR, float z = 0.0f);
	inline void RenderRectangle(YmxRect* rect, YMXCOLOR color, float z = 0.0f);
	inline void RenderPolygon(YmxVertex* vs, int vcount, YMXCOLOR color, float z = 0.0f);
	inline void RenderTexture(LPYMXTEXTURE lpTex, float x, float y, float z = 0.0f);
	inline void RenderTexture(LPYMXTEXTURE lpTex, float x, float y, YmxRect* srcRect, float z = 0.0f);
	inline void RenderTexture(LPYMXTEXTURE lpTex, YmxRect* dstRect, YmxRect* srcRect, float z = 0.0f);
	inline void RenderTexture(LPYMXTEXTURE lpTex, float xDst, float yDst, YmxRect* srcRect, float z, float rotateTheta, YmxVertex* rotateOrigin);
	
private:
	YmxGraphics();
	YmxGraphics(YmxGraphics&);
	inline void _Check_Change_Batch(LPYMXTEXTURE lpTex, int vertexCount, int indiceCount);
	inline void _Render_Batch(bool);
	inline void _PreRender_Particles(LPYMXTEXTURE pTex, bool bContainPSizeInVertex);
	inline void _PostRender_Particles();
	void _PipelineTransform(); 
	inline bool _IsSupporFVFPSize();
	bool _D3D_Initialize();
	inline void _Render_Begin(); 
	inline void _Render_End(); 
	
	inline void _Render_Particles(LPYMXTEXTURE pTex, ParticleVertex* v, int count, bool highlight = false);
	inline void _Render_Particles(LPYMXTEXTURE pTex, ConciseVertex* v, int count, float psize, bool highlight = false);
	inline void _Render_Particles(LPYMXTEXTURE pTex, YmxParticle* particles, float z, float psize = 0.0f, bool highlight = false);
	inline void _RenderTexture_Polygon(LPYMXTEXTURE lpText, LPYMXPOINT points, LPYMXPOINT texCoords, int count, float z);
	YmxGame* m_game;
	int m_CurrIndice;
	int m_CurrVertex;
	Vertex* m_vertexs;
	WORD* m_indices; 
	float m_screenWidth, m_screenHeight;

	LPYMXTEXTURE m_CurrTexture;
	YMXCOLOR m_vertexColor;

	//for rendering particles
	int m_pvbOffset;
	Vertex* m_ParVertexs;
	bool m_bSupportPointSprite;
	bool m_bSupportPSize;

	bool m_bFastParticle;
	bool m_CurBatch;
};

typedef struct YMX2D_API YmxChar
{
	float x;
	float y; 
	float width;
	float height;
	TCHAR c;
} YMXCHAR, *LPYMXCHAR;

typedef struct YMX2D_API YmxCharGroup
{
	DWORD charCount;
	DWORD countPerRow;
	DWORD width;
	DWORD height;
	DWORD nRow;
	DWORD nCol;
	DWORD tmHeight;
	DWORD tmExternalLeading;
	DWORD tmAveCharWidth;
	COLORREF color;
	LOGFONT lf;
	YmxChar chars[1];
} CHARGROUP, YMXCHARGROUP, *LPYMXCHARGROUP, *LPCHARGROUP;

class YMX2D_API YmxFont
{ 
	friend class YmxButton;
	friend class YmxListView; 
	friend class YmxEditText;
	friend class YmxGame;
public:
	~YmxFont();
	void Release();
	void SetText(PTSTR str);
	PCTSTR GetText();
	void Render(LPYMXGRAPHICS g, float x, float y); 
	inline void SetZ(float z);
	inline float GetZ();
	inline void SetColor(YMXCOLOR color);
	inline YMXCOLOR GetColor();
	bool SetAttribute(YMXFONT_ATTRIBUTE attr, int value);
	bool SetAttribute(YMXFONT_ATTRIBUTE attr, bool value);
	inline void SetLineSpacing(float lineSpacing);
	inline void SetCharSpacing(float charSpacing);
	inline float GetLineSpacing();
	inline float GetCharSpacing();
	inline float GetWidth();
	inline float GetHeight();
	inline LPYMXCOMPONENT GetComponent();
	static LPYMXFONT Load_From_File(PCTSTR szFntFileName, LPYMXCOMPONENT pComponent);
private:
	YmxFont(LPYMXCHARGROUP chargrp, LPYMXTEXTURE lpTexture, LPYMXCOMPONENT pComponent);
	YmxFont(YmxFont&);
	inline void _Render(LPYMXGRAPHICS g, float x, float y);
	inline LPYMXCHAR _FindYmxChar(TCHAR c); 
	inline bool _SetStrBufCount(int count);
	inline void _RenderChar(LPYMXGRAPHICS g, LPYMXCHAR pChar, float x, float y);
	inline void _RenderChar(LPYMXGRAPHICS g, LPYMXCHAR pChar, float x, float y, bool bHclip, bool bVclip, YMXRECT& bordaryRect);
	inline void _ReleaseBuffer();
	inline int _FindStrInBuffer(PTSTR str);
	inline int _GetCharOriginWidth(int index);
	inline bool _IsCurrStrSingleLine();
	inline void _RenderSingleLine(LPYMXGRAPHICS g, float x, float y, int startIndex, int endIndex, YMXRECT &bordaryRect, bool bOnBordary);
	inline bool _RenderSingleAutoNewLine(LPYMXGRAPHICS g, float x, float y, int start, int *end, YMXRECT &bordaryRect, float rightBordary, bool bOnBordary);
	inline int _GetCurrentIndex();
	inline void _SetTextExceptIndex(PTSTR str, int index);
	inline void _SetCurrentIndex(int index);
	inline const YMXRECT& _GetBoundingBox();
	PTSTR* m_pStrsBuf;
	LPYMXCHAR** m_ppYmxCharsBuf; 
	LPYMXTEXTURE m_pTexture; 
	LPYMXCHARGROUP m_pChargrp;
	float m_z;
	int m_length;
	int m_BufferCount;
	int m_currBufIndex;
	int m_bufCursor;
	bool m_bMultiline;
	bool m_bAutoNewline;
	float m_LineSpacing; // The spacing between two lines
	float m_CharSpacing;  // The spacing between two columns
	int m_MaxCharCount;  //The max count to print. if it equals -1, no restrict
	float m_Scale;
	LPYMXCOMPONENT m_pComponent;
};

class YMX2D_API YmxSprite
{
	friend class YmxGame;
	friend class YmxComponent;
public:
	void Render(LPYMXGRAPHICS g, float x, float y);
	void Render(LPYMXGRAPHICS g, float x, float y, float rot, float hScale, float vScale);
	~YmxSprite();
	void Release();
	inline void SetTextureRect(YMXRECT& rect);
	inline const YMXRECT& GetTextureRect();
	inline float GetWidth();  
	inline float GetHeight();
	inline void SetHotSpot(float x, float y);
	inline YMXPOINT GetHotSpot();
	inline void SetZ(float z);
	inline float GetZ();
	inline LPYMXTEXTURE GetTexture();
	inline LPYMXCOMPONENT GetComponent();
private:
	YmxSprite(LPYMXTEXTURE pTexture, YMXRECT& srcRect, LPYMXCOMPONENT pComponent);
	inline bool _InsideBordary(YMXBORDARY bordary, LPYMXPOINT p);
	inline YMXPOINT _Intersect_Bordary_Point(YMXBORDARY bordary, LPYMXPOINT p1, LPYMXPOINT p2);
	inline int _Clip_By_Bordary(YMXBORDARY bordary, LPYMXPOINT ptsIn, int ptsInCount, LPYMXPOINT ptsOut);
	inline bool _Quad_Inside(LPYMXQUAD pQuad);
	LPYMXTEXTURE m_pTexture;
	YMXRECT m_TextureRect;
	float m_z;
	YMXVERTEX m_hotSpot;
	LPYMXCOMPONENT m_pComponent;
};

class YMX2D_API YmxComponent
{ 
	friend class YmxSprite;
	friend class YmxFont;
	friend class YmxGame;
	friend class YmxButton;
	friend class YmxListView;

public:
	YmxComponent(int id, LPYMXCOMPONENT pParentComponent, char* name = NULL, UINT order = 0);
	virtual ~YmxComponent();
	inline void SetVisible(bool);
	inline void SetEnabled(bool);
	inline bool IsVisible();
	inline bool IsEnabled();
	inline void SetBoundingBox(YMXRECT& rect);
	inline const YMXRECT& GetBoundingBox();
	inline LPYMXCOMPONENT GetParent();
	inline int GetId();
	inline const char* GetName();
	inline float GetWidth();
	inline float GetHeight(); 
	inline float GetZ();
	inline int GetOrder();
	inline void SetZ(float z); 
	inline void SetBackgroundColor(YMXCOLOR color);
	LPYMXTEXTURE LoadTexture(PCTSTR szFileName);
	LPYMXSPRITE CreateSprite(LPYMXTEXTURE pTexture);
	LPYMXSPRITE CreateSprite(LPYMXTEXTURE pTexture, YMXRECT& textureRect);
	LPYMXFONT CreateFont(PCTSTR szFntFileName);
	LPYMXSOUND CreateSound(PTSTR filename);
	LPPARTICLEYMXTEM CreateParticleSystem(PARTICLESYSTEM_TYPE pst, LPYMXTEXTURE pTexture, int maxCount);
	YmxButton* CreateButton(UINT id, LPYMXTEXTURE, LPYMXFONT, float x, float y);
	YmxListView* CreateListView(UINT id, LPYMXTEXTURE, LPYMXFONT, float x, float y);
	YmxEditText* CreateEditText(UINT id, LPYMXFONT, float x, float y);
	inline void GetMousePos(float* x, float *y);
	inline void SetMousePos(float x, float y); 
	inline bool KeyDown(int key);
	inline bool KeyUp(int key); 
	inline bool GetKeyState(int key);
	inline int GetInputKey();
	inline TCHAR GetInputChar();
	inline bool SetAttribute(YMXCOMPONENT_ATTRIBUTE attr, bool value);
	void Release();
	inline LPYMXCOMPONENT GetComponentById(int id);
	inline LPYMXCOMPONENT GetComponentByName(const char* name);

	virtual bool OnMessage(int msgid, DWORD notifyCode, PVOID param); 
	static inline bool SendMessage(int msgid, int componentId, DWORD notifyCode, PVOID param);
	static inline bool SendMessage(int msgid, const char* compoName, DWORD notifyCode, PVOID param);
	static inline bool SendMessage(int msgid, LPYMXCOMPONENT pTarget, DWORD notifyCode, PVOID param);
	static inline bool BroadcastMessage(int msgid, int componentId, DWORD notifyCode, PVOID param, DWORD flag);
	static inline bool BroadcastMessage(int msgid, const char* compoName, DWORD notifyCode, PVOID param, DWORD flag);
	static inline bool BroadcastMessage(int msgid, LPYMXCOMPONENT pTarget, DWORD notifyCode, PVOID param, DWORD flag);

	bool AddComponent(LPYMXCOMPONENT pComponent, bool bInit = false);
	bool RemoveComponent(LPYMXCOMPONENT pComponent, bool bRelease = true);
	bool RemoveComponent(int id, bool bRelease = true);
	bool RemoveComponent(const char* name, bool bRelease = true);
protected:
	virtual bool Update(float delta);
	virtual void Render(YmxGraphics* g);
	virtual void Initialize();
	virtual void LoadContent();
	virtual void UnloadContent();
private: 
	void _Release();
	void _Initialize();
	void _LoadContent();
	bool _Update(float delta);
	void _Render(YmxGraphics* g);
	void _RemoveFromParent(); 
	inline bool _OnMessage(int msgid, DWORD notifyCode, PVOID param);
	static bool _BroadcastMessage(int msgid, LPYMXCOMPONENT pComponent, DWORD notifyCode, PVOID param, DWORD flag);
	const int m_id;
	char* m_Name;
	bool m_bEnabled;
	bool m_bVisible;
	UINT m_Order;
	LPYMXCOMPONENT m_Parent;
	LPYMXCOMPONENT m_Next;
	LPYMXCOMPONENT m_Children;
	YMXRECT m_BoundingBox;
	bool m_bBkgTransparent;
	YMXCOLOR m_bkgColor;
	bool m_bRequireClip; 
	float m_z;
	bool m_bInitReady;
	bool m_bLoadContentReady;
};

#endif