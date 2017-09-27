#include "opencv2\opencv.hpp"
#include <string>
#include <ctime>

using namespace cv;
using namespace std;

Mat writeTextToImage(Mat, string);
string readTextFromImage(Mat);
void saveImage(Mat, string);
void printPixels(Mat, int);
int changeLastChar(int, int);

int main(int argc, char** argv) {
	// ***args***
	bool encrypt = false;	// debug

	if (encrypt) {
		// encrypt
		argc = 7;
		argv[1] = "img1.png";
		argv[2] = "secret";
		argv[3] = "stuff";
		argv[4] = "vs1";
		argv[5] = "derp";
		argv[6] = "420";
	}else {
		// decrypt
		argc = 2;
		argv[1] = "e_img1.png";
	}
	// **********

	// check arguments
	if (argc == 1) {
		cerr << "Usage: imagename.png textToEncrypt\n";
		return -1;
	}

	String imageName = argv[1];

	Mat image = imread(imageName);
	String message = "";

	// get message to encrypt
	if (argc > 2) {
		for (int i = 2; i < argc; i++) {
			message += argv[i];
			message += " ";
		}
	}

	// check image
	if (!image.data) {
		cerr << "Could not get image data\n";
		return -1;
	} else if (message.length() + 3 > (image.rows * image.cols)) {
		cerr << "Too small image for that message\n";
		return -1;
	}

	if (argc == 2) {
		// decrypt
		clock_t begin = clock();
		string text = readTextFromImage(image);
		double decryptTime = double(clock() - begin);
		cout << "\nDecrypted message (" << decryptTime << "ms): \n" << text << "\n";
	} else {
		// encrypt
		clock_t begin = clock();
		image = writeTextToImage(image, message);
		double encryptTime = double(clock() - begin);
		string newImageName = "e_" + imageName;			// maybe change this later
		saveImage(image, newImageName);
		cout << "\nEncrypted message (" << encryptTime << "ms) to " << newImageName << "\n";
	}

	imshow("Modified", image);	// debug
	waitKey();

	return 0;
}

Mat writeTextToImage(Mat image, string message) {
	// change all pixels RGB values to end with 0
	for (int r = 0; r < image.rows; r++) {
		for (int c = 0; c < image.cols; c++) {
			image.at<Vec3b>(r, c)[2] = changeLastChar(image.at<Vec3b>(r, c)[2], 0);	// red
			image.at<Vec3b>(r, c)[1] = changeLastChar(image.at<Vec3b>(r, c)[1], 0);	// green
			image.at<Vec3b>(r, c)[0] = changeLastChar(image.at<Vec3b>(r, c)[0], 0);	// blue
		}
	}

	// write text to pixels by changing the last decimal in every RGB value
	int ascii;
	int digit;
	int row;
	int col;
	int count = 3;	// skip 3 first pixels
	for (char ch : message) {
		row = count / image.cols;
		col = count % image.cols;
		ascii = ch;
		count++;

		//cout << "char: " << ch << " ascii: " << ascii << "\n";

		image.at<Vec3b>(row, col)[0] = changeLastChar(image.at<Vec3b>(row, col)[0], ascii % 10);	// blue
		//cout << "to blue: " << ascii % 10 << " : ";

		ascii /= 10;
		image.at<Vec3b>(row, col)[1] = changeLastChar(image.at<Vec3b>(row, col)[1], ascii % 10);	// green
		//cout << "to green: " << ascii % 10 << " : ";

		ascii /= 10;
		image.at<Vec3b>(row, col)[2] = changeLastChar(image.at<Vec3b>(row, col)[2], ascii % 10);	// red
		//cout << "to red: " << ascii % 10 << "\n";
	}

	//printPixels(image, 30);

	return image;
}

string readTextFromImage(Mat image) {
	string result = "";
	int red;
	int green;
	int blue;
	int ascii;
	char ch;

	//printPixels(image, 30);	// test

	// check so that first 3 pixels end with 0
	for (int testCol = 0; testCol < 3; testCol++) {
		red = (image.at<Vec3b>(0, testCol)[2] + 1 - 1) % 10;
		green = (image.at<Vec3b>(0, testCol)[1] + 1 - 1) % 10;
		blue = (image.at<Vec3b>(0, testCol)[0] + 1 - 1) % 10;
		if (red != 0 || green != 0 || blue != 0) {
			cout << "\nNo encrypted message found\n";
			return "";
		}
	}
	

	for (int r = 0; r < image.rows; r++) {
		for (int c = 0; c < image.cols; c++) {
			if (r == 0 && c < 3) continue;	// skip 3 first

			red = (image.at<Vec3b>(r, c)[2] + 1 - 1) % 10;
			green = (image.at<Vec3b>(r, c)[1] + 1 - 1) % 10;
			blue = (image.at<Vec3b>(r, c)[0] + 1 - 1) % 10;

			// 0.0.0 = no more data to read
			if (red == 0 && green == 0 && blue == 0) return result;

			ascii = red * 100 + green * 10 + blue;
			ch = ascii;
			result += ch;
		}
	}

	return result;
}

void saveImage(Mat image, string imgName) {
	imwrite(imgName, image);
}

void printPixels(Mat image, int pixelsToPrint) {
	// get RGB value of some pixels. for debugging.
	int readPixels = 0;
	for (int r = 0; r < image.rows; r++) {
		for (int c = 0; c < image.cols; c++) {
			if (readPixels++ < pixelsToPrint) {
				cout << image.at<Vec3b>(r, c)[2] + 1 - 1 << ","		//red
					<< image.at<Vec3b>(r, c)[1] + 1 - 1 << ","		//green
					<< image.at<Vec3b>(r, c)[0] + 1 - 1 << "\n";	//blue
			}
		}
	}
}

int changeLastChar(int value, int newEnd) {
	// change last decimal of an integer
	if (newEnd > 9) return 1;
	value /= 10;
	value *= 10;
	value += newEnd;
	return value;
}
