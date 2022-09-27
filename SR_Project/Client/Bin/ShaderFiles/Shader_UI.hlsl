



//VS_MAIN(float3 vPosition : POSITION, float2 vTexUV : TEXCOORD0)
//{
//	
//}

//float, float2, float3, float4, vector
//
//float3		vTmp;
//
//vTmp.xyz;
//vTmp.rgb;

//
//float2		vTmep2 = vTmp.xy;

// float4x4, float3x3, float1x3, matrix

float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture			g_Texture;
bool			g_isColl;
float           g_alpha;
float           g_alpha1;

sampler TextureSampler = sampler_state {
	texture = g_Texture;
	MINFILTER = linear;
	MAGFILTER = linear;
	MIPFILTER = linear;
};



/* VertexShader_진입점 */
struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;
		
	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;	 
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;

	return Out;
}

/* w 나누기연산. */
/* 뷰포트 변환. */
/* 래스터라이즈  */

struct PS_IN
{
	float4 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};


/* 화면에 출력해야할 픽셀의 최종적인 색을 결정하낟.  */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = tex2D(TextureSampler, In.vTexUV);
	return Out;
}

PS_OUT PS_SCREENEFFECT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = tex2D(TextureSampler, In.vTexUV);
	Out.vColor.a -= g_alpha ;
	return Out;
}

PS_OUT PS_MiniMap(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = tex2D(TextureSampler, In.vTexUV);
	Out.vColor.a -= g_alpha;
	return Out;
}

PS_OUT PS_HP(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = tex2D(TextureSampler, In.vTexUV);
	//Out.vColor.a -= g_alpha1;


	Out.vColor.rgb += g_alpha1;
	return Out;
}


technique		DefaultTechnique
{
	pass DefaultPass_with_AlphaTest
	{	
		AlphaTestEnable = TRUE;
		AlphaFunc = greater;
		AlphaRef = 40;
		CULLMODE = NONE;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass ScreenEffect
	{
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		BlendOp = Add;
		CULLMODE = NONE;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_SCREENEFFECT();
	}

	pass MiniMap
	{
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		BlendOp = Add;
		CULLMODE = NONE;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MiniMap();
	}

	pass HP
	{
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		BlendOp = Add;
		CULLMODE = NONE;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_HP();
	}
}