////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
	float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float4 domePosition : TEXCOORD1;
	float3 normal : NORMAL;
};

static const float3 sunPos = float3(0.0, 0.707, 0.707); 

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{

	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;
	float4 horizon = float4(0.0f, 0.0f, 0.0f, 1.0f); //float4(1.0f, 0.6f, 0.0f, 1.0f)
	float height = input.domePosition.y;

	float sunProximity = dot(sunPos, -height);
	float sunYellow = smoothstep(0.5, 0.0, sunProximity);

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = saturate(diffuseColor * lightIntensity);

    // Multiply the texture pixel and the final diffuse color to get the final pixel color result
	//color = color * horizon;
	color = color * textureColor;
	
	//add this to colour:
	color.r += sunYellow;
	color.g += sunYellow;

    return color;
}
