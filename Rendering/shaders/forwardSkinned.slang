struct StaticDrawDataUB {
	float4x4 view;
	float4x4 proj;
};
[vk_binding(0, 0)] uniform ConstantBuffer<StaticDrawDataUB> uboMain;

struct PerObjectData {
    float4x4 model;
};
[vk_push_constant] uniform PerObjectData pcObjData;

[vk_binding(1, 0)] uniform Sampler2D allTextures[];

[vk_binding(0, 1)] ConstantBuffer<float4x4[]> uInverseBind;
[vk_binding(1, 1)] ConstantBuffer<float4x4[]> uPose;

struct VSInput {
    [vk_location(0)] float3 inPosition;
    [vk_location(1)] float3 inNormal;
    [vk_location(2)] float2 inUV;

    [vk_location(3)] float4 inBoneWeights;
    [vk_location(4)] uint4 inBoneIndices;
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

    float4 tmpPos = float4(0);
    float3 tmpNormal = float3(0);

    for (int i = 0; i < 4; i++) {
        uint idx = input.inBoneIndices[i];

        float4x4 finalBone = mul(uPose[idx], uInverseBind[idx]);
        
        tmpPos += mul(finalBone, float4(input.inPosition, 1)) * input.inBoneWeights[i];
        tmpNormal += mul(float3x3(finalBone), input.inNormal) * input.inBoneWeights[i];
    }

    tmpNormal = normalize(tmpNormal);

    float4x4 modelView = mul(uboMain.view, pcObjData.model);

    output.sv_position = mul(uboMain.proj, mul(modelView, tmpPos));
    output.uv = input.inUV;
    output.normal = mul(float3x3(modelView), tmpNormal);
    output.viewPos = mul(modelView, tmpPos).xyz;
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

    // return float4(abs(viewDir), 1);
    return float4((lambertian + specular) * allTextures[0].Sample(inVert.uv));
    //return float4(lambertian + specular);
    //return float4(inVert.normal, 1.0);
}
