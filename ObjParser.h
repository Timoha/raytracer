#ifndef OBJPARSER_H
#define OBJPARSER_H

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include <Eigen/Dense>
#include <Eigen/StdVector>

#include "Shape.h"

using namespace std;

class ObjParser {

private:
    vector<Triangle*> triangles;
    vector<Eigen::Vector3f*> verticies;
    vector<Eigen::Vector4f*> normals;
    void parseLine(const string& line);
    vector<int> splitSlash(const vector<string>& tokens);

public:
    ObjParser(string filePath);
    vector<Triangle*>& getTriangles();
};


vector<Triangle*>& ObjParser::getTriangles() {
    return triangles;
}




ObjParser::ObjParser(string filePath){
    ifstream fin;
    string line;

    filePath.erase(0, 1);
    filePath.erase(filePath.size() - 1);

    fin.open(filePath);

    if (!fin.good()) {
        fin.close();
        throw invalid_argument( "File \"" + filePath + "\" does not exists");
    }

    while (getline(fin, line)) {

        try {
            parseLine(line);
        } catch (invalid_argument& e) {
            cerr << e.what() << endl;
        }
    }

    fin.close();

}



vector<int> ObjParser::splitSlash(const vector<string>& tokens) {
    vector<int> inds;

    for (int i = 1; i < tokens.size(); i++) {
        istringstream ss(tokens[i]);
        string item;
        while (getline(ss, item, '/')) {
            inds.push_back(atoi(item.c_str()));
        }
    }

    return inds;
}



void ObjParser::parseLine(const string& line) {
    istringstream iss(line);
    vector<string> tokens((istream_iterator<string>(iss)), istream_iterator<string>());

    if (tokens.size() == 0 || tokens[0] == "#") {
        return;
    }

    if (tokens[0] == "v") {
        Eigen::Vector3f* vertex = new Eigen::Vector3f(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
        if (tokens.size() == 5) {
            *vertex = *vertex / atof(tokens[4].c_str());
        }

        verticies.push_back(vertex);
    } else if (tokens[0] == "vn") {
        Eigen::Vector4f* normal = new Eigen::Vector4f(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()), 0.0f);
        normal->normalize();

        normals.push_back(normal);
    } else if (tokens[0] == "f") {
        vector<int> indicies;
        indicies = splitSlash(tokens);

        if (indicies.size() == 3) {
            Triangle* tri = new Triangle(*verticies[indicies[0] - 1], *verticies[indicies[1] - 1], *verticies[indicies[2] - 1]);
            triangles.push_back(tri);
        } else if (indicies.size() == 6) {
            Triangle* tri = new Triangle(*verticies[indicies[0] - 1], *verticies[indicies[2] - 1], *verticies[indicies[4] - 1],
                                         *normals[indicies[1] - 1], *normals[indicies[3] - 1], *normals[indicies[5] - 1]);
            triangles.push_back(tri);
        } else if (indicies.size() == 9) {
            Triangle* tri = new Triangle(*verticies[indicies[0] - 1], *verticies[indicies[3] - 1], *verticies[indicies[6] - 1],
                                         *normals[indicies[2] - 1], *normals[indicies[5] - 1], *normals[indicies[8] - 1]);
            triangles.push_back(tri);
        } else {
            throw invalid_argument("Don't know how to parse face.");
        }

    } else {
        throw invalid_argument("Unrecognized command: " + tokens[0]);
    }
}



#endif