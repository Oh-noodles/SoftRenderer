#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        // vertix
        if (!line.compare(0, 2, "v ")) {
            char cTrash;
            float x, y, z;
            iss >> cTrash >> x >> y >> z;
            Vec3f v(x, y, z);
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            char cTrash;
            int x, y, z, vt0, vt1, vt2, iTrash;
            iss >> cTrash;
            iss >> x >> cTrash >> vt0 >> cTrash >> iTrash;
            iss >> y >> cTrash >> vt1 >> cTrash >> iTrash;
            iss >> z >> cTrash >> vt2 >> cTrash >> iTrash;
            faces_.push_back({x-1, y-1, z-1, vt0-1, vt1-1, vt2-1});
        } else if (!line.compare(0, 3, "vt ")) {
            std::string s;
            float x, y, z;
            iss >> s >> x >> y >> z;
            textureVerts_.push_back(Vec3f(x, y, z));
        }
    }
}

Model::~Model() {}

Vec3f Model::vert(int i) {
    return verts_[i];
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::textureVert(int i) {
    return textureVerts_[i];
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

int Model::nTextureVerts() {
    return (int)textureVerts_.size();
}