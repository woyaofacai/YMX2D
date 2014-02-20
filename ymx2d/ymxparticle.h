#include "ymx2d.h"

#ifndef YMX2D_FUNCTION_CLASS_PARTICLE_H
#define YMX2D_FUNCTION_CLASS_PARTICLE_H

#define MAX_PSIZE_NUM 20

struct YmxParticleGroup
{
	YmxParticle* particles;
	YmxParticle* tail;
	float size;
	YmxParticleGroup(){particles = NULL; tail = NULL; size = 0.0f;}
};


enum YMX2D_API PARTICLE_ATTRIBUTE
{
	PAR_MAX_SIZE = 0,
	PAR_MIN_SIZE,
	PAR_SIZE,
	PAR_MAX_COUNT,
	PAR_MIN_VELOCITY_X,
	PAR_MAX_VELOCITY_X,
	PAR_MIN_VELOCITY_Y,
	PAR_MAX_VELOCITY_Y,
	PAR_VELOCITY_X,
	PAR_VELOCITY_Y,
	PAR_VELOCITY_VARIANT,
	PAR_VELOCITY,
	PAR_Z,
	PAR_COLOR_MIN_A,
	PAR_COLOR_MAX_A,
	PAR_COLOR_A,
	PAR_COLOR_MIN_R,
	PAR_COLOR_MAX_R,
	PAR_COLOR_R,
	PAR_COLOR_MIN_G,
	PAR_COLOR_MAX_G,
	PAR_COLOR_G,
	PAR_COLOR_MIN_B,
	PAR_COLOR_MAX_B,
	PAR_COLOR_B,
	PAR_START_COLOR_A,
	PAR_START_COLOR_R,
	PAR_START_COLOR_G,
	PAR_START_COLOR_B,
	PAR_END_COLOR_A,
	PAR_END_COLOR_R,
	PAR_END_COLOR_G,
	PAR_END_COLOR_B,
	PAR_START_SIZE,
	PAR_END_SIZE,
	PAR_POS_X,
	PAR_POS_Y,
	PAR_BOUNDING_LEFT,
	PAR_BOUNDING_TOP,
	PAR_BOUNDING_RIGHT,
	PAR_BOUNDING_BOTTOM,
	//use for fire
	PAR_FLAME_HEIGHT,
	PAR_FLAME_START_X,
	PAR_FLAME_START_Y,
	PAR_FLAME_END_X,
	PAR_FLAME_END_Y,
	PAR_VELOCITY_Y_VARIANT,
	PAR_VELOCITY_X_VARIANT,
	PAR_ACCELERATION_X,
	PAR_ACCELERATION_Y,
	PAR_ACCELERATION,
	PAR_ACCELERATION_VARIANT,
	PAR_ACCELERATION_X_VARIANT,
	PAR_ACCELERATION_Y_VARIANT,
	//use for explode
	PAR_LIFE,
	PAR_LIFE_VARIANT,
	PAR_SIZE_VARIANT,
	PAR_FIELD_ACCELERATION_X,
	PAR_FIELD_ACCELERATION_Y,
	PAR_EXPLODE_START_ANGLE,
	PAR_EXPLODE_END_ANGLE,
	PAR_COMET_DENSITY,

	//use for style
	PAR_FROM_NOTING,
	PAR_HIGHLIGHT,
	PAR_COLOR_GRADIENT,
	PAR_SIZE_GRADIENT,
	PAR_EXPLODE_CONTINUOUS,
	PAR_COMIT_BURN_ONLY_ONMOVING
};

#define _PS_FROM_NOTING 0x01 // ps stand for particle style
#define _PS_HIGHLIGHT 0x02
#define _PS_COLOR_GRADIENT 0x04
#define _PS_SIZE_GRADIENT 0x08
#define _PS_EXPLODE_CONTINUOUS 0x00010000
#define _PS_COMIT_BURN_ONLY_ONMOVING 0x00020000


class YMX2D_API YmxParticleSystem
{
public:
	virtual bool Initialize() = 0;
	virtual void Update(float delta) = 0;
	virtual void Render(YmxGraphics* g) = 0;
	virtual void Release() = 0;
	virtual void SetAttribute(PARTICLE_ATTRIBUTE attr, float value);
	virtual void SetAttribute(PARTICLE_ATTRIBUTE attr, int value);
	virtual void SetAttribute(PARTICLE_ATTRIBUTE attr, bool value);
	virtual bool GetAttribute(PARTICLE_ATTRIBUTE attr, int* value);
	virtual bool GetAttribute(PARTICLE_ATTRIBUTE attr, float* value);
	virtual bool GetAttribute(PARTICLE_ATTRIBUTE attr, bool* value);
	virtual void Stop();
	virtual void Restart();
	inline virtual void SetPosition(float x, float y);
	inline void SetTexture(LPYMXTEXTURE pTexture);
	inline LPYMXTEXTURE GetTexture();
	inline void SetZ(float z);
	inline float GetZ();
	inline PARTICLESYSTEM_TYPE GetType();
	virtual ~YmxParticleSystem();
	static LPPARTICLEYMXTEM CreateParticleSystem(LPYMXCOMPONENT pComponent, PARTICLESYSTEM_TYPE parType, LPYMXTEXTURE pTexture, int maxCount);
protected:
	YmxParticleSystem(LPYMXCOMPONENT pComponent, LPYMXTEXTURE pTexture, int maxCount);
	inline void _ReleaseParticleList(YmxParticle* particles);
	
	YMXRECT m_BoundingBox;
	PARTICLESYSTEM_TYPE m_Type;
	int m_MaxCount;
	int m_Count;
	LPYMXTEXTURE m_pTexture;
	LPYMXCOMPONENT m_pComponent;
	float m_z;
	DWORD m_Style;
	bool m_bStopped;
	bool m_bSupportPSize;
};

class YMX2D_API YmxSnowParticleSystem : public YmxParticleSystem
{
	friend class YmxParticleSystem;
public:
	virtual bool Initialize();
	virtual void Update(float delta);
	virtual void Render(YmxGraphics* g);
	virtual void Release();
	virtual void SetAttribute(PARTICLE_ATTRIBUTE attr, float value);
	virtual void SetAttribute(PARTICLE_ATTRIBUTE attr, int value);
	virtual bool GetAttribute(PARTICLE_ATTRIBUTE attr, int* value);
	virtual bool GetAttribute(PARTICLE_ATTRIBUTE attr, float* value);
private: 
	YmxSnowParticleSystem(LPYMXCOMPONENT pComponent, LPYMXTEXTURE pTexture, int maxCount);
	void _ResetParticle(YmxParticle* p, bool bOnBordary);
	void _ResetParticle(Vertex *v, bool bOnBordary);
	void _InitAppearRatesOnBordary();
	void _InitPeriodAppearNum();
	void _SetAppearPositionAndVelocity(int index);
	void _AddParticle(bool bOnBordary);
	void _UpdateParticleList(YmxParticle** pList, float delta);
	inline YMXCOLOR _GetRandomColor();
	YmxParticle* m_Particles;
	float m_AppearRatesOnBordary[4];	// left ,right, bottom, up
	int m_PeriodAppearNum;				// appear particle per 0.1s
	float m_elapseTimeSinceAppearPeriod;
	float m_MaxSize;
	float m_MinSize;
	YMXVECTOR m_MaxVelocity; 
	YMXVECTOR m_MinVelocity;
	int m_MinColorR, m_MaxColorR;
	int m_MinColorG, m_MaxColorG;
	int m_MinColorB, m_MaxColorB;
	int m_MinColorA, m_MaxColorA;
	//These members only use when point Size not support	
	int m_psizeNum;
	YmxParticleGroup* m_ParsGroups;
};

class YMX2D_API YmxFlameParticleSystem : public YmxParticleSystem
{
	friend class YmxParticleSystem;
public:
	virtual ~YmxFlameParticleSystem();
	virtual bool Initialize();
	virtual void Update(float delta);
	virtual void Render(YmxGraphics* g);
	virtual void Release();
	virtual void SetAttribute(PARTICLE_ATTRIBUTE attr, float value);
	virtual void SetAttribute(PARTICLE_ATTRIBUTE attr, int value);
	virtual bool GetAttribute(PARTICLE_ATTRIBUTE attr, int* value);
	virtual bool GetAttribute(PARTICLE_ATTRIBUTE attr, float* value);
private:
	YmxFlameParticleSystem(LPYMXCOMPONENT pComponent, LPYMXTEXTURE pTexture, int maxCount);
	void _InitAllParticles();
	YmxParticle* m_Particles;
	void _ResetParticle(YmxParticle* p);
	void _InitHeatSourcePoints();
	YMXRECT m_HeatsourceRect;
	int m_RGBVariant;
	float m_FrameHeight; 
	YMXVECTOR m_Velocity; // The average velocity of one particle
	YMXVECTOR m_VelocityVariant;
	float m_ParLife; 
	float m_ParLifeVariant;
	float m_PSizeVelocity;
	int m_AppearNumPerDelta;

	YMXVECTOR m_Acceleration;
	YMXVECTOR m_AccelerationVariant;
	YMXPOINT* m_HeatSourcePoints;
	YMXPOINT m_StartPoint;
	YMXPOINT m_EndPoint;
	int m_HeatSourcesNum;

	float m_StartSize;
	float m_EndSize;

	//only used for those not support POINTSZIE
	YmxParticleGroup* m_ParGroups;
	int m_ParGroupNum;
	float m_UpdatePSizeDelta;
	float m_elapseTimeSinceUpdatePSize;

	int m_StartColorA, m_StartColorR, m_StartColorG, m_StartColorB;
	int m_EndColorA, m_EndColorR, m_EndColorG, m_EndColorB;
	int m_ColorIncA, m_ColorIncR, m_ColorIncG, m_ColorIncB;
	YMXCOLOR m_StartColor;
	YMXCOLOR m_EndColor;
};

class YMX2D_API YmxExplodeParticleSystem : public YmxParticleSystem
{
	friend class YmxParticleSystem;
public:
	virtual ~YmxExplodeParticleSystem();
	virtual bool Initialize();
	virtual void Update(float delta);
	virtual void Render(YmxGraphics* g);
	virtual void Release();
	virtual void SetAttribute(PARTICLE_ATTRIBUTE attr, float value);
	virtual void SetAttribute(PARTICLE_ATTRIBUTE attr, int value);
	virtual bool GetAttribute(PARTICLE_ATTRIBUTE attr, int* value);
	virtual bool GetAttribute(PARTICLE_ATTRIBUTE attr, float* value);
	virtual void SetPosition(float x, float y);
private:
	YmxExplodeParticleSystem(LPYMXCOMPONENT pComponent, LPYMXTEXTURE pTexture, int maxCount);
	inline void _ResetParticle(YmxParticle* p);
	inline void _ResetParticle(int index);
	inline void _Update_SupportPSize(float delta);
	inline void _Update_Not_SupportPSize(float delta);
	YMXPOINT m_Center;
	float m_ParLife;
	float m_ParLifeVariant;
	float m_Velocity;
	float m_VelocityVariant;
	ParticleVertex* m_ParVertexs;
	YmxParticleAttribute* m_ParAttributes;
	YmxParticle* m_Particles;
	float m_StartSize;
	float m_EndSize;
	float m_SizeVariant;
	float m_SizeIncrement;
	int m_AppearNumPerDelta;
	float m_Delta;
	float m_Acceleration;           // The Particle Acceleration along the moving direction
	YMXVECTOR m_FieldAcceleration;  // The Acceleration given by external force field  
	float m_StartAngle;
	float m_EndAngle; 
	int m_RGBVariant;
	int m_StartColorA, m_StartColorR, m_StartColorG, m_StartColorB;
	int m_EndColorA, m_EndColorR, m_EndColorG, m_EndColorB;
	int m_ColorIncA, m_ColorIncR, m_ColorIncG, m_ColorIncB;
	YMXCOLOR m_StartColor;
	YMXCOLOR m_EndColor;

	// use for those don't support POINT SIZE
	YmxParticleGroup* m_ParGroups;
	int m_ParGroupNum;
	float m_UpdatePSizeDelta;
	float m_elapseTimeSinceUpdatePSize;
	int m_StartSizeRange;
};

class YMX2D_API YmxCometParticleSystem : public YmxParticleSystem
{
	friend class YmxParticleSystem;
public:
	virtual ~YmxCometParticleSystem();
	virtual void SetPosition(float x, float y);
	virtual bool Initialize();
	virtual void Update(float delta);
	virtual void Render(YmxGraphics* g);
	virtual void Release();
	virtual void SetAttribute(PARTICLE_ATTRIBUTE attr, float value);
	virtual void SetAttribute(PARTICLE_ATTRIBUTE attr, int value);
	virtual bool GetAttribute(PARTICLE_ATTRIBUTE attr, int* value);
	virtual bool GetAttribute(PARTICLE_ATTRIBUTE attr, float* value);
private:
	YmxCometParticleSystem(LPYMXCOMPONENT pComponent, LPYMXTEXTURE pTexture, int maxCount); 
	inline void _Update_SupportPSize(float delta);
	inline void _Update_Not_SupportPSize(float delta);
	inline void _ResetParticle(YmxParticle* p);
	inline YmxParticle* _GetFreeParticle();
	YmxParticle* m_Particles;
	YmxParticle* m_FreeParticles;
	YMXPOINT m_Position;
	float m_ParLife;
	float m_ParLifeVariant;
	float m_StartSize;
	float m_EndSize;
	float m_SizeVariant;
	int m_Density;
	float m_Velocity;
	float m_VelocityVariant;
	float m_Acceleration;
	float m_AccelerationVariant;
	int m_StartColorA, m_StartColorR, m_StartColorG, m_StartColorB;
	int m_EndColorA, m_EndColorR, m_EndColorG, m_EndColorB;
	int m_ColorIncA, m_ColorIncR, m_ColorIncG, m_ColorIncB;
	YMXCOLOR m_StartColor;
	YMXCOLOR m_EndColor;
	int m_RGBVariant;
	float m_Delta;
	float m_SizeIncrement;

	//only use for those don't support POINT SIZE
	YmxParticleGroup* m_ParGroups;
	int m_ParGroupNum;
	float m_UpdatePSizeDelta;
	float m_elapseTimeSinceUpdatePSize;
	int m_StartSizeRange;
};


#endif