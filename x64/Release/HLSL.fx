//--------------------------------------------------------------------------------------
// v. 3
// File: BasicHLSL.fx
//
// The effect file for the BasicHLSL sample.  
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#define SHADOW_EPSILON 0.024900f//0.0285f more dark shadows <-----o------> less shadows


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

bool     LightRander;

bool     AddBumpTexture=false;
bool     Drawshadows;


float3   ObsPositionV;				//Position of observer
float3   LightPosition;						//Position of sun
float3	 ObsPosition;				//eye position

float4   g_ObiectColor;             // Light's ambient color
float4   g_LightAmbient;             // Light's ambient color
float4   g_LightObserver;            // Light's observer color

texture  g_BumpTexture;
texture  g_txShadow;



float4x4 g_mWorld;					 // World matrix for object
float4x4 g_mRotate;                  // rotation camera matrix for object
float4x4 g_mWorldViewProjection;	 // World * View * Projection matrix
float4x4 g_mWorldLightProjection;	 // Transform from view space to light projection space

float4 dummy_collor;// tex2D(MeshTextureSampler, In.TextureUV);

//specular section
float Shininess1 = 25;
float Shininess2 = 20;
float4 SunColor; 
float4 SpecularColor;  
 
float SpecularIntensity = .5;


// Contains sampling offsets used by the techniques Gauss blur
static const int    MAX_SAMPLES            = 16;    // Maximum texture grabs
float2 g_avSampleOffsets[MAX_SAMPLES];
float4 g_avSampleWeights[MAX_SAMPLES];
//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
sampler s0 : register(s0);


//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
texture colorMapTexture;

sampler2D colorMap = sampler_state
{
	Texture = <colorMapTexture>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
	AddressU = Clamp;
    AddressV = Clamp;
};

 
float BumpConstant = 8;
texture NormalMap;
sampler2D bumpSampler = sampler_state {
	Texture = <g_BumpTexture>;
    MinFilter = Linear; //Point
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
	//AddressU = Clamp;
    //AddressV = Clamp;
};


sampler2D g_samShadow =
sampler_state
{
    Texture = <g_txShadow>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
};

////////////////////////////////////////////////////
struct VS_INPUT
{
	float4 vPos       : POSITION; 
    float3 Normal     : NORMAL;
    float2 vTexCoord0 : TEXCOORD0;
    float3 Tangent    : TANGENT;
    float3 Binormal   : BINORMAL;
};
////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position			: POSITION0;	// vertex position 
   	float3 Normal			: TEXCOORD0;	//added
	float2 TextureUV		: TEXCOORD1;	// vertex texture coords 
 	float3 Tangent			: TEXCOORD2;
    float3 Binormal			: TEXCOORD3;
    float3 viewDirection	: TEXCOORD4;
	float4 lightDir1		: TEXCOORD5;
	float4 lightDir2		: TEXCOORD6;
	float4 lpos				: TEXCOORD7;    //vertex with regard to light view

};


//--------------------------------------------------------------------------------------
// This shader computes standard transform and lighting
//--------------------------------------------------------------------------------------
VS_OUTPUT RenderSceneVS( VS_INPUT input,
                         uniform int nNumLights,
                         uniform bool bTexture,
                         uniform bool bAnimate )
{
    VS_OUTPUT Output;

	float4 worldPosition;
	float4 sunPosition;
	float4 ObserverPosition;
	

	float4x4 g_mWorldRotate = mul(g_mWorld, g_mRotate);

    // Transform the position from object space to homogeneous projection space on the screen
    Output.Position = mul(input.vPos,  g_mWorldViewProjection );

    // Transform the normal from object space to world space with camera rotation    

    Output.Normal   =  normalize( mul(input.Normal,   (float3x3) g_mWorldRotate) );

	Output.Tangent  =  normalize( mul(input.Tangent,  (float3x3) g_mWorldRotate) ); 

    Output.Binormal =  normalize( mul(input.Binormal, (float3x3) g_mWorldRotate) );


	sunPosition =		mul(LightPosition        , g_mRotate); //sun position
	ObserverPosition =	mul(ObsPositionV, g_mRotate); //for small light at the observer position

	worldPosition =		mul(input.vPos, g_mWorldRotate ); //objeect position
	Output.viewDirection =  normalize( worldPosition - ObsPosition );

	
	Output.lightDir1     =  sunPosition   -     worldPosition;  
	Output.lightDir2     =  ObserverPosition  - worldPosition; 

	float distance1 = length(Output.lightDir1);//calculate the distane from light source	
	float distance2 = length(Output.lightDir2);//calculate the distane from light source	

	Output.lightDir1 = normalize(Output.lightDir1);
	Output.lightDir2 = normalize(Output.lightDir2);

	Output.lightDir1.w=distance1;
	Output.lightDir2.w=distance2;

	//light position in monitor cor.
	Output.lpos = mul( input.vPos, mul(g_mWorld, g_mWorldLightProjection ) );//from sun projection

	// Just copy the texture coordinate through
    Output.TextureUV = input.vTexCoord0; 

    
    return Output;    
}


//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};



//--------------------------------------------------------------------------------------
// This shader outputs the pixel's color by modulating the texture's
//       color with diffuse material color
//--------------------------------------------------------------------------------------
float4 RenderScenePS( VS_OUTPUT In  ) : COLOR0
{ 

float lightIntensity1;
float LightAmount=1. ;


 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
if (Drawshadows){
    //transform from RT space to texture space.
    float2 ShadowTexC = 0.5 * In.lpos.xy / In.lpos.w + float2( 0.5, 0.5 );
    ShadowTexC.y = 1.0f - ShadowTexC.y;


    float shadow= (tex2D( g_samShadow, ShadowTexC ) + SHADOW_EPSILON)-In.lpos.z /In.lpos.w;
    LightAmount = (shadow < 0. )? lerp( 0.5, .2, -shadow) : 1.0f;  //
}
else 
    LightAmount = 1.;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	float3 bumpNormal = normalize(In.Normal);

    // Calculate the diffuse light component with the bump map normal
    lightIntensity1 = dot( bumpNormal, (float3)In.lightDir1);
    if(lightIntensity1 < 0.)
        lightIntensity1 = 0.;


 	// Calculate the specular light component with the bump map normal
    float3 r = normalize(2. * bumpNormal * lightIntensity1 - (float3)In.lightDir1);
    float dotProduct = dot(r, In.viewDirection);


	//calculate specular light
	float4 specular1 = SpecularIntensity * SpecularColor * max(pow(dotProduct, Shininess1), 0) * lightIntensity1 * LightAmount * LightAmount* LightAmount * LightAmount;

   

	

  // Combine all of these values into one
	float4 color = g_ObiectColor * (SunColor * lightIntensity1* LightAmount   ) //
						 + g_ObiectColor *g_LightAmbient
						 + specular1*LightAmount
						 ;
//color.a =1.0;// 1.0;

return  color;


}


//-----------------------------------------------------------------------------
// Vertex Shader: VertShadow
// Desc: Process vertex for the shadow map
//-----------------------------------------------------------------------------
void VertShadow( float4 Pos : POSITION,
                 float3 Normal : NORMAL,
                 out float4 oPos : POSITION,
                 out float2 Depth : TEXCOORD0 )
{
    //
    // Set the size based on the normal size
	//float4 vAnimatedPos = Pos + float4(Normal, 0.) * g_Size;
    
     // Transform the position from object space to homogeneous projection space on the screen
    oPos = mul( mul( Pos, g_mWorld), g_mWorldLightProjection);//    	
    //
    // Store z and w in our spare texcoord
    //
    Depth.xy = oPos.zw;
}
//-----------------------------------------------------------------------------
// Pixel Shader: PixShadow
// Desc: Process pixel for the shadow map
//-----------------------------------------------------------------------------
void PixShadow( float2 Depth : TEXCOORD0,
                out float4 Color : COLOR )
{
    //
    // Depth is z / w
    //
	    Color = (Depth.x / Depth.y);

}

//-----------------------------------------------------------------------------
// Name: GaussBlur5x5PS
// Type: Pixel shader                                      
// Desc: Simulate a 5x5 kernel gaussian blur by sampling the 12 points closest
//       to the center point.
//-----------------------------------------------------------------------------
float4 GaussBlur5x5PS
    (
    in float2 vScreenPosition : TEXCOORD0
    ) : COLOR
{
	
    float4 sample = 0.0f;

	for( int i=0; i < 12; i++ )
	{
		sample += g_avSampleWeights[i] * tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );
	}

	return sample;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                        Techniques
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Name: GaussBlur5x5
// Type: Technique                                     
// Desc: Simulate a 5x5 kernel gaussian blur by sampling the 12 points closest
//       to the center point.
//-----------------------------------------------------------------------------
technique GaussBlur5x5
{
    pass P0
    {
        PixelShader  = compile ps_2_0 GaussBlur5x5PS();
    }
}


//-----------------------------------------------------------------------------
// Technique: RenderSky
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique Blend
{
    pass p0
    {

		AlphaBlendEnable = TRUE;
        DestBlend = INVSRCALPHA;
        SrcBlend = SRCALPHA;
        VertexShader = compile vs_2_0 RenderSceneVS( 1, true, true );
        PixelShader = compile ps_2_0 RenderScenePS();
		CullMode = None;//rander both sides
	}

}

//-----------------------------------------------------------------------------
// Technique: RenderSky
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique NoBlend
{
    pass p0
    {

		AlphaBlendEnable = FALSE;
        VertexShader = compile vs_2_0 RenderSceneVS( 1, true, true );
        PixelShader = compile ps_2_0 RenderScenePS();
		CullMode = None;//rander both sides
	}

}

//-----------------------------------------------------------------------------
// Technique: RenderShadow
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique RenderShadow
{
    pass p0
    {
        VertexShader = compile vs_2_0 VertShadow();
        PixelShader = compile ps_2_0 PixShadow();
		AlphaBlendEnable = false;
    }
}
