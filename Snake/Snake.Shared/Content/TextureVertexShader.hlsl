


cbuffer MaterialVars : register (b0)
{
	float4 MaterialAmbient;
	float4 MaterialDiffuse;
	float4 MaterialSpecular;
	float4 MaterialEmissive;
	float MaterialSpecularPower;
	row_major float4x4 UVTransform;
};

cbuffer ModelViewProjectionConstantBuffer
{
	float4x4 Model;
	float4x4 View;
	float4x4 Projection;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float4 pos : SV_Position;	
	float3 normal : NORMAL0;
	float4 tangent : TANGENT0;
	float4 color : COLOR0;
	float2 tex : TEXCOORD;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float4 tangent : TANGENT0;
	float4 color : COLOR0;
	float2 tex : TEXCOORD;
};


// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	// Transform the vertex position into projected space.
	float4 pos = mul(input.pos, Model);
	pos = mul(pos, View);
	pos = mul(pos, Projection);
	output.pos = pos;
	output.normal = input.normal;
	output.tangent = input.tangent;
	output.tex = mul(float4(input.tex.x, input.tex.y, 0, 1), UVTransform).xy;
	// Pass the color through without modification.
	output.color = input.color;
	return output;
}
