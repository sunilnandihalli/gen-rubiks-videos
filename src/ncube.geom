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
uniform int coordId[6]= int[6](2,1,0,2,1,0);
uniform vec3 coordSelectorVec[3]=vec3[3](vec3(0,1,1),vec3(1,0,1),vec3(1,1,0));
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

void drawQuad(vec3 u0,vec3 u1,vec3 u2,vec3 u3,vec4 color) {
  vec4 v0 = vtx[0].trfMat*vec4(u0,1.0);
  vec4 v1 = vtx[0].trfMat*vec4(u1,1.0);
  vec4 v2 = vtx[0].trfMat*vec4(u2,1.0);
  vec4 v3 = vtx[0].trfMat*vec4(u3,1.0);
     
     VertexOut.color = color;
     gl_Position = v0;
     EmitVertex();
     gl_Position = v1;
     EmitVertex();
     gl_Position = v2;
     EmitVertex();
     EndPrimitive();
     
     gl_Position = v0;
     EmitVertex();
     gl_Position = v2;
     EmitVertex();
     gl_Position = v3;
     EmitVertex();
     EndPrimitive(); 
}

void drawQuadWithBoundary(vec3 v0,vec3 v1,vec3 v2,vec3 v3,int faceid) {
       vec3 r = vec3(1,1,1)-coordSelectorVec[coordId[faceid]]*0.5;
     vec3 rv0 = r*v0;
     vec3 rv1 = r*v1;
     vec3 rv2 = r*v2;
     vec3 rv3 = r*v3;
     drawQuad(rv0,rv1,rv2,rv3,scolors[faceid]);
     drawQuad(rv1,rv0,v0,v1,black);
     drawQuad(rv2,rv1,v1,v2,black);
     drawQuad(rv3,rv2,v2,v3,black);
     drawQuad(rv0,rv3,v3,v0,black);
}

void main() {
  for(int i=0;i<6;i++) {
    vec3 v0 = v[f[i][0]];
    vec3 v1 = v[f[i][1]];
    vec3 v2 = v[f[i][2]];
    vec3 v3 = v[f[i][3]];
    if(0 != ((vtx[0].id)&(1<<i))) {
    	drawQuadWithBoundary(v0,v1,v2,v3,i);
	//drawQuad(v0,v1,v2,v3,scolors[i]);
    } else {
        	drawQuadWithBoundary(v0,v1,v2,v3,i);
	//drawQuad(v0,v1,v2,v3,black);
	//	drawQuad(v0,v1,v2,v3,scolors[i]);	
    }    
  }
}
