// Scene.cpp : Defines the entry point for the console application.
//

#ifndef SCENE_H
#define SCENE_H


#include "stdafx.h"
#include "lodepng.h"
#include <vector>
#include <cmath>
#include <iostream>

#include "Color.h"
#include "Light.h"
#include "Primitive.h"

#include <Eigen/Dense>
using namespace Eigen;
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


//FILM CLASS
class Film{
	public:
		Film(int width, int height);
		int width;
		int height;
		std::vector<unsigned char> image;
		void writeImage();
		void commit(int x, int y, float r, float g, float b);
};

Film::Film(int width1, int height1){
	width = width1;
	height = height1;
	image.resize(width * height * 4);
}

 void Film::writeImage()
{
	 //NOTE: this sample will overwrite the file or rayprint.png without warning!
	 const char* filename = "rayprint.png";

	//Encode the image
	unsigned error = lodepng::encode(filename, image, width, height);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}


//saves image to filename given as argument. Warning, this overwrites the file without warning!. at the moment, only one sample per picture
 void Film::commit(int x, int y, float r, float g, float b)
{
		image[4 * width * y + 4 * x + 0] = r; //RED
		image[4 * width * y + 4 * x + 1] = g; //GREEN
		image[4 * width * y + 4 * x + 2] = b; //BLUE
		image[4 * width * y + 4 * x + 3] = 255; //ALPHA for controlling transaperncy: MAINTAIN AT 255
 }

 //END FILM CLASS


 //GLOBAL VARIABLES. EYE AND FOCAL PLANE
 Vector4f eye(0.0, 0.0, 0.0, 1.0);
 Vector4f UL(-1.0, 1.0, -1.0, 1.0);
 Vector4f UR(1.0, 1.0, -1.0, 1.0);
 Vector4f LL(1.0, -1.0, -1.0, 1.0);
 Vector4f LR(-1.0, -1.0, -1.0, 1.0);
 Vector4f xvec; // horizontal basis vector of focal plane
 Vector4f yvec; // vertical basis vector of focal plane


 Ray generateRay(float i, float j){
	 float focalplane = (UL +(xvec * i / 100)+(yvec * j / 100))[2];
	 Vector4f pixel_loc = Vector4f(UL[0]+xvec[0]*i/100, UL[0]+ yvec[1]*j/100, focalplane, 1.0);
	 Vector4f direction = pixel_loc - eye;
	 direction.normalize();
	 ///FIX ME
	 Ray q(eye, direction, focalplane, 10000000.0);//MAY NEED TO BE INCREASED
	 return q;
	 //END FIX ME
 }
 //END RAY CLASS




 //Class for directional lights



 vector <Plight> plights;
 vector <Dlight> dlights;



//SCENE CLASS
 class Scene{
	public:
		Film negative;
		float height;
		float width;
		float depth;

 };
//END SCENE CLASS



vector<Primitives> balls;



 Color trace(Ray ray, vector<Sphere>  balls){

	 float closest_t = 999999999999999999; //FIX ME MAX FLOAT
     GeometricPrimitive * closest_sphere;
     bool isPrimitiveHit = false;
	 for (int i = 0; i < balls.size(); i++) {
		 GeometricPrimitive sphere = balls[i];
		 if (sphere.isHit(ray)){
             isPrimitiveHit = true;
			 float intersect = sphere.intersect(ray);
			 if (intersect < closest_t){
				 closest_t = intersect;
				 closest_sphere = balls + i;
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


     Vector4f surfacepoint = ray.source + t * r.direction;

     // normal
     Vector4f n = surfacepoint - center;
     n.normalize();

     // view
     Vector4f v = surfacepoint - eye;
     v.normalize();

     Material* sphereBRDF = closest_sphere->getBRDF();

     for (int i = 0; i < plights.size(); i++){

         // light
         Vector4f l = plights[a].source - surfacepoint;
         l.normalize();

         // reflected
         Vector4f r = -l + 2 * l.dot(n) * n;
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





 int main(int argc, char *argv[]){
	 int height = 200;
	 int width = 200;

	 xvec = UR - UL;
	 yvec = UL - LL;


	 Film negative(width, height);
	 Vector3f rgbc(1.0f, 1.0f, 1.0f);
	 Vector3f rgbd(0.0f, 0.4f, 0.3f);
	 Sphere  test = Sphere(Vector3f(0.0f, .0f, -2.0f), 1.0f, rgbc, rgbd);
	 balls.push_back(test);
	 Sphere  test2 = Sphere(Vector3f(1.0f, 0.0f, -2.5f), 1.0f, Vector3f(1.0f, 0.0f, 1.0f), Vector3f(0.3f, 0.5f, 0.2f));
	 balls.push_back(test2);

	 Plight source(Color(2.0f, 2.0f, 2.0f), 200.0f, 0.0f, 155);
	 plights.push_back(source);

	 for (int i = 0; i < width; i++){
		 for (int j = 0; j < height; j++){
			 Ray temp = generateRay(i, j);
			 Color * result = &trace(temp, balls);
			 negative.commit(i, j, result->red, result->green, result->blue);
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


	 //polymorphism dynamic late binding check: should output child1 and child2
	 Child child;
	 Child1 child1;
	 Parent * a = &child;
	 Parent * b = &child1;
	 a->doShit();
	 b->doShit();


	 float num = UL[0];
	 cout << "Dot product: " << num << endl;
	 system("PAUSE");

	 return 0;
 }







//Andrey, the following is SHIT WE DO NOT NEED. ALSO FROM LODEPNG WEBSITE. ALTERNATE WAYS OF ENCODING IMAGE

//Example 2
//Encode from raw pixels to an in-memory PNG file first, then write it to disk
//The image argument has width * height RGBA pixels or width * height * 4 bytes
void encodeTwoSteps(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height)
{
	std::vector<unsigned char> png;

	unsigned error = lodepng::encode(png, image, width, height);
	if (!error) lodepng::save_file(png, filename);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

//Example 3
//Save a PNG file to disk using a State, normally needed for more advanced usage.
//The image argument has width * height RGBA pixels or width * height * 4 bytes
void encodeWithState(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height)
{
	std::vector<unsigned char> png;
	lodepng::State state; //optionally customize this one

	unsigned error = lodepng::encode(png, image, width, height, state);
	if (!error) lodepng::save_file(png, filename);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

#endif