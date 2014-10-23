#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

#include <Eigen/Dense>


#include "Transformation.h"
#include "Shape.h"
#include "Primitive.h"
#include "Color.h"
#include "Light.h"
#include "Film.h"
#include "Scene.h"
#include "Ray.h"


using namespace std;



/************ FROM SCENE CLASS ************/

Eigen::Vector4f eye(0.0f, 0.0f, 0.0f, 1.0f);
Eigen::Vector4f UL(-1.0f, 1.0f, -1.0f, 1.0f);
Eigen::Vector4f UR(1.0f, 1.0f, -1.0f, 1.0f);
Eigen::Vector4f LL(1.0f, -1.0f, -1.0f, 1.0f);
Eigen::Vector4f LR(-1.0f, -1.0f, -1.0f, 1.0f);
Eigen::Vector4f xvec; // horizontal basis vector of focal plane
Eigen::Vector4f yvec; // vertical basis vector of focal plane


vector<Primitive*> balls;
vector<PLight> plights;
vector<DLight> dlights;



Ray generateRay(float i, float j, int width, int height) {
    float focalplane = (UL + xvec * i / width + yvec * j / height)[2];
    Eigen::Vector4f pixel_loc = Eigen::Vector4f(UL[0] + xvec[0] * i / width, UL[0]+ yvec[1]*j/ height, focalplane, 1.0f);
    Eigen::Vector4f direction = pixel_loc - eye;
    direction.normalize();
    ///FIX ME
    return Ray(eye, direction, focalplane, 10000000.0f); //MAY NEED TO BE INCREASED
    //END FIX ME
}


Color trace(Ray ray, const vector<Primitive*>& balls){

    float closest_t = 999999999999999999.0f; //FIX ME MAX FLOAT
    Intersection closestInter;
    bool isPrimitiveHit = false;
    for (int i = 0; i < balls.size(); i++) {
        if (balls[i]->isHit(ray)){
            isPrimitiveHit = true;
            Intersection intersect = balls[i]->intersect(ray);
            if (intersect.local.tHit < closest_t){
                closest_t = intersect.local.tHit;
                closestInter = intersect;
            }
        }
    }

    if (!isPrimitiveHit) {
        return Color(0.0f, 0.0f, 0.0f);
    }


    // calculate color

    Color rgbSpecular(0.0f, 0.0f, 0.0f);
    Color rgbDiffuse(0.0f, 0.0f, 0.0f);


    Eigen::Vector4f surfacepoint = closestInter.local.point;

    // normal
    Eigen::Vector4f n = closestInter.local.normal;

    // view
    Eigen::Vector4f v = surfacepoint - eye;
    v.normalize();

    Material* sphereBRDF = closestInter.primitive->getBRDF();

    for (int i = 0; i < plights.size(); i++){

        // light
        Eigen::Vector4f l = plights[i].getSource() - surfacepoint;
        l.normalize();

        // reflected
        Eigen::Vector4f r = l - 2 * l.dot(n) * n;
        r.normalize();


        float scaleSpecular = powf(fmax(0.0, r.dot(v)), sphereBRDF->specularExponent);
        float scaleDiffuse = fmax(0.0, n.dot(l));

        Color lightColor = plights[i].getColor();

        rgbSpecular = rgbSpecular + lightColor * scaleSpecular;
        rgbDiffuse = rgbDiffuse + lightColor * scaleDiffuse;

    }


    rgbDiffuse = rgbDiffuse * sphereBRDF->diffuse;
    rgbSpecular = rgbSpecular * sphereBRDF->specular;

    return rgbDiffuse + rgbSpecular;
}




vector<Transformation> transforms;



void checkNumArguments(vector<string> * args, int min) {
    int n = args->size() - 1;
    if (n < min) {
        throw invalid_argument("Invalid number of arguments for command: " + (*args)[0]);
    }
}


void parseLine(string line) {
    istringstream iss(line);
    vector<string> tokens{istream_iterator<string>{iss}, istream_iterator<string>{}};

    Transformation test;

    if (tokens[0] == "cam") {
        checkNumArguments(&tokens, 15);
    } else if (tokens[0] == "sph") {
        checkNumArguments(&tokens, 4);
    } else if (tokens[0] == "tri") {
        checkNumArguments(&tokens, 9);
    } else if (tokens[0] == "obj") {
        checkNumArguments(&tokens, 1);
    } else if (tokens[0] == "ltp") {
        checkNumArguments(&tokens, 6);
        // there is an optional argument
    } else if (tokens[0] == "lta") {
        checkNumArguments(&tokens, 3);
    } else if (tokens[0] == "mat") {
        checkNumArguments(&tokens, 13);
    } else if (tokens[0] == "xft") {
        cout << "xft" << endl;
        checkNumArguments(&tokens, 3);
        transforms.push_back(Translation(stof(tokens[1]), stof(tokens[2]), stof(tokens[3])));
        test.compose(transforms);
        cout << test << endl;
    } else if (tokens[0] == "xfr") {
        cout << "xfr" << endl;
        checkNumArguments(&tokens, 3);
        transforms.push_back(Rotation(stof(tokens[1]), stof(tokens[2]), stof(tokens[3])));
        test.compose(transforms);
        cout << test << endl;
    } else if (tokens[0] == "xfs") {
        cout << "xfs" << endl;
        checkNumArguments(&tokens, 3);
        transforms.push_back(Rotation(stof(tokens[1]), stof(tokens[2]), stof(tokens[3])));
        test.compose(transforms);
        cout << test << endl;
    } else if (tokens[0] == "xfz") {
        cout << "xfz" << endl;
        checkNumArguments(&tokens, 0);
        transforms.clear();
        test.compose(transforms);
        cout << test << endl;
    } else {
        throw invalid_argument("Unrecognized command: " + tokens[0]);
    }

}

bool DEBUG = false;

int main(int argc, const char * argv[]) {

//    if(DEBUG) {
//        Transformation s = Scaling(2, 2, 2);
////        cout << s << endl;
//
//        Transformation t = Translation(3, 3, 3);
//    //    cout << t << endl;
//
//
//        Transformation r = Rotation(-90, 0, 0);
//    //    cout << r << endl;
//
//        Transformation st = s * t;
//        cout << st << endl;
//        return 0;
//    }
//
//    ifstream fin;
//    string line;
//
//    if (argc == 2) {
//        fin.open(argv[1]);
//
//        if (!fin.good()) {
//            return 1;
//        }
//
//        while (getline(fin , line)) {
//
//            try {
//                parseLine(line);
//            } catch (invalid_argument& e) {
//                cerr << e.what() << endl;
//            }
//        }
//
//        fin.close();
//    } else {
//        cout << "Invalid argument." << endl;
//    }
//
//    return 0;



    int height = 900;
    int width = 900;

    xvec = UR - UL;
    yvec = UL - LL;


    Film negative(width, height);

    Sphere test1 = Sphere(Eigen::Vector4f(0.0f, 0.0f, -2.0f, 1.0f), 1.0f);

    Material matTest1;
    matTest1.specular = Color(0.0f, 0.4f, 0.3f);
    matTest1.diffuse = Color(1.0f, 1.0f, 1.0f);
    matTest1.specularExponent = 2.0f;

    Transformation identity;
    GeometricPrimitive testSphere1(&test1, &matTest1, identity);

    balls.push_back(&testSphere1);
//
//    Sphere test2 = Sphere(Eigen::Vector4f(1.0f, 0.0f, -2.5f, 1.0f), 1.0f);
//    Material matTest2;
//    matTest2.specular = Color(0.3f, 0.5f, 0.2f);
//    matTest2.diffuse = Color(1.0f, 0.0f, 1.0f);
//    matTest2.specularExponent = 2.0f;
//
//    GeometricPrimitive testSphere2(&test2, &matTest2, identity);
//
//    balls.push_back(&testSphere2);

    PLight source(Color(200.0f, 0.0f, 155.0f), Eigen::Vector4f(2.0f, 2.0f, 2.0f, 1.0f));
    plights.push_back(source);



    for (int i = 0; i < width; i++) {

        for (int j = 0; j < height; j++) {
            Ray temp = generateRay(i, j, width, height);
            Color result = trace(temp, balls);
            negative.commit(i, j, result.getRed(), result.getGreen(), result.getBlue());
            //FIX ME: currently only supporting one sample per pixel
            //negative.commit(i, j, 255*i/width, 255*j/height, 200) activate this for pretty colors
        }
    }

    negative.writeImage();




    return 0;
}
