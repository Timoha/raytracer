#ifndef FILM_H
#define FILM_H

#include "lodepng.h"

#include <vector>
#include <iostream>

using namespace std;


//CODE FOR WRITING TO AN IMAGE FILE (LODEPNG LIBRARY),
//CITATION: code relating to the writeimage and commit methods is ADAPTED/BORROWED/COPIED FROM THE EXAMPLE FILE ON THE LODEPNG WEBSITE.
//I DO NOT ASSUME 100% CREDIT FOR THAT CODE. ALL DUE CREDIT IS GIVEN TO THE MAKERS OF THE LIBRARY.

//"g++ lodepng.cpp example_encode.cpp -ansi -pedantic -Wall -Wextra -O3"

//From lodepng example: "Encode from raw pixels to disk with a single function call"
//"The image argument has width * height RGBA pixels or width * height * 4 bytes"

//CITATION: code for constructing, adding, multiplying, transforming etc. vectors is powered by the Eigen library. All due credits is given to the makers
//of this library

//ALL OTHER CODE WRITTEN BY SHIV SUNDRAM AND ANDREY ELENSKIY



class Film {

private:
    int width, height;
    vector<unsigned char> image;
public:
    Film(int widthIn, int heightIn);
    void writeImage();
    void commit(int x, int y, float r, float g, float b);
};

Film::Film(int widthIn, int heightIn){
    width = widthIn;
    height = heightIn;
    image.resize(width * height * 4);
}

void Film::writeImage() {
    //NOTE: this sample will overwrite the file or rayprint.png without warning!
    const char* filename = "rayprint.png";

    //Encode the image
    unsigned error = lodepng::encode(filename, image, width, height);

    //if there's an error, display it
    if (error) cout << "encoder error " << error << ": " << lodepng_error_text(error) << endl;
}


//saves image to filename given as argument. Warning, this overwrites the file without warning!. at the moment, only one sample per picture
void Film::commit(int x, int y, float r, float g, float b) {
    image[4 * width * y + 4 * x + 0] = r; //RED
    image[4 * width * y + 4 * x + 1] = g; //GREEN
    image[4 * width * y + 4 * x + 2] = b; //BLUE
    image[4 * width * y + 4 * x + 3] = 255; //ALPHA for controlling transaperncy: MAINTAIN AT 255
}

#endif