#include "stdafx.h"
#include "ymx2d.h"
#include "ymxform.h"
#include "ymxparticle.h"

/*
* Because use BFS when broadcast messages through Component, so we need 
* define the queue data structure and the functions operate the queue
*/
typedef struct ComponentQueueNode
{
	LPYMXCOMPONENT pComponent;
	ComponentQueueNode *next;
}COMPONENT_QUEUE_NODE, *LPCOMPONENT_QUEUE_NODE;

static LPYMXCOMPONENT Pop_From_ComponentQueue(LPCOMPONENT_QUEUE_NODE* head, LPCOMPONENT_QUEUE_NODE* tail)
{
	if(head == NULL || *head == NULL) return NULL;
	LPYMXCOMPONENT pComponent;
	if(*head == *tail)
	{
		pComponent = (*head)->pComponent;
		delete *head;
		*head = *tail = NULL;
		return pComponent;
	}

	LPCOMPONENT_QUEUE_NODE node = *head;
	pComponent = (*head)->pComponent;
	*head = (*head)->next;
	delete node;
	return pComponent;
}

static void Add_To_ComponentQueue(LPCOMPONENT_QUEUE_NODE *head, LPCOMPONENT_QUEUE_NODE* tail, LPYMXCOMPONENT pComponent)
{
	LPCOMPONENT_QUEUE_NODE p = new ComponentQueueNode;
	p->next = NULL;
	p->pComponent = pComponent;

	if(head == NULL || *head == NULL)
	{	
		*head = *tail = p;
		return;
	}

	(*tail)->next = p;
	*tail = p;
}

static void Clear_ComponentQueue(LPCOMPONENT_QUEUE_NODE *head, LPCOMPONENT_QUEUE_NODE *tail)
{
	LPCOMPONENT_QUEUE_NODE p = *head;
	LPCOMPONENT_QUEUE_NODE p2 = NULL;
	while(p != NULL)
	{
		p2 = p;
		p = p->next;
		delete p2;
	}
	*tail = *head = NULL;
}

static LPCOMPONENT_QUEUE_NODE g_cqHead = NULL, g_cqTail = NULL;

YmxComponent::YmxComponent(int id, LPYMXCOMPONENT pParentComponent, char* name /*=NULL*/, UINT order /*=0*/)
	:m_id(id), m_Order(order)
{
	m_bRequireClip = false;
	m_Children = NULL;
	m_Next = NULL; 
	m_Name = NULL;
	m_bVisible = true;
	m_bEnabled = true;
	m_bBkgTransparent = true;
	m_bkgColor = YMX_RGB(0, 0, 0);
	m_Parent = NULL;
	m_bInitReady = false;
	m_bLoadContentReady = false;

	// If have parent, set the same bounding box and z-value as parent component
	if(pParentComponent != NULL) 
	{
		pParentComponent->AddComponent(this);
		m_BoundingBox = pParentComponent->m_BoundingBox;
		m_z = pParentComponent->GetZ();
	}
	else {
		m_Parent = NULL;
		m_BoundingBox = YMXRECT(0, 0, GetGameObject()->GetWindowWidth(), GetGameObject()->GetWindowHeight());
		m_z = 0.0f;
	}
	//set Name
	if(name != NULL) {
		int nameLen = strlen(name);
		m_Name = (char*)malloc((nameLen + 1) * sizeof(char));
		strcpy(m_Name, name);
	}

}

YmxComponent::~YmxComponent()
{
	free(m_Name);
}

void YmxComponent::SetBoundingBox(YMXRECT& rect)
{
	m_BoundingBox = rect;
}

LPYMXSPRITE YmxComponent::CreateSprite(LPYMXTEXTURE pTexture)
{
	YMXRECT textureRect(0, 0, pTexture->GetWidth(), pTexture->GetHeight());
	return CreateSprite(pTexture, textureRect);
}

LPYMXSPRITE YmxComponent::CreateSprite(LPYMXTEXTURE pTexture, YMXRECT& textureRect)
{
	LPYMXSPRITE sprite = new YmxSprite(pTexture, textureRect, this);
	sprite->SetZ(m_z);
	return sprite;
}

void YmxComponent::Initialize()
{

}

void YmxComponent::_Initialize()
{
	// if the component have not been initialized, initialize it.
	if(!m_bInitReady)
	{
		m_bInitReady = true;
		Initialize();
	}

	// Initialize the children components recursively.
	LPYMXCOMPONENT p = m_Children;
	while(p != NULL)
	{
		p->_Initialize();
		p = p->m_Next;
	}
}

void YmxComponent::_Render(YmxGraphics* g)
{
	if(!m_bVisible) 
		return;

	// if the component is not transparent, render a large rectangle as the background color
	if(!m_bBkgTransparent)
		g->RenderRectangle(&m_BoundingBox, m_bkgColor, m_z);

	Render(g);

	// Render the children recursively
	LPYMXCOMPONENT p = m_Children;
	while(p != NULL)
	{
		p->_Render(g);
		p = p->m_Next;
	}
}

bool YmxComponent::_Update(float delta)
{
	if(!m_bEnabled) 
		return true;

	// The update function returns false means game prepare to exit
	if(!Update(delta)) 
		return false;
	LPYMXCOMPONENT p = m_Children;
	while(p != NULL)
	{
		if(!p->_Update(delta)) return false;
		p = p->m_Next;
	}
	return true;
}

void YmxComponent::Render(YmxGraphics* g)
{

}

bool YmxComponent::Update(float delta)
{
	return true;
}

void YmxComponent::_RemoveFromParent()
{
	if(m_Parent != NULL)
	{
		LPYMXCOMPONENT p1 = m_Parent->m_Children;
		LPYMXCOMPONENT p2 = NULL;

		if(p1 == this)
		{
			p1 = p1->m_Next;
			m_Parent->m_Children = p1;
		}
		else 
		{
			while(p1->m_Next != this)
			{
				p1 = p1->m_Next;
			}
			p1->m_Next = this->m_Next;
		}

		m_Parent = NULL;
	}
}

void YmxComponent::SetVisible(bool visible)
{
	m_bVisible = visible;
}

void YmxComponent::SetEnabled(bool enable)
{
	m_bEnabled = enable;
}

bool YmxComponent::IsVisible()
{
	return m_bVisible;
}

bool YmxComponent::IsEnabled()
{
	return m_bEnabled;
}

const YMXRECT& YmxComponent::GetBoundingBox()
{
	return m_BoundingBox;
}

float YmxComponent::GetWidth()
{
	return m_BoundingBox.right - m_BoundingBox.left;
}

float YmxComponent::GetHeight()
{
	return m_BoundingBox.bottom - m_BoundingBox.top;
}

int YmxComponent::GetId()
{
	return m_id;
}

void YmxComponent::GetMousePos(float* x, float *y)
{
	GetGameObject()->GetMousePos(x, y);
	*x -= m_BoundingBox.left;
	*y -= m_BoundingBox.top;
}

void YmxComponent::SetMousePos(float x, float y)
{
	x += m_BoundingBox.left;
	y += m_BoundingBox.top;
	GetGameObject()->SetMousePos(x, y);
}

bool YmxComponent::KeyUp(int key)
{
	return GetGameObject()->KeyUp(key);
}

bool YmxComponent::KeyDown(int key)
{
	return GetGameObject()->KeyDown(key);
}

int YmxComponent::GetInputKey()
{
	return GetGameObject()->GetInputKey();
}

TCHAR YmxComponent::GetInputChar()
{
	return GetGameObject()->GetInputChar();
}

bool YmxComponent::GetKeyState(int key)
{
	return GetGameObject()->GetKeyState(key);
}

bool YmxComponent::SetAttribute(YMXCOMPONENT_ATTRIBUTE attr, bool value)
{
	switch(attr)
	{
	case COMPONENT_REQUIRE_CLIP:
		m_bRequireClip = value;
		return true;
	case COMPONENT_VISIBLE:
		m_bVisible = value;
		return true;
	case COMPONENT_ENABLE:
		m_bEnabled = value;
		return true;
	case COMPONENT_BACKGROUND_TRANSPARENT:
		m_bBkgTransparent = value;
	default:
		break;
	}
	return false;
}

LPYMXTEXTURE YmxComponent::LoadTexture(PCTSTR szFileName)
{
	return GetGameObject()->LoadTexture(szFileName);
}

void YmxComponent::SetZ(float z)
{
	m_z = RESTRICT_Z(z);
}

float YmxComponent::GetZ()
{
	return m_z;
}

void YmxComponent::SetBackgroundColor(YMXCOLOR color)
{
	m_bkgColor = color;
}

void YmxComponent::LoadContent()
{

}

void YmxComponent::_LoadContent()
{
	if(!m_bLoadContentReady)
	{
		LoadContent();
		m_bLoadContentReady = true;
	}
	
	LPYMXCOMPONENT p = m_Children;
	while(p != NULL)
	{
		p->_LoadContent();
		p = p->m_Next;
	}
}

void YmxComponent::UnloadContent()
{

}

void YmxComponent::Release()
{
	if(m_Parent != NULL)
	{ 
		_RemoveFromParent();
	}
	_Release();
}

void YmxComponent::_Release()
{
	LPYMXCOMPONENT p1 = m_Children, p2 = NULL;
	
	while(p1 != NULL)
	{
		p2 = p1->m_Next;
		p1->_Release();
		p1 = p2;
	}

	UnloadContent();
	delete this;
}

LPYMXCOMPONENT YmxComponent::GetComponentById(int id)
{
	if(m_id == id) return this;
	LPYMXCOMPONENT p = m_Children;
	LPYMXCOMPONENT pResult = NULL;
	while(p != NULL)
	{
		pResult = p->GetComponentById(id);
		if(pResult != NULL) return pResult;
		p = p->m_Next;
	}
	return NULL;
}

LPYMXCOMPONENT YmxComponent::GetComponentByName(const char* name)
{
	if(m_Name != NULL && strcmp(m_Name, name) == 0)
		return this;

	LPYMXCOMPONENT p = m_Children;
	LPYMXCOMPONENT pResult = NULL;
	while(p != NULL)
	{
		pResult = p->GetComponentByName(name);
		if(pResult != NULL) return pResult;
		p = p->m_Next;
	}
	return NULL;
}


bool YmxComponent::_OnMessage(int msgid, DWORD notifyCode, PVOID param)
{
	switch(msgid)
	{
	case GCM_SETVISIBLE:
		m_bVisible = (bool)notifyCode;
		return true;
	case GCM_SETENABLED:
		m_bEnabled = (bool)notifyCode;
		return true;
	default:
		return OnMessage(msgid, notifyCode, param);
	}
	return false;
}


bool YmxComponent::OnMessage(int msgid, DWORD notifyCode, PVOID param)
{
	return false;
}

LPYMXCOMPONENT YmxComponent::GetParent()
{
	return m_Parent;
}

bool YmxComponent::SendMessage(int msgid, int targetId, DWORD notifyCode, PVOID param)
{
	LPYMXCOMPONENT pTarget = GetGameObject()->GetComponentById(targetId);
	if(pTarget == NULL) return false;
	return pTarget->_OnMessage(msgid, notifyCode, param);
} 

bool YmxComponent::SendMessage(int msgid,const char* compoName, DWORD notifyCode, PVOID param)
{
	LPYMXCOMPONENT pTarget = GetGameObject()->GetComponentByName(compoName);
	if(pTarget == NULL) return false;
	return pTarget->_OnMessage(msgid, notifyCode, param);
}

bool YmxComponent::SendMessage(int msgid, LPYMXCOMPONENT pTarget, DWORD notifyCode, PVOID param)
{
	if(pTarget == NULL)
		return false;
	return pTarget->_OnMessage(msgid, notifyCode, param);
}

bool YmxComponent::BroadcastMessage(int msgid, int componentId, DWORD notifyCode, PVOID param, DWORD flag)
{
	LPYMXCOMPONENT pComponent = GetGameObject()->GetComponentById(componentId);
	return _BroadcastMessage(msgid, pComponent, notifyCode, param, flag);
}

bool YmxComponent::BroadcastMessage(int msgid, const char* compoName, DWORD notifyCode, PVOID param, DWORD flag)
{
	LPYMXCOMPONENT pComponent = GetGameObject()->GetComponentByName(compoName);
	return _BroadcastMessage(msgid, pComponent, notifyCode, param, flag);
}

bool YmxComponent::BroadcastMessage(int msgid, LPYMXCOMPONENT pTarget, DWORD notifyCode, PVOID param, DWORD flag)
{
	return _BroadcastMessage(msgid, pTarget, notifyCode, param, flag);
}

bool YmxComponent::_BroadcastMessage(int msgid, LPYMXCOMPONENT pComponent, DWORD notifyCode, PVOID param, DWORD flag)
{
	if(pComponent == NULL) 
		return false;

	// if flag is GMF_BUBBLE, then from bottom component to top.
	if(flag & GMF_BUBBLE)
	{
		if(!(flag & GMF_THROWABLE))
		{
			if(pComponent->_OnMessage(msgid, notifyCode, param)) return true;
			LPYMXCOMPONENT p = pComponent->m_Parent;
			if(_BroadcastMessage(msgid, p, notifyCode, param, flag)) return true;
			return false;
		}
		else {
			bool result = pComponent->_OnMessage(msgid, notifyCode, param);
			if(flag & GMF_BUBBLE)
			{
				LPYMXCOMPONENT p = pComponent->m_Parent;
				result |= _BroadcastMessage(msgid, p, notifyCode, param, flag);
			}
			return result;
		}
	}

	// If flag is GMF_CAPTURE, then from top to bottom component, use BFS.
	Clear_ComponentQueue(&g_cqHead, &g_cqTail); 

	Add_To_ComponentQueue(&g_cqHead, &g_cqTail, pComponent);

	LPYMXCOMPONENT p;
	bool result = false;
	while(g_cqHead != NULL)
	{
		p = Pop_From_ComponentQueue(&g_cqHead, &g_cqTail);
		if(p->_OnMessage(msgid, notifyCode, param))
		{
			if(!(flag & GMF_THROWABLE)) return true;
			else result = true;
		}

		LPYMXCOMPONENT pChild = p->m_Children;
		while(pChild != NULL)
		{
			Add_To_ComponentQueue(&g_cqHead, &g_cqTail, pChild);
			pChild = pChild->m_Next;
		}
	}
	return result;
}

const char* YmxComponent::GetName()
{
	return m_Name;
}

LPYMXFONT YmxComponent::CreateFont(PCTSTR szFntFileName)
{
	return YmxFont::Load_From_File(szFntFileName, this);
}

LPPARTICLEYMXTEM YmxComponent::CreateParticleSystem(PARTICLESYSTEM_TYPE parType, LPYMXTEXTURE pTexture, int maxCount)
{
	return YmxParticleSystem::CreateParticleSystem(this, parType, pTexture, maxCount);
}

YmxButton* YmxComponent::CreateButton(UINT id, LPYMXTEXTURE pTexture, LPYMXFONT pFont, float x, float y)
{
	return new YmxButton(this, id, pTexture, pFont, x, y);
}

YmxListView* YmxComponent::CreateListView(UINT id, LPYMXTEXTURE pTexture, LPYMXFONT pFont, float x, float y)
{
	return new YmxListView(this, id, pTexture, pFont, x, y);
}

YmxEditText* YmxComponent::CreateEditText(UINT id, LPYMXFONT pFont, float x, float y)
{
	return new YmxEditText(this, id, pFont, x, y);
}

LPYMXSOUND YmxComponent::CreateSound(PTSTR filename)
{
	return GetGameObject()->CreateSound(filename);
}

/**
* Add pComponent to this.
* if bInit is true, Then the pComponent is initialized (and LoadContent) when this method called
* if bInit is false, Then the pComponent is initialized when the YmxGame->Start() is called.
*/
bool YmxComponent::AddComponent(LPYMXCOMPONENT pComponent, bool bInit /* = true */)
{
	if(pComponent->m_Parent != NULL)
	{
		pComponent->_RemoveFromParent();
	}

	pComponent->m_Parent = this;

	LPYMXCOMPONENT p1 = m_Children, p2 = NULL;

	while (p1 != NULL && p1->m_Order <= pComponent->m_Order)
	{
		p2 = p1;
		p1 = p1->m_Next;
	}

	if(p1 == m_Children)
	{
		pComponent->m_Next = p1;
		m_Children = pComponent;
	}
	else
	{
		p2->m_Next = pComponent;
		pComponent->m_Next = p1;
	}

	if(bInit)
	{
		pComponent->_LoadContent();
		pComponent->_Initialize();
	}

	return true;
}


/*
* Remove the pComponent from 'this' component
* if bRelease is true, then then pComponent is released when this method called.
* if bRelease is false, Then the pComponent is released when the YmxGame->Release() is called.
*/
bool YmxComponent::RemoveComponent(LPYMXCOMPONENT pComponent, bool bRelease /* = true */)
{
	if(pComponent != NULL)
	{
		pComponent->_RemoveFromParent();
		if(bRelease)
		{
			pComponent->_Release();
		}
		return true;
	}
	return false;
}

bool YmxComponent::RemoveComponent(const char* name, bool bRelease /* = true */)
{
	LPYMXCOMPONENT pComponent = GetComponentByName(name);
	return RemoveComponent(pComponent, bRelease);
}

bool YmxComponent::RemoveComponent(int id, bool bRelease /* = true */)
{
	LPYMXCOMPONENT pComponent = GetComponentById(id);
	return RemoveComponent(pComponent, bRelease);
}

int YmxComponent::GetOrder()
{
	return m_Order;
}


