#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <time.h>
#include <sys/time.h>

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


vector<Primitive*> primitives;
vector<Transformation> transforms;
vector<Light*> lights;

Material currentMaterial;
ALight globalAmbient(Color(0.0f, 0.0f, 0.0f));


Ray generateRay(float i, float j, int width, int height) {
    float focalplane = (UL + xvec * i / width + yvec * j / height)[2];
    Eigen::Vector4f pixel_loc = Eigen::Vector4f(UL[0] + xvec[0] * i / width, UL[0]+ yvec[1]*j/ height, focalplane, 1.0f);
    Eigen::Vector4f direction = pixel_loc - eye;
    direction.normalize();
    ///FIX ME
    return Ray(eye, direction, focalplane, 10000000.0f); //MAY NEED TO BE INCREASED
    //END FIX ME
}


Color trace(const Ray& ray, const vector<Primitive*>& primitives){

    float closest_t = 999999999999999999.0f; //FIX ME MAX FLOAT
    Intersection closestInter;
    bool isPrimitiveHit = false;
    for (int i = 0; i < primitives.size(); i++) {
        if (primitives[i]->isHit(ray)){
            isPrimitiveHit = true;
            Intersection intersect = primitives[i]->intersect(ray);
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
    Color rgbAmbient(0.0f, 0.0f, 0.0f);


    Eigen::Vector4f surfacepoint = closestInter.local.point;

    // normal
    Eigen::Vector4f n = closestInter.local.normal;

    // view
    Eigen::Vector4f v = surfacepoint - eye;
    v.normalize();

    Material primitiveBRDF = closestInter.primitive->getBRDF();
    float specularDot = 0.0f;
    float diffuseDot = 0.0f;
    float scaleSpecular = 0.0f;

    for (int i = 0; i < lights.size(); i++){

        Color lightColor = lights[i]->getColor();

        // light
        Eigen::Vector4f l = lights[i]->getLightVector(surfacepoint);

        // reflected
        Eigen::Vector4f r = l - 2 * l.dot(n) * n;
        r.normalize();

        specularDot = r.dot(v);
        diffuseDot = n.dot(l);

        if (specularDot > 0.0f) {
            scaleSpecular = powf(specularDot, primitiveBRDF.specularExponent);
            rgbSpecular = rgbSpecular + lightColor * scaleSpecular;

        }

        if (diffuseDot > 0.0f) {
            rgbDiffuse = rgbDiffuse + lightColor * diffuseDot;
        }

        rgbAmbient = rgbAmbient + lightColor;
    }

    rgbAmbient = (rgbAmbient + globalAmbient.getColor()) * primitiveBRDF.ambient;
    rgbDiffuse = rgbDiffuse * primitiveBRDF.diffuse ;
    rgbSpecular = rgbSpecular * primitiveBRDF.specular;

    return (rgbDiffuse + rgbSpecular + rgbAmbient)* 255.0f;
}



void checkNumArguments(const vector<string> & args, int min) {
    int n = args.size() - 1;
    if (n < min) {
        throw invalid_argument("Invalid number of arguments for command: " + args[0]);
    }
}


void parseLine(const string& line) {
    istringstream iss(line);
    vector<string> tokens((istream_iterator<string>(iss)), istream_iterator<string>());

    Transformation test;

    if (tokens[0] == "cam") {
        checkNumArguments(tokens, 15);
        eye = Eigen::Vector4f(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()), 1.0f);
        LL = Eigen::Vector4f(atof(tokens[4].c_str()), atof(tokens[5].c_str()), atof(tokens[6].c_str()), 1.0f);
        LR = Eigen::Vector4f(atof(tokens[7].c_str()), atof(tokens[8].c_str()), atof(tokens[9].c_str()), 1.0f);
        UL = Eigen::Vector4f(atof(tokens[10].c_str()), atof(tokens[11].c_str()), atof(tokens[12].c_str()), 1.0f);
        UR = Eigen::Vector4f(atof(tokens[13].c_str()), atof(tokens[14].c_str()), atof(tokens[15].c_str()), 1.0f);
    } else if (tokens[0] == "sph") {
        checkNumArguments(tokens, 4);
        Transformation currentTransform;
        currentTransform.compose(transforms);

        Sphere* sphere = new Sphere(Eigen::Vector4f(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()), 1.0f), atof(tokens[4].c_str()));

        GeometricPrimitive *spherePrim = new GeometricPrimitive(sphere, currentMaterial, currentTransform);
        primitives.push_back(spherePrim);

    } else if (tokens[0] == "tri") {
        checkNumArguments(tokens, 9);
    } else if (tokens[0] == "obj") {
        checkNumArguments(tokens, 1);
    } else if (tokens[0] == "ltp") {
        checkNumArguments(tokens, 6);
        // there is an optional argument
        float falloff = 0.0f;
        if (tokens.size() - 1 < 6) {
            falloff = atof(tokens[7].c_str());
        }
        Eigen::Vector4f* source = new Eigen::Vector4f(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()), 1.0f);
        PLight* pointLight = new PLight(Color(atof(tokens[4].c_str()), atof(tokens[5].c_str()), atof(tokens[6].c_str())), *source, falloff);
        lights.push_back(pointLight);
    } else if (tokens[0] == "ltd") {
        checkNumArguments(tokens, 6);
        Eigen::Vector4f* source = new Eigen::Vector4f(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()), 1.0f);
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
        cout << "xft" << endl;
        checkNumArguments(tokens, 3);
        transforms.push_back(Translation(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str())));
        test.compose(transforms);
        cout << test << endl;
    } else if (tokens[0] == "xfr") {
        cout << "xfr" << endl;
        checkNumArguments(tokens, 3);
        transforms.push_back(Rotation(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str())));
        test.compose(transforms);
        cout << test << endl;
    } else if (tokens[0] == "xfs") {
        cout << "xfs" << endl;
        checkNumArguments(tokens, 3);
        transforms.push_back(Rotation(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str())));
        test.compose(transforms);
        cout << test << endl;
    } else if (tokens[0] == "xfz") {
        cout << "xfz" << endl;
        checkNumArguments(tokens, 0);
        transforms.clear();
        test.compose(transforms);
        cout << test << endl;
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

    ifstream fin;
    string line;

    if (argc == 2) {
        fin.open(argv[1]);

        if (!fin.good()) {
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

    cout << "Lights size: " << lights.size() << endl;
    cout << "Primitives size: " << primitives.size() << endl;

    cout << "Primitives materials: " << endl;
    for (int i = 0; i < primitives.size(); i++) {
        Material curr = primitives[i]->getBRDF();
        cout << i << ": " << curr << endl;
    }
    cout << "--------------------\n" << endl;

    cout << "Lights: " << endl;
    for (int i = 0; i < lights.size(); i++) {
        Color curr = lights[i]->getColor();
        cout << i << ": " << curr << endl;
    }
    cout << "--------------------\n" << endl;

    cout << "Eye " << eye << endl;
    cout << "UR " << UR << endl;
    cout << "UL " << UL << endl;
    cout << "LL " << LL << endl;
    cout << "LR " << LR << endl;

    int height = 500;
    int width = 500;

    xvec = UR - UL;
    yvec = UL - LL;

    Film negative(width, height);

    int pixelCount = 0;
    int fraction = width * height / 10;
    float totalPixelsScale = (float) 100.0/(width * height);

    timestamp_t t0 = get_timestamp();

    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            pixelCount++;
            if (pixelCount % fraction == 0) {
                cout << (int) (pixelCount * totalPixelsScale) << "%" << endl;
            }

            Ray temp = generateRay(i, j, width, height);
            Color result = trace(temp, primitives);
            negative.commit(i, height - j - 1, fmin(result.getRed(), 255.0f), fmin(result.getGreen(), 255.0f), fmin(result.getBlue(), 255.0f));
            //FIX ME: currently only supporting one sample per pixel
            //negative.commit(i, j, 255*i/width, 255*j/height, 200) activate this for pretty colors
        }
    }

    timestamp_t t1 = get_timestamp();
    cout << "Elapsed time: " << (t1 - t0) / 1000000.0L << endl;

    negative.writeImage();

    return 0;
}
