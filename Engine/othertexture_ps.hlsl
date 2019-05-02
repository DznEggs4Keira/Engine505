////////////////////////////////////////////////////////////////////////////////
// Filename: texture.ps
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float4 sepia;

	//Sepia thing
	float rOffset = 0.686;
	float gOffset = 0.541;
	float bOffset = 0.412;

	textureColor = shaderTexture.Sample(SampleType, input.tex);

	//sepia effect
	sepia.r = textureColor * rOffset;
	sepia.g = textureColor * gOffset;
	sepia.b = textureColor * bOffset;
	sepia.a = 1.0f;

	return sepia;
}