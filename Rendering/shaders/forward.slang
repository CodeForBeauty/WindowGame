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
    float3 viewPos;
    float3 normal;
    float2 uv;
};

[shader("vertex")]
VertexOutput vertMain(VSInput input) {
    VertexOutput output;
    output.sv_position = mul(uboMain.proj, mul(uboMain.view, mul(uboMain.model, float4(input.inPosition, 1.0))));
    output.uv = input.inUV;
    output.normal = mul(uboMain.view, mul(uboMain.model, float4(input.inNormal, 0.0))).xyz;
    output.viewPos = mul(uboMain.view, mul(uboMain.model, float4(input.inPosition, 1.0))).xyz;
    return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
    const float3 lightDir = normalize(float3(-0, -1.5, -1.5));
    //const float3 lightDir = float3(0, -0.5, -0.5);
    float lambertian = max(dot(inVert.normal, -lightDir), 0);

    float3 viewDir = normalize(-inVert.viewPos);
    float3 halfDir = normalize(-lightDir + viewDir);
    float specAngle = max(dot(halfDir, inVert.normal), 0);
    float specular = pow(specAngle, 16);

    //return float4(abs(viewDir), 1);
    return float4(lambertian + specular);
    //return float4(inVert.normal, 1.0);
}
