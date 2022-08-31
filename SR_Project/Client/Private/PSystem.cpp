//#include "stdafx.h"
//#include "..\Public\PSystem.h"
//
//
//CPSystem::CPSystem(LPDIRECT3DDEVICE9 pGraphic_Device)
//	: CGameObject(pGraphic_Device)
//{
//}
//
//CPSystem::CPSystem(const CParticle & rhs)
//	: CGameObject(rhs)
//{
//}
//
//HRESULT CPSystem::Initialize_Prototype()
//{
//	
//	return S_OK;
//}
//
//HRESULT CPSystem::Initialize(void * pArg)
//{
//	if (nullptr != pArg)
//		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));
//
//	if (FAILED(SetUp_Components()))
//		return E_FAIL;
//
//	if (FAILED(Ready_ParticleSystem()))
//		return E_FAIL;
//
//	if (FAILED(Ready_VIBuffer()))
//		return E_FAIL;
//
//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_StateDesc.vPosition);
//	m_bStart = false;
//
//
//	m_pTransformCom->Set_Scale(m_StateDesc.vParticleScale.x, m_StateDesc.vParticleScale.y, 0);
//
//	return S_OK;
//}
//
//int CPSystem::Tick(_float fTimeDelta)
//{
//	return OBJ_NOEVENT;
//}
//
//void CPSystem::Late_Tick(_float fTimeDelta)
//{
//}
//
//HRESULT CPSystem::Render()
//{
//	return E_NOTIMPL;
//}
//
//HRESULT CPSystem::SetUp_RenderState()
//{
//	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
//	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//
//	return S_OK;
//}
//
//HRESULT CPSystem::Release_RenderState()
//{
//	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
//	return S_OK;
//}
//
//void CPSystem::reset()
//{
//	std::list<Attribute>::iterator i;
//	for (i = _particles.begin(); i != _particles.end(); i++)
//	{
//		resetParticle(&(*i));
//	}
//}
//
//void CPSystem::addParticle()
//{
//	//이 메서드는 리스트에 추가하기 전에 파티클을 초기화 하는데  resetParticle메서드를 이용한다.
//	Attribute attribute;
//	resetParticle(&attribute);
//
//	_particles.push_back(attribute);
//}
//
//bool CPSystem::isDead()
//{
//
//	std::list<Attribute>::iterator i;
//	for (i = _particles.begin(); i != _particles.end(); i++)
//	{
//		// is there at least one living particle?  If yes,
//		// the system is not dead.
//		if (i->_isAlive)
//			return false;
//	}
//	// no living particles found, the system must be dead.
//	return true;
//}
//
//void CPSystem::removeDeadParticles()
//{
//	std::list<Attribute>::iterator i;
//
//	i = _particles.begin();
//
//	while (i != _particles.end())
//	{
//		if (i->_isAlive == false)
//		{
//			//erase는 다음 반복자를 리턴하므로 우리가 반복자를 증가시킬 수 없다
//			i = _particles.erase(i);
//		}
//		else
//		{
//			i++; // next in list
//		}
//	}
//}
//
//
//CPSystem::~CPSystem()
//{
//}
