#version 330 core
//#extension GL_EXT_geometry_shader : enable

// in range 0 to 23
in int rotMatId;

// in range 0 to 26
in int transMatId;

// enum(NX,PX,NY,PY,NZ,PZ,ROTATINGSIDE);
// piece (NX,NY) id = 1<<NX|1<<NY
// piece (Nx,NY,NZ)  id = 1<<NX|1<<NY|1<<NZ
// 1 <= id <= 42
in int pieceId;

uniform mat4 projection;
uniform mat4 camera;
uniform mat4 cubeTranslation;
uniform mat4 cubeRotation;
uniform mat4 rotatingSideTrf;
uniform mat4 pTransMat[26];
uniform mat4 pRotMat[24];
uniform mat4 pScale;

out VertexData {
  mat4 trfMat;
  int id;
} vtx;

void main() {
  mat4 trf = projection*camera*cubeTranslation*cubeRotation;
  /* ROTATINGSIDE=6 */
  if ((pieceId & (1<<6)) != 0) {
    trf = trf * rotatingSideTrf;
  }
  vtx.trfMat = trf*pTransMat[transMatId]*pRotMat[rotMatId]*pScale;
  vtx.id = pieceId & 63;
}

