#version 440

layout(location = 0) out vec2 v_uv;

layout(std140, binding = 0) uniform VertBuf {
    mat4 corrMatrix;
    vec4 imgRect; // left, top, right, bottom in pre-correction OpenGL NDC (y-up)
}
ubo;

void main() {
    // Triangle strip: 0=TL, 1=BL, 2=TR, 3=BR
    float x = (gl_VertexIndex >= 2) ? ubo.imgRect.z : ubo.imgRect.x;
    float y = ((gl_VertexIndex & 1) != 0) ? ubo.imgRect.w : ubo.imgRect.y;

    gl_Position = ubo.corrMatrix * vec4(x, y, 0.0, 1.0);

    v_uv = vec2(
        (gl_VertexIndex >= 2) ? 1.0 : 0.0,
        ((gl_VertexIndex & 1) != 0) ? 1.0 : 0.0
    );
}
