struct VSOutput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOutput VSMain(float4 position : POSITION, float4 color : COLOR) {
    VSOutput result;

    result.position = position;
    result.color = color;

    return result;
}

float4 PSMain(VSOutput input) : SV_Target {
    return input.color;
}