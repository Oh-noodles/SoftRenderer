#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;
	std::vector<Vec3f> textureVerts_;
	std::vector<Vec3f> normalVerts_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	int nTextureVerts();
	int nNormalVerts();
	Vec3f vert(int i);
	Vec3f textureVert(int i);
	Vec3f normalVert(int i);
	std::vector<int> face(int idx);
};

#endif //__MODEL_H__
