#shader vertex
#version 460 core

layout (location = 0) in vec3 position;

out vec3 v_TexCoord;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
    vec4 pos = u_Projection * u_View * vec4(position, 1.0f);
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w); // Having z equal w will always result in a depth of 1.0f
    v_TexCoord = vec3(position.x, position.y, -position.z);   // We want to flip the z axis due to the different coordinate systems (left hand vs right hand)
}    

#shader fragment
#version 460 core

out vec4 color;

in vec3 v_TexCoord;

uniform samplerCube u_Skybox;

void main()
{    
    color = texture(u_Skybox, v_TexCoord);
}