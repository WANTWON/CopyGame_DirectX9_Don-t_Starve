//#pragma once
//#include "Particle.h"
//
//BEGIN(Client)
//
//class CPSystem : public CGameObject
//{
//public:
//	struct Particle
//	{
//		D3DXVECTOR3 _position; //파티클의 위치 
//		D3DCOLOR    _color; //파티클의 컬러
//		float		_size;
//		static const DWORD FVF;
//	};
//
//	//파티클의 속성을 담는 구조체
//	typedef struct Attribute
//	{
//		D3DXVECTOR3 _position; //월드 스페이스 내의 파티클 위치
//		D3DXVECTOR3 _velocity; //파티클의 속도
//		D3DXVECTOR3 _acceleration; //파티클의 가속
//		float _lifeTime; //파티클이 소멸할 때까지 유지되는 시간
//		float _age;  //파티클의 나이
//		bool _isAlive; //파티클의 생존 여부
//	}STATEDESC;
//
//private:
//	CPSystem(LPDIRECT3DDEVICE9 pGraphic_Device);
//	CPSystem(const CParticle& rhs);
//	virtual ~CPSystem() = default;
//
//public:
//	virtual HRESULT Initialize_Prototype();
//	virtual HRESULT Initialize(void* pArg);
//	virtual int Tick(_float fTimeDelta);
//	virtual void Late_Tick(_float fTimeDelta);
//	virtual HRESULT Render();
//	HRESULT SetUp_RenderState();
//	HRESULT Release_RenderState();
//
//	virtual void reset();
//	virtual void resetParticle(Attribute* attribute) = 0;
//	virtual void addParticle();
//
//	bool isEmpty() { return _particles.empty(); }
//	bool isDead();
//
//protected:
//	virtual void removeDeadParticles();
//
//protected:
//	IDirect3DDevice9* _device;
//	D3DXVECTOR3		  _origin; //시스템의 원천 시스템 내에서 파티클이 시작되는 곳
//	BoundingBox  _boundingBox; //경계 상자, 영역을 벗어나면 파티클들이 소멸된다
//	float _emitRate; //초당 파티클 수로
//	float _size;	 //파티클의 크기
//	CTexture*			m_pTextureCom = nullptr;
//	PDIRECT3DVERTEXBUFFER9		m_pVBuffer = nullptr;
//	_uint			m_iStride = 0;
//	_uint			m_iNumVertices = 0;
//	_uint			m_iNumPrimitive = 0;
//	_ulong			m_dwFVF = 0;
//
//	list<Attribute> _particles; //속성의 리스트
//	int				_maxParticles; //주어진 시간 동안 시스템이 가질 수 있는 최대 파티클의 수.
//
//	DWORD  _vbSize; // 버텍스 버퍼 보괄할 수 있는 파티클의 수
//	DWORD  _vbOffset;
//	DWORD  _vbBatchSize;
//};
//
//END