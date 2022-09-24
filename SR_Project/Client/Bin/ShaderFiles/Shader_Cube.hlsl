

float3			g_PlayerPosition;
float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture			g_Texture;

float			g_fDinnerMinRange = 2.f;
float			g_fDinnerMaxRange = 10.f;

float			g_fDinnerDelta = 0.f;
float			g_fNightDelta = 0.f;
float			g_fNightDarkAlpha = 0.f;

float			g_fMinRange = 3.f;
float			g_fMaxRange = 25.f;


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
	float3 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
	float3 vTexUV : TEXCOORD0;
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
	float3 vTexUV : TEXCOORD0;
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

	Out.vColor = texCUBE(TextureSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_HIT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = texCUBE(TextureSampler, In.vTexUV);
	Out.vColor.r += 0.2f;
	Out.vColor.gb -= 0.1f;

	return Out;
}

PS_OUT PS_PICKING(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = texCUBE(TextureSampler, In.vTexUV);
	Out.vColor.rgb += float3(0.2, 0.2, 0.2);
	return Out;
}

PS_OUT PS_DEAD(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = texCUBE(TextureSampler, In.vTexUV);
	Out.vColor.gb = Out.vColor.r;

	Out.vColor -= 0.2f;

	return Out;
}

PS_OUT PS_FIRE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = texCUBE(TextureSampler, In.vTexUV);

	/*Out.vColor.r += 0.1f;

	float4		vFogColor = vector(0.2f, 0.5f, 0.5f, 0.f);
	float		fDistance = length(g_PlayerPosition - In.vWorldPos);

	float		fFogPower = max(fDistance - g_fMinRange, 0.f) / (g_fMaxRange - g_fMinRange);

	Out.vColor -= vFogColor * fFogPower;*/

	return Out;
}

PS_OUT PS_DARK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = texCUBE(TextureSampler, In.vTexUV);

	float4		vFogColor = vector(1.f, 1.f, 1.f, 0.f);
	float		fDistance = length(g_PlayerPosition - In.vWorldPos);

	float		fFogPower = max(fDistance - g_fMinRange, 0.f) / (g_fMaxRange - g_fMinRange);

	Out.vColor -= vFogColor * fFogPower;

	return Out;
}


PS_OUT PS_DARKWITHLIGHT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = texCUBE(TextureSampler, In.vTexUV);

	Out.vColor.r += g_fDinnerDelta;
	Out.vColor.b += g_fNightDelta;

	float		fDistance = length(g_PlayerPosition - In.vWorldPos);

	float4		vLightColor = vector(2.f, 2.f, 1.f, 0.f);
	float		fLightPower = max(g_fMinRange - fDistance, 0.f) / (g_fMaxRange - g_fMinRange);


	Out.vColor += vLightColor * fLightPower;

	return Out;
}

PS_OUT PS_DAYCYCLE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = texCUBE(TextureSampler, In.vTexUV);

	Out.vColor.b -= g_fDinnerDelta;
	Out.vColor.g -= g_fDinnerDelta*0.5f;

	Out.vColor -= g_fNightDelta;

	///float4		vFogColor = vector(g_fNightDarkAlpha, g_fNightDarkAlpha, g_fNightDarkAlpha, 0.f);
	//float		fDistance = length(g_PlayerPosition - In.vWorldPos);

	//float		fFogPower = max(fDistance - g_fDinnerMinRange, 0.f) / (g_fDinnerMaxRange - g_fDinnerMinRange);

	//Out.vColor.rgb -= g_fNightDarkAlpha*0.2f;
	//Out.vColor -= vFogColor * fFogPower;

	return Out;
}



technique		DefaultTechnique
{
	pass DefaultPass_with_AlphaTest
	{
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = cw;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Hit
	{
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = cw;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_HIT();
	}

	pass Picking
	{
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = cw;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_PICKING();
	}

	pass Dead
	{
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = cw;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DEAD();
	}

	pass Fire
	{
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = cw;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_FIRE();
	}

	pass Dark
	{
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = cw;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DARK();
	}

	pass DarkwithLight
	{
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = cw;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DARKWITHLIGHT();
	}

	pass DayCycle
	{
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = cw;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DAYCYCLE();
	}
}

