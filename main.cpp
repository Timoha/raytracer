#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

#include <Eigen/Dense>

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
//#include <GL/glut.h>
//#include <OpenGL/glu.h>
#endif


#include "Transformation.h"
#include "Shape.h"
#include "Primitive.h"


using namespace std;



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

    return 0;
}
