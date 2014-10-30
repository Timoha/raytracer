#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <vector>
#include <time.h>
#include <limits>
#include <sys/time.h>

#include <Eigen/Dense>
#include <Eigen/StdVector>

#include "Transformation.h"
#include "Shape.h"
#include "Primitive.h"
#include "Color.h"
#include "Light.h"
#include "Film.h"
#include "Scene.h"
#include "Ray.h"
#include "ObjParser.h"
#include "AABBNode.h"


using namespace std;

/************ FROM SCENE CLASS ************/

Eigen::Vector4d eye(0.0, 0.0, 0.0, 1.0);
Eigen::Vector4d UL(-1.0, 1.0, -1.0, 1.0);
Eigen::Vector4d UR(1.0, 1.0, -1.0, 1.0);
Eigen::Vector4d LL(1.0, -1.0, -1.0, 1.0);
Eigen::Vector4d LR(-1.0, -1.0, -1.0, 1.0);
Eigen::Vector4d xvec; // horizontal basis vector of focal plane
Eigen::Vector4d yvec; // vertical basis vector of focal plane
int height = 1000;
int width = 1000;
int depth = 10;

AABBNode rootAABB;


vector<Primitive*> primitives;
vector<Transformation*> transforms;
vector<Light*> lights;

Material currentMaterial;
ALight globalAmbient(Color(0.0, 0.0, 0.0));


Ray generateRay(double scaleWidth, double scaleHeight) {
    double focalplane = (UL + xvec * scaleWidth + yvec * scaleHeight)[2];
    Eigen::Vector4d pixel_loc = Eigen::Vector4d(UL[0] + xvec[0] * scaleWidth, UL[0]+ yvec[1] * scaleHeight, focalplane, 1.0);
    Eigen::Vector4d direction = pixel_loc - eye;
    direction.normalize();
    return Ray(eye, direction, focalplane, numeric_limits<double>::infinity());
}


Color trace(const Ray& ray, const vector<Primitive*>& primitives, int depth){

    if (depth == 0) {
        return Color(0.0, 0.0, 0.0);
    }

    double closest_t = numeric_limits<double>::infinity();
    Intersection closestInter, intersect;
    bool isPrimitiveHit = false;

    vector<const Primitive*> relevant = rootAABB.getRelevantPrimitives(ray);
    for (int i = 0; i < relevant.size(); i++) {
        intersect = relevant[i]->intersect(ray);
        if (intersect.local.isHit && intersect.local.tHit < closest_t){
            isPrimitiveHit = true;
            closest_t = intersect.local.tHit;
            closestInter = intersect;
        }
    }

    if (!isPrimitiveHit) {
        return Color(0.0, 0.0, 0.0);
    }


    // calculate color
    Color rgbSpecular(0.0, 0.0, 0.0);
    Color rgbDiffuse(0.0, 0.0, 0.0);
    Color rgbAmbient(0.0, 0.0, 0.0);

    Eigen::Vector4d surfacepoint = closestInter.local.point;

    // normal
    Eigen::Vector4d n = closestInter.local.normal;

    // view
    Eigen::Vector4d v = surfacepoint - ray.source;
    v.normalize();

    Material primitiveBRDF = closestInter.primitive->getBRDF();
    double specularDot = 0.0;
    double diffuseDot = 0.0;
    double scaleSpecular = 0.0;

    for (int i = 0; i < lights.size(); i++){

        // light
        Eigen::Vector4d l = lights[i]->getLightVector(surfacepoint);

        Color lightColor = lights[i]->getColor();
        rgbAmbient = rgbAmbient + lightColor;

        // shadows
        bool isShadowHit = false;
        Ray shadowRay(surfacepoint, l, 0.0, numeric_limits<double>::infinity());

        vector<const Primitive*> relevantShadow = rootAABB.getRelevantPrimitives(shadowRay);
        for (int x = 0; x < relevantShadow.size(); x++) {
            intersect = relevantShadow[x]->intersect(shadowRay);
            if (intersect.local.isHit && closestInter.primitive != intersect.primitive){
                isShadowHit = true;
                break;
            }
        }

        if (isShadowHit) {
            continue;
        }

        // reflected
        Eigen::Vector4d r = l - 2 * l.dot(n) * n;
        r.normalize();

        specularDot = r.dot(v);
        diffuseDot = n.dot(l);

        if (specularDot > 0.0) {
            scaleSpecular = powf(specularDot, primitiveBRDF.specularExponent);
            rgbSpecular = rgbSpecular + lightColor * scaleSpecular;
        }

        if (diffuseDot > 0.0) {
            rgbDiffuse = rgbDiffuse + lightColor * diffuseDot;
        }
    }

    rgbAmbient = (rgbAmbient + globalAmbient.getColor()) * primitiveBRDF.ambient;
    rgbDiffuse = rgbDiffuse * primitiveBRDF.diffuse;
    rgbSpecular = rgbSpecular * primitiveBRDF.specular;

    Eigen::Vector4d rd = v - 2 * n.dot(v) * n;
    rd.normalize();

    Ray reflect(surfacepoint, rd, 0.0, numeric_limits<double>::infinity());

    return rgbDiffuse + rgbSpecular + rgbAmbient + primitiveBRDF.reflective * trace(reflect, primitives, depth - 1);
}



void checkNumArguments(const vector<string>& args, int min) {
    int n = args.size() - 1;
    if (n < min) {
        throw invalid_argument("Invalid number of arguments for command: " + args[0]);
    }
}


void parseLine(const string& line) {
    istringstream iss(line);
    vector<string> tokens((istream_iterator<string>(iss)), istream_iterator<string>());

    if (tokens.size() == 0 || tokens[0] == "#") {
        return;
    }

    if (tokens[0] == "cam") {
        checkNumArguments(tokens, 15);
        eye = Eigen::Vector4d(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()), 1.0);
        LL = Eigen::Vector4d(atof(tokens[4].c_str()), atof(tokens[5].c_str()), atof(tokens[6].c_str()), 1.0);
        LR = Eigen::Vector4d(atof(tokens[7].c_str()), atof(tokens[8].c_str()), atof(tokens[9].c_str()), 1.0);
        UL = Eigen::Vector4d(atof(tokens[10].c_str()), atof(tokens[11].c_str()), atof(tokens[12].c_str()), 1.0);
        UR = Eigen::Vector4d(atof(tokens[13].c_str()), atof(tokens[14].c_str()), atof(tokens[15].c_str()), 1.0);
    } else if (tokens[0] == "sph") {
        checkNumArguments(tokens, 4);
        Transformation* currentTransform = new Transformation();
        currentTransform = currentTransform->compose(transforms);

        Sphere* sphere = new Sphere(Eigen::Vector4d(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()), 1.0), atof(tokens[4].c_str()));

        primitives.push_back(new GeometricPrimitive(sphere, currentMaterial, currentTransform));

    } else if (tokens[0] == "tri") {
        checkNumArguments(tokens, 9);
        Transformation* currentTransform = new Transformation();
        currentTransform = currentTransform->compose(transforms);
        Eigen::Vector3d a(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
        Eigen::Vector3d b(atof(tokens[4].c_str()), atof(tokens[5].c_str()), atof(tokens[6].c_str()));
        Eigen::Vector3d c(atof(tokens[7].c_str()), atof(tokens[8].c_str()), atof(tokens[9].c_str()));

        Triangle* triangle = new Triangle(a, b, c);

        primitives.push_back(new GeometricPrimitive(triangle, currentMaterial, currentTransform));

    } else if (tokens[0] == "obj") {
        checkNumArguments(tokens, 1);
        Transformation* currentTransform = new Transformation();
        currentTransform = currentTransform->compose(transforms);

        ObjParser object(tokens[1]);

        vector<Triangle*> ts = object.getTriangles();

//        vector<Primitive*>* aggregate = new vector<Primitive*>;

        for (int i = 0; i < ts.size(); i++) {
            primitives.push_back(new GeometricPrimitive(ts[i], currentMaterial, currentTransform));
        }

//        primitives.push_back(new AggregatePrimitive(*aggregate));

    } else if (tokens[0] == "ltp") {
        checkNumArguments(tokens, 6);
        // there is an optional argument
        double falloff = 0.0;
        if (tokens.size() - 1 < 6) {
            falloff = atof(tokens[7].c_str());
        }
        Eigen::Vector4d* source = new Eigen::Vector4d(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()), 1.0);
        PLight* pointLight = new PLight(Color(atof(tokens[4].c_str()), atof(tokens[5].c_str()), atof(tokens[6].c_str())), *source, falloff);
        lights.push_back(pointLight);
    } else if (tokens[0] == "ltd") {
        checkNumArguments(tokens, 6);
        Eigen::Vector4d* source = new Eigen::Vector4d(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()), 0.0);
        DLight* dirLight = new DLight(Color(atof(tokens[4].c_str()), atof(tokens[5].c_str()), atof(tokens[6].c_str())), *source);
        lights.push_back(dirLight);
    } else if (tokens[0] == "lta") {
        checkNumArguments(tokens, 3);
        globalAmbient = ALight(globalAmbient.getColor() + Color(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str())));
    } else if (tokens[0] == "mat") {
        checkNumArguments(tokens, 13);
        currentMaterial.ambient = Color(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
        currentMaterial.diffuse = Color(atof(tokens[4].c_str()), atof(tokens[5].c_str()), atof(tokens[6].c_str()));
        currentMaterial.specular = Color(atof(tokens[7].c_str()), atof(tokens[8].c_str()), atof(tokens[9].c_str()));
        currentMaterial.specularExponent = atof(tokens[10].c_str());
        currentMaterial.reflective = Color(atof(tokens[11].c_str()), atof(tokens[12].c_str()), atof(tokens[13].c_str()));
    } else if (tokens[0] == "xft") {
        checkNumArguments(tokens, 3);
        transforms.push_back(new Translation(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str())));
    } else if (tokens[0] == "xfr") {
        checkNumArguments(tokens, 3);
        transforms.push_back(new Rotation(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str())));
    } else if (tokens[0] == "xfs") {
        checkNumArguments(tokens, 3);
        transforms.push_back(new Scaling(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str())));
    } else if (tokens[0] == "xfz") {
        checkNumArguments(tokens, 0);
        transforms.clear();
    } else {
        throw invalid_argument("Unrecognized command: " + tokens[0]);
    }
}


typedef unsigned long long timestamp_t;

static timestamp_t
get_timestamp ()
{
    struct timeval now;
    gettimeofday (&now, NULL);
    return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}



bool DEBUG = false;

int main(int argc, const char * argv[]) {

    if(DEBUG) {
        Transformation s = Scaling(2, 2, 2);
//        cout << s << endl;

        Transformation t = Translation(3, 3, 3);
    //    cout << t << endl;


        Transformation r = Rotation(-90, 0, 0);
    //    cout << r << endl;

        Transformation st = s * t;
        cout << st << endl;
        return 0;
    }


    cout << "Setting up scene..." << endl;
    ifstream fin;
    string line;

    if (argc >= 2) {
        fin.open(argv[1]);

        if (argc == 5) {
            width = atoi(argv[2]);
            height = atoi(argv[3]);
            depth = atoi(argv[4]);
        }


        if (!fin.good()) {
            cout << "File \"" << argv[1] << "\" does not exists" << endl;
            return 1;
        }

        while (getline(fin , line)) {

            try {
                parseLine(line);
            } catch (invalid_argument& e) {
                cerr << e.what() << endl;
            }
        }

        fin.close();
    } else {
        cout << "Invalid argument." << endl;
    }

    cout << "Constructing acceleration structure..." << endl;

    // acceleration
    rootAABB.constructTree(primitives);


    xvec = UR - UL;
    yvec = UL - LL;


    Film negative(width, height);

    int pixelCount = 0;
    int fraction = width * height / 10;
    double totalPixelsScale = 100.0 / (width * height);


    cout << "Raytracing..." << endl;
    timestamp_t t0 = get_timestamp();

    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            pixelCount++;
            if (pixelCount % fraction == 0) {
                cout << (int) (pixelCount * totalPixelsScale) << "%" << endl;
            }

            Ray temp = generateRay((double) i / width, (double) j / height);
            Color result = trace(temp, primitives, depth) * 255.0;
            negative.commit(i, height - j - 1, fmin(result.getRed(), 255.0), fmin(result.getGreen(), 255.0), fmin(result.getBlue(), 255.0));
            //FIX ME: currently only supporting one sample per pixel
            //negative.commit(i, j, 255*i/width, 255*j/height, 200) activate this for pretty colors
        }
    }

    timestamp_t t1 = get_timestamp();
    cout << "Elapsed time: " << (t1 - t0) / 1000000.0L << endl;

    cout << "Saving image..." << endl;
    negative.writeImage();

    return 0;
}
