#include "CommonEngineStructs.h"

struct PSInput
{
	float4 position : SV_POSITION;
};

struct PSOutput
{
	float4 Color: SV_Target;
};

ConstantBuffer<ViewProjectionMatrixData> viewProjectionData : register(b0);
StructuredBuffer<Vertex> points : register(t0);

PSInput VSMain(uint id : SV_VertexID)
{
	PSInput result;
    result.position = mul(viewProjectionData.proj, mul(viewProjectionData.view, points[id].position));
	return result;
}

PSOutput PSMain(PSInput input) // : SV_TARGET
{
	PSOutput output;

    output.Color = float4(1, 1, 1, 1);
	
	return output;
}
