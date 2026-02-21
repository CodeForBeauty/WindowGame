struct MainMeshUB {
	float4x4 model;
	float4x4 view;
	float4x4 proj;
};
ConstantBuffer<MainMeshUB> uboMain;

struct VSInput {
    float3 inPosition;
    float3 inNormal;
    float2 inUV;
};

struct VertexOutput {
    float4 sv_position : SV_Position;
    float3 normal;
    float2 uv;
};

[shader("vertex")]
VertexOutput vertMain(VSInput input) {
    VertexOutput output;
    output.sv_position = mul(uboMain.proj, mul(uboMain.view, mul(uboMain.model, float4(input.inPosition, 1.0))));
    output.uv = input.inUV;
    output.normal = input.inNormal;
    return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
    return float4(inVert.normal, 1.0);
}
