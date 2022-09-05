#include "..\Public\Collider_Rect.h"

CCollider_Rect::CCollider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CCollider_Rect::CCollider_Rect(const CCollider_Rect & rhs)
	: CComponent(rhs)
	, m_pLine(rhs.m_pLine)
	, m_ColliderMatrix(rhs.m_ColliderMatrix)
	, m_pVB(rhs.m_pVB)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_dwFVF(rhs.m_dwFVF)
	, m_iStride(rhs.m_iStride)
	, m_ePrimitiveType(rhs.m_ePrimitiveType)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)

{
	memcpy(m_vOriginalPoint, rhs.m_vOriginalPoint, sizeof(_float3) * 4);
	memcpy(m_vPoint, rhs.m_vOriginalPoint, sizeof(_float3) * 4);
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pLine);
}

HRESULT CCollider_Rect::Initialize_Prototype()
{
	D3DXMatrixIdentity(&m_StateDesc.StateMatrix);

	if (FAILED(D3DXCreateLine(m_pGraphic_Device, &m_pLine)))
		return E_FAIL;

	m_vOriginalPoint[0] = _float3(-0.5f, 0.5f, 0.f);
	m_vOriginalPoint[1] = _float3(0.5f, 0.5f, 0.f);
	m_vOriginalPoint[2] = _float3(0.5f, -0.5f, 0.f);
	m_vOriginalPoint[3] = _float3(-0.5f, -0.5f, 0.f);

	memcpy(m_vPoint, m_vOriginalPoint, sizeof(_float3) * 4);


	m_iNumVertices = 4;
	m_iStride = sizeof(VTXTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1;
	m_ePrimitiveType = D3DPT_TRIANGLELIST;
	m_iNumPrimitive = 2;

	/* 정점들을 할당했다. */
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iNumVertices * m_iStride, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, 0)))
		return E_FAIL;

	VTXTEX*			pVertices = nullptr;

	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexture = _float2(0.0f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexture = _float2(1.f, 0.f);
	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexture = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexture = _float2(0.f, 1.f);
	m_pVB->Unlock();

	return S_OK;
}

HRESULT CCollider_Rect::Initialize(void * pArg)
{
	if (pArg)
		memcpy(&m_StateDesc, pArg, sizeof(COLLRECTDESC));

	return S_OK;
}

HRESULT CCollider_Rect::Update_ColliderBox(_float4x4 WorldMatrix)
{

	//D3DXVec3Normalize((_float3*)&m_StateDesc.StateMatrix.m[0][0], (_float3*)&WorldMatrix.m[0][0]);
	//D3DXVec3Normalize((_float3*)&m_StateDesc.StateMatrix.m[1][0], (_float3*)&WorldMatrix.m[1][0]);
	//D3DXVec3Normalize((_float3*)&m_StateDesc.StateMatrix.m[2][0], (_float3*)&WorldMatrix.m[2][0]);

	//WorldMatrix.m[3][0] = WorldMatrix.m[3][0] + m_StateDesc.fOffSetX;
	//WorldMatrix.m[3][1] = WorldMatrix.m[3][1] + m_StateDesc.fOffSetY;

	//memcpy(&m_StateDesc.StateMatrix.m[3][0], &WorldMatrix.m[3][0], sizeof(_float3));

	//memcpy(&m_StateDesc.StateMatrix.m[0][0], &(*(_float3*)&m_StateDesc.StateMatrix.m[0][0] * m_StateDesc.fWidth), sizeof(_float3));
	//memcpy(&m_StateDesc.StateMatrix.m[1][0], &(*(_float3*)&m_StateDesc.StateMatrix.m[1][0] * m_StateDesc.fHeight), sizeof(_float3));


	//for (size_t i = 0; i < 4; ++i)
	//{
	//	// 1x4 * 4x4
	//	D3DXVec3TransformCoord(&m_vPoint[i], &m_vOriginalPoint[i], &m_StateDesc.StateMatrix);
	//}

	_float3 vPosition = *(_float3*)(&WorldMatrix.m[3][0]);
	VTXTEX*			pVertices = nullptr;

	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&pVertices, 0);

  	pVertices[0].vPosition = vPosition + m_vOriginalPoint[0];
	pVertices[0].vTexture = _float2(0.0f, 0.f);

	pVertices[1].vPosition = vPosition + m_vOriginalPoint[1];
	pVertices[1].vTexture = _float2(1.f, 0.f);

	pVertices[2].vPosition = vPosition + m_vOriginalPoint[2];
	pVertices[2].vTexture = _float2(1.f, 1.f);

	pVertices[3].vPosition = vPosition + m_vOriginalPoint[3];
	pVertices[3].vTexture = _float2(0.f, 1.f);
	m_pVB->Unlock();


	return S_OK;
}

HRESULT CCollider_Rect::Render_ColliderBox()
{
	if (nullptr == m_pLine)
		return E_FAIL;

	//_float2		vPoint[5] = {
	//	{ m_vPoint[0].x, m_vPoint[0].y },
	//	{ m_vPoint[1].x, m_vPoint[1].y },
	//	{ m_vPoint[2].x, m_vPoint[2].y },
	//	{ m_vPoint[3].x, m_vPoint[3].y },
	//	{ m_vPoint[0].x, m_vPoint[0].y },
	//};

	//_float4x4 InverseViewMatrix;
	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &InverseViewMatrix);
	//D3DXMatrixInverse(&InverseViewMatrix, nullptr, &InverseViewMatrix);

	//for (size_t i = 0; i < 5; ++i)
	//{
	//	D3DXVec2TransformCoord(&vPoint[i], &vPoint[i], &InverseViewMatrix);
	//}


	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pGraphic_Device->SetFVF(m_dwFVF);

	m_pGraphic_Device->DrawPrimitive(m_ePrimitiveType, 0, m_iNumPrimitive);

	/*m_pLine->SetWidth(2.f);
	m_pLine->Begin();
	m_pLine->Draw(vPoint, 5, D3DXCOLOR(1.f, 0.f, 0.f, 1.f));
	m_pLine->End();*/

	return S_OK;
}

CCollider_Rect * CCollider_Rect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider_Rect*	pInstance = new CCollider_Rect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCollider_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider_Rect::Clone(void * pArg)
{
	CCollider_Rect*	pInstance = new CCollider_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCollider_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Rect::Free()
{
	__super::Free();

	Safe_Release(m_pLine);
	Safe_Release(m_pVB);
}
