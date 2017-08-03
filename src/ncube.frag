#version 330 core
//#extension GL_EXT_geometry_shader : enable

in GeomData {
  vec4 color;
} gdata;
out vec4 clr;
void main(void) {
  clr = gdata.color;
}
