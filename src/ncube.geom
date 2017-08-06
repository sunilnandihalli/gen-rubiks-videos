#version 330 core
// #extension GL_EXT_geometry_shader : enable

const vec4 black = vec4(0.5,0.5,0.5,1);
const vec4 red = vec4(1,0,0,1);
const vec4 green = vec4(0,1,0,1);
const vec4 blue = vec4(0,0,1,1);
const vec4 white = vec4(1,1,1,1);
const vec4 orange = vec4(1,0.65,0,1);
const vec4 yellow = vec4(1,1,0,1);
uniform vec4 scolors[6] = vec4[6](red,green,blue,white,orange,yellow);
uniform vec3 v[8]=vec3[8](vec3(-1,-1,-1),vec3(-1,-1, 1),
                          vec3(-1, 1,-1),vec3(-1, 1, 1),
                          vec3( 1,-1,-1),vec3( 1,-1, 1),
                          vec3( 1, 1,-1),vec3( 1, 1, 1));
// all counter clockwise
uniform ivec4 f[6] = ivec4[6](ivec4(1,5,7,3),ivec4(3,7,6,2),ivec4(0,1,3,2),
                              ivec4(4,0,2,6),ivec4(0,4,5,1),ivec4(5,4,6,7));

layout(points) in;
layout(triangle_strip, max_vertices=36) out;

in VertexData {
  mat4 trfMat;
  int id;
} vtx[1];

out GeomData {
  vec4 color;
  vec3 normal;
} VertexOut;


void main() {
  ivec3 t[2] = ivec3[2](ivec3(0,1,2),ivec3(0,2,3));
  for(int i=0;i<6;i++) {
    VertexOut.color = (0 != ((vtx[0].id)&(1<<i))) ? scolors[i] : black;
    for(int triaid = 0;triaid<2;triaid++) {
      for(int j=0;j<3;j++) {
        gl_Position = vtx[0].trfMat*vec4(v[f[i][t[triaid][j]]],1.0);
        EmitVertex();
      }
      EndPrimitive();
    }
  }
}
