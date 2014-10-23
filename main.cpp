#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

#include <Eigen/Dense>

//#ifdef OSX
//#include <GLUT/glut.h>
//#include <OpenGL/glu.h>
//#else
////#include <GL/glut.h>
////#include <OpenGL/glu.h>
//#endif


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


vector<Primitive> balls;
vector<PLight> plights;
vector<DLight> dlights;



Ray generateRay(float i, float j){
    float focalplane = (UL + (xvec * i + yvec * j) / 100)[2];
    Eigen::Vector4f pixel_loc = Eigen::Vector4f(UL[0]+xvec[0]*i/100, UL[0]+ yvec[1]*j/100, focalplane, 1.0f);
    Eigen::Vector4f direction = pixel_loc - eye;
    direction.normalize();
    ///FIX ME
    return Ray(eye, direction, focalplane, 10000000.0f); //MAY NEED TO BE INCREASED
    //END FIX ME
}


Color trace(Ray ray, const vector<Primitive>& balls){

    float closest_t = 999999999999999999.0f; //FIX ME MAX FLOAT
    Intersection closestInter;
    bool isPrimitiveHit = false;
    for (int i = 0; i < balls.size(); i++) {
        if (balls[i].isHit(ray)){
            isPrimitiveHit = true;
            Intersection intersect = balls[i].intersect(ray);
            if (intersect.local.tHit < closest_t){
                closest_t = intersect.local.tHit;
                closestInter = intersect;
            }

            //return Color(red, blue, green);
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
        Eigen::Vector4f r = -l + 2 * l.dot(n) * n;
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
    vector<string> tokens{istream_iterator<string>{iss},
            istream_iterator<string>{}};

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



    int height = 200;
    int width = 200;

    xvec = UR - UL;
    yvec = UL - LL;


    Film negative(width, height);
    Color rgbs1(1.0f, 1.0f, 1.0f);
    Color rgbd1(0.0f, 0.4f, 0.3f);
    Sphere test1 = Sphere(Eigen::Vector4f(0.0f, 0.0f, -2.0f, 1.0f), 1.0f);
    Material matTest1;
    matTest1.specular = rgbs1;
    matTest1.diffuse = rgbd1;
    matTest1.specularExponent = 2.0f;

    Transformation identity;
    GeometricPrimitive testSphere1(&test1, &matTest1, identity);

    balls.push_back(testSphere1);

    Color rgbs2(1.0f, 0.0f, 1.0f);
    Color rgbd2(0.3f, 0.5f, 0.2f);
    Sphere test2 = Sphere(Eigen::Vector4f(1.0f, 0.0f, -2.5f, 1.0f), 1.0f);
    Material matTest2;
    matTest2.specular = rgbs2;
    matTest2.diffuse = rgbd2;
    matTest2.specularExponent = 2.0f;

    GeometricPrimitive testSphere2(&test2, &matTest2, identity);

    balls.push_back(testSphere2);

    PLight source(Color(2.0f, 2.0f, 2.0f), Eigen::Vector4f(200.0f, 0.0f, 155.0f, 1.0f));
    plights.push_back(source);

    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            Ray temp = generateRay(i, j);
            Color result = trace(temp, balls);
            negative.commit(i, j, result.getRed(), result.getGreen(), result.getBlue());
            //FIX ME: currently only supporting one sample per pixel
            //negative.commit(i, j, 255*i/width, 255*j/height, 200) activate this for pretty colors
        }
    }

    negative.writeImage();

    //testing random shit
    /*
    Vector3f v(1.0f, 2.0f, 3.0f);
    Vector3f w(0.0f, 1.0f, 2.0f);
    int dotty = v.dot(w);
    Vector3f sub = v - w;
    */
    // Sphere test(Vector3f(5.0f, 5.0f, 5.0f), 5.0f);
    // Ray trial(Vector3f(5.0f+3.0f,5.0f+4.1f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), 1.0f, 300.0f);
    //int hh = test.hit(trial);
    // cout << "Dot product: " << hh << endl;




    float num = UL[0];
    cout << "Dot product: " << num << endl;
    system("PAUSE");

    return 0;
}
