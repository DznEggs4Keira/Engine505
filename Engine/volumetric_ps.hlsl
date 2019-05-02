Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer : register(b0)
{
    float4 diffuseColor;
    float3 lightDirection;
	float padding;
};

cbuffer CamNormBuffer : register(b1)
{
	float3 cameraPosition;
	float timeElapsed;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 wPos : TEXCOORD1;
	float3 dirCalc : TEXCOORD2;
	float3 normal : NORMAL;
};

static const float STEPS = 64.0f;
static const float STEP_SIZE = sqrt(5.0f) / STEPS;	//go through whole box even in worst case //3.0f

float SDF(float3 p /*, float4 s sphere*/)
{
    return length(p);
}


float rayMarch(float3 ro, float3 rd)
{
	float d = 0;
	
	float3 curPos = ro;
	
	for(int i = 0; i < STEPS; ++i)
	{	
		curPos = curPos + (rd * STEP_SIZE);
        float distFromCenter = SDF(curPos/*, float4(curPos.x, curPos.y, curPos.z, 0.02f)*/);

		if(distFromCenter < 1.0f)
			d += STEP_SIZE;
		
		if (d >= 0.99f) break;

		//won't happen because of how steps are set up
		//if(distFromCenter > sqrt(3.f)) break;
	}

	return d;
}

//------------------------------- Failed Lighting ---------------------------------//

float sphereNorm(float3 n, float4 s)
{
    return length(n - s.xyz) - s.w;
}

float3 estimateNormal(float3 p)
{
    const float EPSILON = 0.01;

    return normalize(float3(
        sphereNorm(float3(p.x + EPSILON, p.y, p.z), float4(0, 0, 0, 0.5)) - sphereNorm(float3(p.x - EPSILON, p.y, p.z), float4(0, 0, 0, 0.5)),
        sphereNorm(float3(p.x, p.y + EPSILON, p.z), float4(0, 0, 0, 0.5)) - sphereNorm(float3(p.x, p.y - EPSILON, p.z), float4(0,0,0, 0.5)),
        sphereNorm(float3(p.x, p.y, p.z + EPSILON), float4(0, 0, 0, 0.5)) - sphereNorm(float3(p.x, p.y, p.z - EPSILON), float4(0,0,0, 0.5))
    ));
}

float4 VolumetricPixelShader(PixelInputType input) : SV_TARGET
{
	float4 color;
    float3 lightDir;
    float lightIntensity;
    float4 textureColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

    /*--------------------------- Lerp the container opacity------------------------------------*/

    float elapsed = timeElapsed * .001f; //smaller the number, calmer the waves
    float initLerp = sin(elapsed);

    /*------------------------------- Volumetric Cube Ray Marching Call -------------------------*/
	float3 rayOrigin = input.wPos;

	float3 rayDirection = input.dirCalc.xyz - cameraPosition.xyz;

	rayDirection = normalize(rayDirection);

    float den = rayMarch(rayOrigin, rayDirection);
    
    /*----------------------------------- Lighting -----------------------------------------*/
    // Invert the light direction for calculations.
    lightDir = -lightDirection;

	// Calculate the amount of light on this pixel.
    //lightIntensity = saturate(dot(input.normal, lightDir));
    //lightIntensity = max(dot(estimateNormal(rayOrigin), lightDir), 0);
    //lightIntensity = saturate(dot(estimateNormal(rayOrigin), lightDir));

    /*------------------------- Final Output with Texture Applied ------------------*/
	//makes the box light up too if the first value is < 0
    den = smoothstep(initLerp, 1.0, den);

    //final color multiplied
    color = float4(den, den, den, den);
    
	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    //color = color * saturate(diffuseColor * lightIntensity);

    // Multiply the texture pixel and the final diffuse color to get the final pixel color result
    color *= textureColor;

    return color;
}