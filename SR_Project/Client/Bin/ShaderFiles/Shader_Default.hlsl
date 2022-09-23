



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

float3			g_PlayerPosition;
float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture			g_Texture;
bool			g_isColl;

float			g_fMinRange = 3.f;
float			g_fMaxRange = 15.f;


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

PS_OUT PS_HIT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = tex2D(TextureSampler, In.vTexUV);
	Out.vColor.r += 0.2f;
	Out.vColor.gb -= 0.1f;
	
	return Out;
}

PS_OUT PS_PICKING(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = tex2D(TextureSampler, In.vTexUV);
	Out.vColor.rgb += float3(0.2, 0.2, 0.2);
	return Out;
}

PS_OUT PS_DEAD(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = tex2D(TextureSampler, In.vTexUV);
	Out.vColor.gb = Out.vColor.r;

	float4		vFogColor = vector(1.f, 1.f, 1.f, 0.f);
	float		fDistance = length(g_PlayerPosition - In.vWorldPos);

	float		fFogPower = max(fDistance - g_fMinRange, 0.f) / (g_fMaxRange - g_fMinRange);

	Out.vColor -= vFogColor * fFogPower;

	return Out;
}

PS_OUT PS_LIGHT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = tex2D(TextureSampler, In.vTexUV);

	float4		vFogColor = vector(2.f, 2.f, 1.f, 0.f);
	float		fDistance = length(g_PlayerPosition - In.vWorldPos);

	float		fFogPower = max(g_fMinRange - fDistance, 0.f) / (g_fMaxRange - g_fMinRange);


	Out.vColor += vFogColor * fFogPower;

	return Out;
}

PS_OUT PS_DARK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = tex2D(TextureSampler, In.vTexUV);

	float4		vFogColor = vector(1.f, 1.f, 1.f, 0.f);
	float		fDistance = length(g_PlayerPosition - In.vWorldPos);

	float		fFogPower = max(fDistance - g_fMinRange, 0.f) / (g_fMaxRange - g_fMinRange);

	Out.vColor -= vFogColor * fFogPower;

	return Out;
}


PS_OUT PS_DARKWITHLIGHT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = tex2D(TextureSampler, In.vTexUV);

	float4		vFogColor = vector(1.f, 1.f, 1.f, 0.f);
	float		fDistance = length(g_PlayerPosition - In.vWorldPos);

	float		fFogPower = max(fDistance - g_fMinRange, 0.f) / (g_fMaxRange - g_fMinRange);

	Out.vColor -= vFogColor * fFogPower;



	float4		vLightColor = vector(2.f, 2.f, 1.f, 0.f);
	float		fLightPower = max(g_fMinRange - fDistance, 0.f) / (g_fMaxRange - g_fMinRange);


	Out.vColor += vLightColor * fLightPower;

	return Out;
}



technique		DefaultTechnique
{
	pass DefaultPass_with_AlphaTest
	{	
		AlphaTestEnable = TRUE;
		AlphaFunc = greater;
		AlphaRef = 50;
		CULLMODE = NONE;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass DefaultPass_with_AlphaBlend
	{
		ALPHABLENDENABLE = true;
		SRCBLEND = SRCALPHA;
		DESTBLEND = INVSRCALPHA;
	//	BlendOp = Add;
		CULLMODE = NONE;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Hit
	{
		AlphaTestEnable = TRUE;
		AlphaFunc = greater;
		AlphaRef = 50;
		CULLMODE = NONE;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_HIT();
	}

	pass Picking
	{
		AlphaTestEnable = TRUE;
		AlphaFunc = greater;
		AlphaRef = 50;
		CULLMODE = NONE;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_PICKING();
	}

	pass Dead
	{
		AlphaTestEnable = TRUE;
		AlphaFunc = greater;
		AlphaRef = 50;
		CULLMODE = NONE;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DEAD();
	}

	pass Light
	{
		AlphaTestEnable = TRUE;
		AlphaFunc = greater;
		AlphaRef = 50;
		CULLMODE = NONE;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_LIGHT();
	}

	pass Dark
	{
		AlphaTestEnable = TRUE;
		AlphaFunc = greater;
		AlphaRef = 50;
		CULLMODE = NONE;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DARK();
	}

	pass DarkwithLight
	{
		AlphaTestEnable = TRUE;
		AlphaFunc = greater;
		AlphaRef = 50;
		CULLMODE = NONE;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DARKWITHLIGHT();
	}
}

