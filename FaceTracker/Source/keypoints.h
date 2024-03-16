#ifndef KEYPOINTS_H
#define KEYPOINTS_H

#include <vector>
#include <map>
#include <string>

typedef std::vector<int> pointList;
typedef std::map<std::string,pointList> pointMap;

pointMap & getFaceKeyPoints();
std::vector<std::string> & getBoneList();

#endif // KEYPOINTS_H
