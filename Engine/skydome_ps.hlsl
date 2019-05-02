////////////////////////////////////////////////////////////////////////////////
// Filename: skydome.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer GradientBuffer
{
	float4 apexColor;
	float4 centerColor;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
	float4 domePosition : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 SkyDomePixelShader(PixelInputType input) : SV_TARGET
{
	float height = input.domePosition.y;
	float4 	outputColor;
	float4 bottomColor = float4(0.2f, 0.7f, 0.2f, 1.0f);

	//Sky
	if(height > 0.5)
	{
		outputColor = lerp(centerColor, apexColor, height);	
	}

	//Horizon
	// The value ranges from -1.0f to +1.0f so change it to only positive values.
	if(height < 0.0)
	{
		outputColor = lerp(centerColor, apexColor, height);
	}

	//bottom
	if( 0.0 < height < 0.5)
	{
		//height = smoothstep(0.5, 0.0, height);
		//outputColor = lerp(centerColor, bottomColor, height);

		outputColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
		
	}

    return outputColor;
}