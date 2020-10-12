#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec4 m_n;
uniform vec3 direction;
uniform vec2 point;
uniform vec2 center;
uniform vec2 length;


uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;


out vec2 texcoord; 

#define PI  3.141592653589793238462643383279502884197169399


void main()
{
   int row = gl_InstanceID/(int(m_n.z));
   int col = gl_InstanceID%(int(m_n.z));
   texcoord = vec2(aTexCoord.x + col/m_n.z,aTexCoord.y + row/m_n.w);
   vec3 pos = vec3(aPos.x + col * m_n.x,aPos.y + row * m_n.y,0);

   // vec2 center1 = vec2(20,-20);
    vec2 current = vec2(pos.x - center.x,pos.y-center.y);
    float len = length(current);
    pos.z = length.y*cos(length.x*len);
   /*
   float distance = dot((point - pos.xy),direction.xy);
   float angle = PI - distance / direction.z;

   if(distance > 0){
      float h = distance - sin(angle)*direction.z;
      float z = direction.z + cos(angle)*direction.z;
        
      vec2 vD = pos.xy + h * direction.xy;
      pos = vec3(vD,z);
     
   }*/

   gl_Position =  projection * view * model * vec4(pos,1);
}
