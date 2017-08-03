#include "Cube.h"
#include <bitset>
#include <cmath>
#include <map>
#include <set>
#include <thread>
#include <chrono>

uint8_t Cube::mp[6][2][6];
int Cube::piece[54];
std::vector<std::vector<std::vector<uint8_t>>> Cube::rotMatIds;
glm::mat4 Cube::pRotMat[24];
glm::mat4 Cube::pTransMat[26];
std::map<std::set<uint8_t>, int> Cube::pid;
using std::cout;
using std::endl;
Cube::Cube() {
  int cumFieldId = 0;
  for (int i = 0; i < numOneDimPieces; i++, cumFieldId++) {
    pos[cumFieldId] = pieces1[i][0];
  }
  for (int i = 0; i < numTwoDimPieces; i++)
    for (int f = 0; f < 2; f++, cumFieldId++) {
      pos[cumFieldId] = pieces2[i][f];
    }
  for (int i = 0; i < numThreeDimPieces; i++)
    for (int f = 0; f < 3; f++, cumFieldId++) {
      pos[cumFieldId] = pieces3[i][f];
    }
}

DIR opp(int side) { return DIR((side + 3) % 6); }
void Cube::init() {
  int cumFieldId = 0;
  for (int i = 0; i < numOneDimPieces; i++, cumFieldId++) {
    piece[cumFieldId] = pieces1[i][0];
  }
  for (int i = 0; i < numTwoDimPieces; i++)
    for (int f = 0; f < 2; f++, cumFieldId++) {
      piece[cumFieldId] = pieces2[i][f];
    }
  for (int i = 0; i < numThreeDimPieces; i++)
    for (int f = 0; f < 3; f++, cumFieldId++) {
      piece[cumFieldId] = pieces3[i][f];
    }
  for (uint8_t side = 0; side < 6; side++) {
    mp[side][CW][side] = side;
    mp[side][CCW][side] = side;
    int oppSide = opp(side);
    mp[side][CW][oppSide] = oppSide;
    mp[side][CCW][oppSide] = oppSide;
    for (int i = 0; i < 4; i++) {
      int s = OrdersCCW[side][i];
      int e = OrdersCCW[side][(i + 1) % 4];
      mp[side][CCW][s] = e;
      mp[side][CW][e] = s;
    }
  }
  genConfigs();
  setupTranslateMats();
}

void Cube::rotate(Cube& c, SIDE s, DIR d) {
  int cumFieldId = numOneDimFields;
  for (int i = 0; i < numTwoDimPieces; i++) {
    if (std::set<int>({c.pos[cumFieldId], c.pos[cumFieldId + 1]}).count(s) !=
        0) {
      for (int f = 0; f < 2; f++)
        c.pos[cumFieldId + f] = mp[s][d][c.pos[cumFieldId + f]];
    }
    cumFieldId += 2;
  }
  for (int i = 0; i < numThreeDimPieces; i++) {
    if (std::set<int>(
            {c.pos[cumFieldId], c.pos[cumFieldId + 1], c.pos[cumFieldId + 2]})
            .count(s) != 0) {
      for (int f = 0; f < 3; f++)
        c.pos[cumFieldId + f] = mp[s][d][c.pos[cumFieldId + f]];
    }
    cumFieldId += 3;
  }
}

bool Cube::operator==(const Cube& c) const {
  for (int i = 0; i < totalFields; ++i) {
    if (pos[i] != c.pos[i]) return false;
  }
  return true;
}

void Cube::genConfigs() {
  rotMatIds.resize(3);
  rotMatIds[0].resize(6, std::vector<uint8_t>(6, -1));
  rotMatIds[1].resize(12, std::vector<uint8_t>(24, -1));
  rotMatIds[2].resize(8, std::vector<uint8_t>(24, -1));
  using std::set;
  using std::setw;
  for (int i = 0; i < 6; i++) {
    const uint8_t* p = &(pieces1[i][0]);
    pid[set<uint8_t>(p, p + 1)] = i;
  }
  for (int i = 0; i < 12; i++) {
    const uint8_t* p = &(pieces2[i][0]);
    pid[set<uint8_t>(p, p + 2)] = i;
  }
  for (int i = 0; i < 8; i++) {
    const uint8_t* p = &(pieces3[i][0]);
    pid[set<uint8_t>(p, p + 3)] = i;
  }
  std::vector<uint8_t> ort = {FRONT, TOP, LEFT, BACK, BOTTOM, RIGHT};
  std::vector<uint8_t> ops = {LEFT, FRONT, FRONT, FRONT, TOP};  // always rotate CCW
  std::vector<uint8_t> frontMapped = {
      FRONT, TOP, LEFT, BOTTOM, RIGHT, BACK};  //sequence locations of FRONT
  glm::mat4 m(1.0f);
  auto rot = [&m, &ort](uint8_t s, uint8_t d) {
    std::cout<<"-----------------------------------"<<std::endl;
    m = glm::rotate(m, -piby2, dir[s]);
    for (int i = 0; i < 6; i++) ort[i] = mp[s][d][ort[i]];
    std::vector<SIDE> cartIdTofaceId({RIGHT,TOP,FRONT});
    std::map<SIDE,int> faceIdTocartId(
    {{RIGHT,0},{TOP,1},{FRONT,2},{LEFT,3},{BOTTOM,4},{BACK,5}});
    for (int coordId = 0;coordId<3;coordId++) {
      int mappedCoordId = faceIdTocartId[SIDE(ort[cartIdTofaceId[coordId]])];
      for(int i=0;i<3;i++) m[coordId][i] = 0;
      m[coordId][mappedCoordId%3] = mappedCoordId>2?-1:1;      
    }
    for(uint8_t i = 0;i<6;i++) 
      std::cout<<std::setw(7)<<SIDEstr[i];
    std::cout<<std::endl;
    for(uint8_t i = 0;i<6;i++) 
      std::cout<<std::setw(7)<<SIDEstr[ort[i]];
    std::cout<<std::endl;

    for(int i=0;i<3;i++) {
      for(int j=0;j<3;j++)
	std::cout<<std::setw(3)<<(m[i][j]<-0.5?-1:(m[i][j]>0.5)?1:0);
      std::cout<<std::endl;
    }
    std::cout<<glm::to_string(m)<<std::endl;
    
  };
  const int intmax = 999999999;
  for (int confid = 0, i = 0; i < 6; i++) {
    for (int dimPieceId = 0; dimPieceId < 6; dimPieceId++)
      rotMatIds[0][dimPieceId][ort[dimPieceId]] = confid;
    for (int j = 0; j < 4; j++, confid++) {
      for (int dimPieceId = 0; dimPieceId < 12; dimPieceId++) {
        uint8_t p[2] = {ort[pieces2[dimPieceId][0]],
                        ort[pieces2[dimPieceId][1]]};
        int t = pid[set<uint8_t>({p[0], p[1]})];
        int locid;
        if (p[0] == pieces2[t][0])
          locid = t;
        else if (p[0] == pieces2[t][1])
          locid = t + 12;
        else
          locid = intmax;
        rotMatIds[1][dimPieceId][locid] = confid;
      }
      for (int dimPieceId = 0; dimPieceId < 8; dimPieceId++) {
        uint8_t p[3] = {ort[pieces3[dimPieceId][0]],
                        ort[pieces3[dimPieceId][1]],
                        ort[pieces3[dimPieceId][2]]};
        int t = pid[set<uint8_t>({p[0], p[1], p[2]})];
        int locid;
        if (p[0] == pieces3[t][0])
          locid = t;
        else if (p[0] == pieces3[t][1])
          locid = t + 8;
        else if (p[0] == pieces3[t][2])
          locid = t + 16;
        else
          locid = intmax;
        rotMatIds[2][dimPieceId][locid] = confid;
      }
      pRotMat[confid] = m;
      if (j != 3) rot(ort[FRONT], CCW);
    }
    if (i != 5) rot(ops[i], CCW);
  }
}
void Cube::setupTranslateMats() {
  using std::pair;
  using std::set;
  using std::vector;
  typedef vector<pair<double, uint8_t>> pairs;
  for (auto dx : pairs({{-1.0, NX}, {0.0, NONE}, {1.0, PX}}))
    for (auto dy : pairs({{-1.0, NY}, {0.0, NONE}, {1.0, PY}}))
      for (auto dz : pairs({{-1.0, NZ}, {0.0, NONE}, {1.0, PZ}})) {
        set<uint8_t> p({dx.second, dy.second, dz.second});
        p.erase(NONE);
        if (p.empty()) continue;
        int id = pid[p] + vector<int>({0, 6, 18})[p.size() - 1];
        pTransMat[id] = glm::translate(glm::mat4(1.0f),
                                       glm::vec3(dx.first, dy.first, dz.first));
      }
}
void Cube::setupCurrentFrameData() {
  camera =
      glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  projection = glm::perspective(20.0f, 1.0f, 5.0f, 15.0f);
  cubeTranslation = glm::translate(glm::mat4(1.0f), curCubeTranslation);
  cubeRotation = glm::toMat4(curCubeRotQuaternion);
  rotatingSideTrf = glm::rotate(glm::mat4(1.0f),
				float(curAngle*(curSideRotDir==CW?-1:1)),
				dir[curSideRotAxis]);
  pScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
  genRenderingData();
}

void Cube::genRenderingData() {
  typedef std::set<uint8_t> si;
  int cumFieldId = 0;
  int o = 0;
  const int intmax = 999999999;

  for (uint8_t i = 0; i < 6; i++, o += 3, cumFieldId += 1) {
    uint8_t& rotid(renderingData[o]);
    uint8_t& transid(renderingData[o + 1]);
    uint8_t& colorMask(renderingData[o + 2]);
    uint8_t p1(pos[cumFieldId]);
    int tmp = pid[si({p1})];
    rotid = rotMatIds[0][0][0];
    transid = tmp;
    colorMask = 1 << piece[cumFieldId];
    if(p1==curSideRotAxis) {
      colorMask = colorMask|(1<<6);
    }
  }
  for (int i = 0; i < 12; i++, o += 3, cumFieldId += 2) {
    uint8_t& rotid(renderingData[o]);
    uint8_t& transid(renderingData[o + 1]);
    uint8_t& colorMask(renderingData[o + 2]);
    uint8_t p1(pos[cumFieldId]), p2(pos[cumFieldId + 1]);
    int tmp = pid[si({p1, p2})];
    int locid;
    if (pieces2[tmp][0] == p1) {
      locid = tmp;
    } else if (pieces2[tmp][1] == p1) {
      locid = tmp + 12;
    } else {
      locid = intmax;
    }
    rotid = rotMatIds[1][i][locid];
    transid = 6 + tmp;
    colorMask = 1 << piece[cumFieldId] | 1 << piece[cumFieldId + 1];
    if(p1==curSideRotAxis || p2 == curSideRotAxis) {
      colorMask = colorMask | (1<<6);
    }
  }
  for (int i = 0; i < 8; i++, o += 3, cumFieldId += 3) {
    uint8_t& rotid(renderingData[o]);
    uint8_t& transid(renderingData[o + 1]);
    uint8_t& colorMask(renderingData[o + 2]);
    uint8_t p1(pos[cumFieldId]), p2(pos[cumFieldId + 1]),
        p3(pos[cumFieldId + 2]);
    int tmp = pid[si({p1, p2, p3})];
    int locid;
    if (pieces3[tmp][0] == p1) {
      locid = tmp;
    } else if (pieces3[tmp][1] == p1) {
      locid = tmp + 8;
    } else if (pieces3[tmp][2] == p1) {
      locid = tmp + 16;
    } else {
      locid = intmax;
    }
    rotid = rotMatIds[2][i][locid];
    transid = 18 + tmp;
    colorMask = 1 << piece[cumFieldId] | 1 << piece[cumFieldId + 1] |
                1 << piece[cumFieldId + 2];
    if(p1==curSideRotAxis || p2 == curSideRotAxis || p3 == curSideRotAxis) {
      colorMask = colorMask | (1<<6);
    }
  }
  for(int i=0;i<0/*26*/;i++) {
    renderingData[i*3] = rand()%24;
    std::cout<<int(renderingData[i*3])
	     <<std::setw(4)<<int(renderingData[i*3+1])
	     <<std::setw(4)<<int(renderingData[i*3+2])
	     <<std::setw(10)<<std::bitset<6>(renderingData[i*3+2])<<std::endl;
    }
}

void Cube::vertShader(int rotMatId, int transMatId, int pieceId,
                      glm::mat4& outTrfMat, int& outId) const {
  cout << " projection      : " << glm::to_string(projection) << endl;
  cout << " camera          : " << glm::to_string(camera) << endl;
  cout << " cubeTranslation : " << glm::to_string(cubeTranslation) << endl;
  cout << " cubeRotation    : " << glm::to_string(cubeRotation) << endl;
  cout << " rotatingSideTrf : " << glm::to_string(rotatingSideTrf) << endl;
  cout << " pscale          : " << glm::to_string(pScale) << endl;
  cout << " transMatId      : " << transMatId << endl;
  cout << " rotMatId        : " << rotMatId << endl;
  cout << " pTransMat       : " << glm::to_string(pTransMat[transMatId])
       << endl;
  cout << " pRotMat         : " << glm::to_string(pRotMat[rotMatId]) << endl;
  glm::mat4 trf = projection * camera * cubeTranslation * cubeRotation;
  if ((pieceId & (1 << 6)) == 1) {
    trf *= rotatingSideTrf;
  }
  outTrfMat = trf * pTransMat[transMatId] * pRotMat[rotMatId] * pScale;
  outId = pieceId & 63;
}

void Cube::geomShader(
    const glm::mat4& trfMat, const int& id,
    std::vector<std::vector<std::pair<glm::vec4, glm::vec4>>>& out) {
  cout << "trfMat : " << glm::to_string(trfMat) << endl
       << " id : " << id << endl;
  using glm::ivec3;
  using glm::ivec4;
  using glm::mat4;
  using glm::vec3;
  using glm::vec4;
  using std::pair;
  using std::vector;
  const vec4 black = vec4(0, 0, 0, 1);
  const vec4 red = vec4(1, 0, 0, 1);
  const vec4 green = vec4(0, 1, 0, 1);
  const vec4 blue = vec4(0, 0, 1, 1);
  const vec4 white = vec4(1, 1, 1, 1);
  const vec4 orange = vec4(1, 0.65, 0, 1);
  const vec4 yellow = vec4(1, 1, 0, 1);
  vec4 scolors[6] = {red, green, blue, white, orange, yellow};
  vec3 v[8] = {vec3(-1, -1, -1), vec3(-1, -1, 1), vec3(-1, 1, -1),
               vec3(-1, 1, 1),   vec3(1, -1, -1), vec3(1, -1, 1),
               vec3(1, 1, -1),   vec3(1, 1, 1)};
  // all counter clockwise
  ivec4 f[6] = {ivec4(1, 5, 7, 3), ivec4(3, 7, 6, 2), ivec4(0, 1, 3, 2),
                ivec4(4, 0, 2, 6), ivec4(0, 4, 5, 1), ivec4(5, 4, 6, 7)};
  ivec3 t[2] = {ivec3(0, 1, 2), ivec3(0, 2, 3)};
  for (int i = 0; i < 6; i++) {
    vec4 color = (1 == (id & (1 << i))) ? scolors[i] : black;
    for (int triaid = 0; triaid < 2; triaid++) {
      out.push_back(vector<pair<vec4, vec4>>());
      for (int j = 0; j < 3; j++) {
        vec4 gl_Position = trfMat * vec4(v[f[i][t[triaid][j]]], 1.0);
        out.back().push_back(
            pair<vec4, vec4>(gl_Position, color));  // EmitVertex();
      }
      // EndPrimitive();
    }
  }
}
glm::mat4 Cube::viewport(int vl, int vb, int vr, int vt) {
  glm::mat4 ret(0.0f);
  ret[0][0] = 0.5 * (vr - vl);
  ret[1][1] = 0.5 * (vt - vb);
  ret[2][2] = 0.5;

  ret[3][0] = 0.5 * (vr + vl);
  ret[3][1] = 0.5 * (vt + vb);
  ret[3][2] = 0.5;
  ret[3][3] = 1.0;
  return ret;
}

void Cube::geomToFragFPP(
    const std::vector<std::vector<std::pair<glm::vec4, glm::vec4>>>& in,
    int width, int height,
    std::vector<std::vector<std::pair<glm::vec4, glm::vec4>>>& out) {
  using std::abs;
  glm::mat4 vp = Cube::viewport(0, 0, width, height);
  cout << "vp : " << glm::to_string(vp) << endl;
  for (auto tria : in) {
    out.push_back(std::vector<std::pair<glm::vec4, glm::vec4>>());
    for (auto point_color : tria) {
      glm::vec4 tmp = point_color.first;
      cout << "before clipping division : " << glm::to_string(tmp) << endl;
      tmp /= tmp[3];
      cout << "after clipping division : " << glm::to_string(tmp) << endl;
      if (false ||
          (abs(tmp[0]) <= 1.0 && abs(tmp[1]) <= 1.0 && abs(tmp[2]) <= 1.0)) {
        tmp = vp * tmp;
        out.back().push_back(
            std::pair<glm::vec4, glm::vec4>(tmp, point_color.second));
      }
    }
  }
}

void Cube::shaderProgram(
    int rotMatId, int transMatId, int pieceId,
    std::vector<std::vector<std::pair<glm::vec4, glm::vec4>>>& out) {
  glm::mat4 outTrfMat;
  using std::abs;
  int outId;
  vertShader(rotMatId, transMatId, pieceId, outTrfMat, outId);
  std::vector<std::vector<std::pair<glm::vec4, glm::vec4>>> geomPoints;
  geomShader(outTrfMat, outId, geomPoints);
  geomToFragFPP(geomPoints, 100, 100, out);
}

static int max(int a,int b) {
  if(a>b) return a; else return b;
}

std::thread* Cube::animate(int n,int durationSecs,
		   int minTimeSecsPerTurn,int fps) {
  n=60;
  using glm::vec3;
  std::vector<glm::vec3> positions({vec3(0,0,0)/*,vec3(0,1,0)*/,vec3(0,0,0)});
  std::vector<glm::quat> quats({
      glm::angleAxis(float(piby2*2),glm::normalize(vec3(1,1,1))),
		glm::angleAxis(float(piby2/2),dir[0]),
	glm::angleAxis(float(piby2/2),dir[1]),
	glm::angleAxis(float(piby2/2),dir[2]),
	glm::angleAxis(float(piby2/2),dir[3]),
	glm::angleAxis(float(piby2/2),dir[4]),
	glm::angleAxis(float(piby2/2),dir[5]),
	glm::angleAxis(float(piby2*2),glm::normalize(vec3(1,1,1)))});
  std::vector<std::pair<SIDE,DIR>> moves;
  
  int actualTotalDuration = max(durationSecs,n*minTimeSecsPerTurn);
  int numFrames = n*fps;
  int sleepTimeMillis = (actualTotalDuration*1000)/numFrames;
  for(int i=0;i<n;i++)    {
    //moves.push_back(std::pair<SIDE,DIR>(SIDE(3),DIR(0)));
    //moves.push_back(std::pair<SIDE,DIR>(SIDE(0),DIR(1)));
    moves.push_back(std::pair<SIDE,DIR>(SIDE(rand()%6),DIR(rand()%2)));
  }
  auto f = [fps,quats,positions,numFrames,this,moves,sleepTimeMillis](Cube* c) {
    
    int currentPosId = -1;
    int currentQuatId = -1;
    int curMove = -1;
    int frameId = 0;
    float dtheta = piby2/(fps-1);
    float theta = 0.0;
    int numFramesPerQuat = numFrames/(quats.size()-1);
    int numFramesPerPos = numFrames/(positions.size()-1);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    while(frameId<numFrames) {
      {
	std::lock_guard<std::mutex> guard(objectLock);
    if(frameId%fps == 0) {
      if(curMove>-1) 
	Cube::rotate(*c,curSideRotAxis,curSideRotDir);

        curMove++;
	curSideRotAxis = moves[curMove].first;
	curSideRotDir = moves[curMove].second;
	std::cout<<"curSideRotAxis : "<<curSideRotAxis<<" curSideRotDir : "<<curSideRotDir<<" angle : "<<theta<<std::endl;
	theta = dtheta;
      } else {
	theta += dtheta;
      }
      curAngle = theta;
      if(((frameId%numFramesPerQuat) == 0) && (currentQuatId < int(quats.size()-1))) {
	currentQuatId++;
      }
      {
	float a = float(frameId%numFramesPerQuat)/numFramesPerQuat;
	curCubeRotQuaternion =
	  quats[currentQuatId]*(1-a)+
	  quats[currentQuatId+1]*a;
      }
      if(((frameId%numFramesPerPos) == 0) && (currentPosId < int(positions.size()-1))) {
	currentPosId++;
      }
      {
	float a = float(frameId%numFramesPerPos)/numFramesPerPos;
	curCubeTranslation =
	  positions[currentPosId]*(1-a)+
	  positions[currentPosId+1]*a;
      }
      setupCurrentFrameData();
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMillis));
      frameId++;
    };
  };
  return new std::thread(f,this);
}
