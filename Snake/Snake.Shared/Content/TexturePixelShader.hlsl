// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float4 tangent : TANGENT0;
	float4 color : COLOR0;
	float2 tex : TEXCOORD;
};
Texture2D gTexture;
SamplerState sampleState;

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	//return input.color;
	float4 tex = gTexture.Sample(sampleState, input.tex);
	return tex;
}
