#ifndef EXPERIMENTAL_USERS_SUNILSN_EDWARD_RUBIKS_CUBE_H_
#define EXPERIMENTAL_USERS_SUNILSN_EDWARD_RUBIKS_CUBE_H_
#include <algorithm>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <vector>
#include <thread>
#include <mutex>

//#define GLM_GTC_matrix_transform
#include <glm/gtc/matrix_transform.hpp>
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
static float piby2 = 3.14159265358979323846/2;

enum CARTSIDE : uint8_t { PZ, PY, NX, NZ, NY, PX, NONE };
enum SIDE : uint8_t { FRONT, TOP, LEFT, BACK, BOTTOM, RIGHT, INVALIDSIDE };
const std::vector<const char*> SIDEstr({"front", "top", "left", "back",
                                        "bottom", "right"});

const glm::vec3 dir[6] = {glm::vec3(0,0,1), glm::vec3(0,1,0), glm::vec3(-1,0,0),
			  glm::vec3(0,0,-1),glm::vec3(0,-1,0), glm::vec3(1,0,0)};
enum DIR { CW, CCW };
const std::vector<const char*> DIRstr({"CW","CCW"});
enum COLOR { RED, WHITE, GREEN, ORANGE, YELLOW, BLUE };
const uint8_t OrdersCCW[6][4] = {
    {TOP, LEFT, BOTTOM, RIGHT}, {FRONT, RIGHT, BACK, LEFT},
    {BOTTOM, FRONT, TOP, BACK}, {RIGHT, BOTTOM, LEFT, TOP},
    {LEFT, BACK, RIGHT, FRONT}, {BACK, TOP, FRONT, BOTTOM}};

const uint8_t pieces1[6][1] = {{FRONT}, {TOP},    {LEFT},
                               {BACK},  {BOTTOM}, {RIGHT}};
const uint8_t pieces2[12][2] = {{FRONT, TOP},    {FRONT, LEFT}, {FRONT, BOTTOM},
                                {FRONT, RIGHT},  {TOP, LEFT},   {LEFT, BOTTOM},
                                {BOTTOM, RIGHT}, {TOP, RIGHT},  {BACK, RIGHT},
                                {BACK, BOTTOM},  {LEFT, BACK},  {TOP, BACK}};
const uint8_t pieces3[8][3] = {{FRONT, TOP, LEFT},     {FRONT, LEFT, BOTTOM},
                               {FRONT, BOTTOM, RIGHT}, {FRONT, RIGHT, TOP},
                               {BACK, RIGHT, BOTTOM},  {LEFT, BACK, BOTTOM},
                               {TOP, BACK, LEFT},      {TOP, RIGHT, BACK}};
const int pieceStart[4] = {0, sizeof(pieces1),
                           sizeof(pieces1) + sizeof(pieces2),
                           sizeof(pieces1) + sizeof(pieces2) + sizeof(pieces3)};

const int numOneDimPieces = 6;
const int numTwoDimPieces = 12;
const int numThreeDimPieces = 8;

const int numOneDimFields = 6 * 1;
const int numTwoDimFields = 12 * 2;
const int numThreeDimFields = 8 * 3;
const int totalFields = numOneDimFields + numTwoDimFields + numThreeDimFields;

DIR opp(int side);

class Cube {
 public:
  static int piece[totalFields];
  static uint8_t mp[6][2][6];
  /* We can't explore the transition from all possible orientations to this
   * orientation in genConfigs .. we will need a new function and we will leave
   * it for now as a future exercise!
   */
  /* 0 -> 6x6
   * 1 -> 12x24
   * 2 -> 8x24 */
  static std::vector<std::vector<std::vector<uint8_t>>> rotMatIds;
  static glm::mat4 pRotMat[24];
  static glm::mat4 pTransMat[26];
  static std::map<std::set<uint8_t>, int> pid;
  static void genConfigs();
  static void setupTranslateMats();
  std::mutex objectLock;
  void setupCurrentFrameData();
  glm::mat4 camera, projection, cubeTranslation, cubeRotation, rotatingSideTrf,
      pScale;
  DIR curSideRotDir;
  SIDE curSideRotAxis;
  glm::vec3 curCubeTranslation;
  glm::quat curCubeRotQuaternion;
  double curAngle;
  
  int width, height;
  int pos[totalFields];
  uint8_t renderingData[78];
  void genRenderingData();
  Cube();
  bool operator==(const Cube& c) const;
  static void rotate(Cube& c, SIDE side, DIR dir);
  static void init();
  std::thread* animate(int n=10,int durationSecs=20,int minTimeSecsPerTurn=2,int fps=60);
  glm::mat4 viewport(int vl, int vb, int vr, int vt);
};
#endif  
