// csv2obj ... a tool to convert csv files (preferably from tiled) to a polygon
// original version by xhighway999

// mesh in obj file format

// this software is licenced under the UNILICENCE
/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org/>
*/

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Mesh {
  struct Position {
    float x, y, z;
  };
  struct Vertex {
    Vertex() = default;
    Vertex(float a, float b, float c, float u = 0, float v = 0) {
      x = a;
      y = b;
      z = c;
      this->u = u;
      this->v = v;
    }
    Vertex(Position p) {
      x = p.x;
      y = p.y;
      z = p.z;
    }
    float x, y, z, u, v;
  };
  std::vector<Vertex> vertices;
  std::vector<unsigned int> faces;

  void addQuad(Position a, Position b, Position c, Position d) {
    Vertex vertices[] = {
        {a}, // top right
        {b}, // bottom right
        {c}, // bottom left
        {d}  // top left
    };
    unsigned int indices[] = {
        // note that we start from 0!
        1, 0, 3, // first triangle
        2, 1, 3  // second triangle
    };
    size_t idxOffset = this->vertices.size();
    for (unsigned long i = 0; i < 6; i++) {
      indices[i] += idxOffset;
    }
    this->vertices.insert(this->vertices.end(), std::begin(vertices),
                          std::end(vertices));

    this->faces.insert(this->faces.end(), std::begin(indices),
                       std::end(indices));
  }

  void addWall(size_t x, size_t y, bool sides[4], float height = 2.0) {
    // above, below, left, right
    // addFloor(x,y,height);

    std::vector<Vertex> vertices = {
        {0.5f + x, 0.5f + y, 0.0f, 1, 0},   // top right       0
        {0.5f + x, -0.5f + y, 0.0f, 0, 0},  // bottom right     1
        {-0.5f + x, -0.5f + y, 0.0f, 0, 1}, // bottom left     2
        {-0.5f + x, 0.5f + y, 0.0f, 1, 1},  // top left       3

        {0.5f + x, 0.5f + y, height, 0, 0},   // top right t    4
        {0.5f + x, -0.5f + y, height, 1, 0},  // bottom right t 5
        {-0.5f + x, -0.5f + y, height, 1, 1}, // bottom left t 6
        {-0.5f + x, 0.5f + y, height, 0, 1}   // top left t    7
    };

    unsigned int localIdxShift = 8;

    std::vector<unsigned int> indices = {5, 4, 7, 6, 5, 7};

    if (sides[0]) {
      Vertex faceVertices[] = {
          {0.5f + x, 0.5f + y, 0.0f, 0, 0},  // top right       0
          {-0.5f + x, 0.5f + y, 0.0f, 1, 0}, // top left       1

          {0.5f + x, 0.5f + y, height, 0, 1}, // top right t    2
          {-0.5f + x, 0.5f + y, height, 1, 1} // top left t    3
      };

      indices.insert(indices.end(),
                     {localIdxShift + 3, localIdxShift + 2, localIdxShift + 0,
                      localIdxShift + 0, localIdxShift + 1, localIdxShift + 3});
      localIdxShift += 4;
      vertices.insert(vertices.end(), faceVertices, faceVertices + 4);
    }

    if (sides[1]) {

      Vertex faceVertices[] = {
          {0.5f + x, -0.5f + y, 0.0f, 1, 0},  // bottom right     0
          {-0.5f + x, -0.5f + y, 0.0f, 0, 0}, // bottom left     1

          {0.5f + x, -0.5f + y, height, 1, 1},  // bottom right t 2
          {-0.5f + x, -0.5f + y, height, 0, 1}, // bottom left t 3
      };
      // 6 = 3, 5 = 2, 2 = 1, 1= 0

      indices.insert(indices.end(),
                     {localIdxShift + 3, localIdxShift + 1, localIdxShift + 0,
                      localIdxShift + 0, localIdxShift + 2, localIdxShift + 3});

      localIdxShift += 4;
      vertices.insert(vertices.end(), faceVertices, faceVertices + 4);
    }

    if (sides[2]) {
      Vertex faceVertices[] = {
          {0.5f + x, 0.5f + y, 0.0f, 1, 0},    // top right       0
          {0.5f + x, -0.5f + y, 0.0f, 0, 0},   // bottom right     1
          {0.5f + x, 0.5f + y, height, 1, 1},  // top right t    4
          {0.5f + x, -0.5f + y, height, 0, 1}, // bottom right t 5
      };

      indices.insert(indices.end(),
                     {localIdxShift + 2, localIdxShift + 3, localIdxShift + 1,
                      localIdxShift + 1, localIdxShift + 0, localIdxShift + 2});

      localIdxShift += 4;

      vertices.insert(vertices.end(), faceVertices, faceVertices + 4);
    }

    if (sides[3]) {

      Vertex faceVertices[] = {
          {-0.5f + x, -0.5f + y, 0.0f, 1, 0},   // bottom left     2
          {-0.5f + x, 0.5f + y, 0.0f, 0, 0},    // top left       3
          {-0.5f + x, -0.5f + y, height, 1, 1}, // bottom left t 6
          {-0.5f + x, 0.5f + y, height, 0, 1}   // top left t    7
      };
      indices.insert(indices.end(),
                     {localIdxShift + 2, localIdxShift + 3, localIdxShift + 1,
                      localIdxShift + 1, localIdxShift + 0, localIdxShift + 2});

      localIdxShift += 4;

      vertices.insert(vertices.end(), faceVertices, faceVertices + 4);
    }

    size_t idxOffset = this->vertices.size();
    for (unsigned long i = 0; i < indices.size(); i++) {
      indices[i] += idxOffset;
    }
    this->vertices.insert(this->vertices.end(), std::begin(vertices),
                          std::end(vertices));

    this->faces.insert(this->faces.end(), std::begin(indices),
                       std::end(indices));
  }

  void addFloor(size_t x, size_t y, float height = 0.0) {
    Vertex vertices[] = {
        {0.5f, 0.5f, 0.0f, 0, 1},   // top right
        {0.5f, -0.5f, 0.0f, 1, 1},  // bottom right
        {-0.5f, -0.5f, 0.0f, 1, 0}, // bottom left
        {-0.5f, 0.5f, 0.0f, 0, 0}   // top left
    };
    unsigned int indices[] = {
        // note that we start from 0!
        1, 0, 3, // first triangle
        2, 1, 3  // second triangle
    };
    for (auto &vertex : vertices) {
      vertex.x += x;
      vertex.y += y;
      vertex.z += height;
    }

    size_t idxOffset = this->vertices.size();
    for (unsigned long i = 0; i < 6; i++) {
      indices[i] += idxOffset;
    }
    this->vertices.insert(this->vertices.end(), std::begin(vertices),
                          std::end(vertices));

    this->faces.insert(this->faces.end(), std::begin(indices),
                       std::end(indices));
  };
};

void saveAsObj(std::string path, Mesh &map) {
  std::string obj = "#generated by csv2obj, developed by xhighway999\n";
  for (size_t i = 0; i < map.vertices.size(); i++) {
    char data[512];
    auto &vert = map.vertices;
    sprintf(data, "v %f %f %f \n", vert[i].x, vert[i].y, vert[i].z);
    obj += data;
  }
  // do the same thing foor texture coordinates
  for (size_t i = 0; i < map.vertices.size(); i++) {
    char data[512];
    auto &vert = map.vertices;
    sprintf(data, "vt %f %f \n", vert[i].u, vert[i].v);
    obj += data;
  }

  obj += "#face data\n";
  for (size_t i = 0; i < map.faces.size(); i += 3) {
    char data[512];
    auto &face = map.faces;
    // obj indices start at 1
    // set the same index twice as texture coordinate index == vertex coordinate
    // index
    auto a = 1 + face[i + 0];
    auto b = 1 + face[i + 1];
    auto c = 1 + face[i + 2];
    sprintf(data, "f %u/%u %u/%u %u/%u \n", a, a, b, b, c, c);
    obj += data;
  }
  std::ofstream of(path);
  of << obj;
  of.close();
}

struct Map {
  std::vector<long int> data;
  size_t width, height = 0;
  bool blockAt(long long x, long long y) {
    // out of bounds is always true
    if (x < 0 || x > width) {
      return true;
    }
    if (y < 0 || y > height) {
      return true;
    }
    size_t i = y * width + x;
    return data[i] != -1;
  };
};

int main(int argc, char **argv) {
  // argv[0] *should* be the path to the executable
  if (argc < 3) {
    std::cout << "Usage: csv2obj input(path) output(path) walls(boolean))"
              << std::endl;
    return -1;
  }
  std::string csvPath = argv[1];
  std::string meshPath = argv[2];

  std::string walls = argv[3];
  bool wall = false;

  auto up = [](std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return s;
  };

  walls = up(walls);

  if (walls == "TRUE" || walls == "1" || walls == "ON") {
    wall = true;
  }

  std::ifstream iffile(csvPath);
  if (!iffile.is_open()) {
    perror("Error open");
    return -1;
  }
  Map map;
  std::string line;
  while (getline(iffile, line)) {
    size_t rowWidth = 0;
    std::stringstream linestream(line);
    std::string value;
    while (getline(linestream, value, ',')) {
      map.data.push_back(std::stol(value));
      rowWidth++;
    }
    map.width = rowWidth;
    map.height++;
  }
  // convert stuff
  Mesh m;
  for (size_t i = 0; i < map.data.size(); ++i) {
    size_t y = i / map.width;
    size_t x = i % map.width;
    if (map.data[i] != -1) {
      if (wall) {
        bool above, left, right, below;
        above = map.blockAt(x, y - 1);
        below = map.blockAt(x, y + 1);
        left = map.blockAt(x - 1, y);
        right = map.blockAt(x + 1, y);
        bool sides[4] = {!below, !above, !right, !left};
        m.addWall(x, y, sides);
      } else {
        m.addFloor(x, y);
      };
    }
  }
  saveAsObj(meshPath, m);

  return 0;
}
