////////////////////////////////////////////////////////////////////////////////
// Filename: texture.ps
////////////////////////////////////////////////////////////////////////////////

// Screen Space Cartoon Effect
//
// Cartoon EFfect
// Pat 'Hawthorne' Shearon
// pat.shearon@gmail.com www.theregulators.org
//
// This version is provided under a Creative Commons Attribution 3.0 License
// http://creativecommons.org/licenses/by/3.0/ 


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

static const float4 ColorBlend = float4(4, 4, 4, 0.11f);

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	half4 cartoon;

	//Cartoon Effect
	cartoon = shaderTexture.Sample(SampleType, input.tex);
	cartoon.r = round(cartoon.r*ColorBlend.r) / ColorBlend.r;
	cartoon.g = round(cartoon.g*ColorBlend.g) / ColorBlend.g;
	cartoon.b = round(cartoon.b*ColorBlend.b) / ColorBlend.b;

	const float threshold = ColorBlend.w;

    const int NUM = 9;
	const float2 c[NUM] =
	{
		float2(-0.0078125, 0.0078125), //1
		float2(0.00 ,     0.0078125), //2
		float2(0.0078125, 0.0078125), //3
		float2(-0.0078125, 0.00), //4
		float2(0.0,       0.0), //5
		float2(0.0078125, 0.007), //6
		float2(-0.0078125,-0.0078125), //7
		float2(0.00 ,    -0.0078125), //8
		float2(0.0078125,-0.0078125), //9
    };

	int i;
	float3 col[NUM];
	for (i = 0; i < NUM; i++)
	{
		col[i] = shaderTexture.Sample(SampleType, input.tex.xy + 0.2*c[i]);
	}

	float3 rgb2lum = float3(0.30, 0.59, 0.11);
	float lum[NUM];
	for (i = 0; i < NUM; i++)
	{
		lum[i] = dot(col[i].xyz, rgb2lum);
	}
    float x = lum[2] + lum[8] + 2 * lum[5] - lum[0] - 2 * lum[3] - lum[6];
	float y = lum[6] + 2 * lum[7] + lum[8] - lum[0] - 2 * lum[1] - lum[2];
	float edge = (x*x + y * y < threshold) ? 1.0 : 0.0;

	cartoon.rgb *= edge;

	return cartoon;
}