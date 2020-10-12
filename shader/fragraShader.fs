#version 330 core
out vec4 FragColor;

in vec2 texcoord; 

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{

    FragColor =  texture2D(texture1, texcoord);
    vec2 center = vec2(1184/2,565/2);
    vec2 current = vec2(gl_FragCoord.x - center.x,gl_FragCoord.y-center.y);
    float len = length(current);
   // gl_FragDepth = 0.5;//1*sin(len);
} 