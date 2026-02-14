struct MainMeshUB {
	float4x4 model;
	float4x4 view;
	float4x4 proj;
};
ConstantBuffer<MainMeshUB> uboMain;

struct VSInput {
    float3 inPosition;
    float2 inUV;
};

struct VertexOutput {
    float2 uv;
    float4 sv_position : SV_Position;
};

[shader("vertex")]
VertexOutput vertMain(VSInput input) {
    VertexOutput output;
    output.sv_position = mul( uboMain.proj, mul( uboMain.view, mul( uboMain.model, float4(input.inPosition, 1.0) ) ) );
    output.uv = input.inUV;
    return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
    return float4(inVert.uv, 0, 1.0);
}
