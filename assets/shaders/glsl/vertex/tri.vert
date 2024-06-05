#version 460

layout(location = 0) out vec3 outColor;

layout(push_constant) uniform constants
{
    vec4 data1;
    vec4 data2;
    vec4 data3;
    vec4 data4;
} PushConstants;

void main()
{
    //const array of positions for the triangle
    const vec3 positions[3] = vec3[3](
            vec3(0.1f + sin(PushConstants.data1[0] / 5000), 0.1f + cos(PushConstants.data1[0] / 5000), 0.0f),
            vec3(-0.1f + sin(PushConstants.data1[0] / 5000), 0.1f + cos(PushConstants.data1[0] / 5000), 0.0f),
            vec3(sin(PushConstants.data1[0] / 5000), -0.1f + cos(PushConstants.data1[0] / 5000), 0.0f)
        );

    //const array of colors for the triangle
    const vec3 colors[3] = vec3[3](
            vec3(1.0f, 0.0f, 0.0f), //red
            vec3(0.0f, 1.0f, 0.0f), //green
            vec3(0.0f, 0.0f, 1.0f) //blue
        );

    //output the position of each vertex
    gl_Position = vec4(positions[gl_VertexIndex], 1.0f);
    outColor = colors[gl_VertexIndex];
}
