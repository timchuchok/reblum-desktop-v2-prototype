#version 440

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D texSampler;

layout(std140, binding = 2) uniform FragBuf {
    float orangeOpacity;
    float orangeThreshold;
    float greenOpacity;
    float greenThreshold;
    int   orangeEnabled;
    int   greenEnabled;
    // 8 bytes implicit padding before vec4
    vec4  orangeColor;
    vec4  greenColor;
}
effects;

void main() {
    vec4 color = texture(texSampler, v_uv);
    float lum = dot(color.rgb, vec3(0.299, 0.587, 0.114));

    if (effects.orangeEnabled != 0 && lum > effects.orangeThreshold) {
        color.rgb =
            mix(color.rgb, effects.orangeColor.rgb, effects.orangeOpacity);
    }

    if (effects.greenEnabled != 0 && lum < effects.greenThreshold) {
        color.rgb =
            mix(color.rgb, effects.greenColor.rgb, effects.greenOpacity);
    }

    fragColor = color;
}
