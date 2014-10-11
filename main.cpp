#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>


#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
//#include <GL/glut.h>
//#include <OpenGL/glu.h>
#endif


#include "Transformation.h"



using namespace std;

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
        checkNumArguments(&tokens, 3);
    } else if (tokens[0] == "xfr") {
        checkNumArguments(&tokens, 3);
    } else if (tokens[0] == "xfs") {
        checkNumArguments(&tokens, 3);
    } else if (tokens[0] == "xfz") {
        checkNumArguments(&tokens, 0);
    } else {
        throw invalid_argument("Unrecognized command: " + tokens[0]);
    }

}

int main(int argc, const char * argv[]) {

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


//    Transformation s = Scaling(2, 2, 2);
////    cout << s << endl;
//
//    Transformation t = Translation(3, 3, 3);
////    cout << t << endl;
//
//
//    Transformation r = Rotation(-90, 0, 0);
////    cout << r << endl;
//
//
//
//
//    Transformation st = s * t;
//    cout << st << endl;

    return 0;
}
