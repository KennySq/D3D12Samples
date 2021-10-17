struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput vert(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = position;
    result.color = color;

    return result;
}

float4 frag(PSInput input) : SV_TARGET
{
    return input.color;
}
