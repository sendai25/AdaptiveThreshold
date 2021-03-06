// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ctime>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
using namespace std;

#include <windows.h>

void print(BITMAPFILEHEADER h) {
	cout << "bfType: " << hex << h.bfType << endl;
	cout << "bfSize: " << dec << h.bfSize << endl;
	cout << "bfOffBits: " << dec << h.bfOffBits << endl;
}

void print(BITMAPINFOHEADER h) {
	cout << "biSize: " << dec << h.biSize << endl;
	cout << "biWidth: " << h.biWidth << endl;
	cout << "biHeight: " << h.biHeight << endl;
	cout << "biBitCount: " << h.biBitCount << endl;
	cout << "biCompression: " << h.biCompression << endl;
	cout << "biSizeImage: " << h.biSizeImage << endl;
	cout << "biXPelsPerMeter: " << h.biXPelsPerMeter << endl << endl;
}

class Img {
public:
	int height;
	int width;
	int rowsize;
	BYTE* term;
	RGBQUAD rgbquad[256];
	Img() {
		height = 0;
		width = 0;
	}
	Img(int h, int w) {
		height = h;
		width = w;
		rowsize = (3 * width + 3) / 4 * 4;
		term = new BYTE[height*rowsize];
	}
	bool Load(const char* filename) {
		BITMAPFILEHEADER h;
		BITMAPINFOHEADER hInfo;
		ifstream f;
		f.open(filename, ios::binary);
		f.read((char*)&h, sizeof(h));
		f.read((char*)&hInfo, sizeof(hInfo));
		//print(h);
		//print(hInfo);
		if (h.bfType != 0x4d42
			|| hInfo.biClrUsed != 0
			|| hInfo.biBitCount != 24
			|| hInfo.biCompression != 0
			|| hInfo.biPlanes != 1) {
			cout << "error when loading bmp";
			f.close();
			return 0;
		}
		width = hInfo.biWidth;
		height = hInfo.biHeight;
		*this = Img(height, width);
		f.read((char*)term, height*rowsize);
		f.close();
		return 1;
	}

	bool Loadgray(const char* filename) {
		BITMAPFILEHEADER h;
		BITMAPINFOHEADER hInfo;
		ifstream f;
		f.open(filename, ios::binary);
		f.read((char*)&h, sizeof(h));
		f.read((char*)&hInfo, sizeof(hInfo));
		//print(h);
		//print(hInfo);
		if (h.bfType != 0x4d42
			|| hInfo.biClrUsed != 0
			|| hInfo.biBitCount != 8
			|| hInfo.biCompression != 0
			|| hInfo.biPlanes != 1) {
			cout << "error when loading bmp";
			f.close();
			return 0;
		}
		width = hInfo.biWidth;
		height = hInfo.biHeight;
		f.read((char*)&rgbquad, sizeof(RGBQUAD) * 256);
		term = new BYTE[hInfo.biSizeImage];
		f.read((char*)term, hInfo.biSizeImage);
		f.close();
		return 1;
	}

	bool Save(const char* filename) {
		BITMAPFILEHEADER h = {
			0x4d42,
			54 + rowsize * height,
			0,
			0,
			54 };
		BITMAPINFOHEADER hInfo = {
			sizeof(BITMAPINFOHEADER),
			width,
			height,
			1,
			24,
			0,
			rowsize*height,
			3780,
			3780,
			0,
			0 };
		ofstream f;
		f.open(filename, ios::binary);
		f.write((char*)&h, sizeof(h));
		f.write((char*)&hInfo, sizeof(hInfo));
		f.write((char*)term, rowsize*height);
		f.close();
		return 1;
	}

	bool Save2gray(const char* filename) {
		BITMAPFILEHEADER h = {
			0x4d42,
			54 + width * height + sizeof(RGBQUAD) * 256,
			0,
			0,
			54 + sizeof(RGBQUAD) * 256 };
		BITMAPINFOHEADER hInfo = {
			sizeof(BITMAPINFOHEADER),
			width,
			height,
			1,
			8,
			BI_RGB,
			width*height,
			3780,
			3780,
			0,
			0 };
		RGBQUAD rgbquad[256];
		for (int i = 0; i < 256; i++) {
			rgbquad[i].rgbBlue = i;
			rgbquad[i].rgbGreen = i;
			rgbquad[i].rgbRed = i;
			rgbquad[i].rgbReserved = 0;
		}
		BYTE* targetbuf;
		targetbuf = new BYTE[hInfo.biSizeImage];
		ofstream f;
		f.open(filename, ios::binary);
		f.write((char*)&h, sizeof(h));
		f.write((char*)&hInfo, sizeof(hInfo));
		f.write((char*)&rgbquad, sizeof(RGBQUAD) * 256);
		int z = 0;
		for (int y = 0; y < this->height; y++)
			for (int x = 0; x < this->rowsize; x += 3) {
				BYTE B = this->term[y*this->rowsize + x];
				BYTE G = this->term[y*this->rowsize + x + 1];
				BYTE R = this->term[y*this->rowsize + x + 2];
				BYTE gray = (BYTE)((R * 19595 + G * 38469 + B * 7472) >> 16);
				targetbuf[z] = gray;
				z++;
			}
		f.write((char*)targetbuf, hInfo.biSizeImage);
		f.close();
		return 1;
	}

	bool SaveAsgray(const char* filename) {
		BITMAPFILEHEADER h = {
			0x4d42,
			54 + width * height + sizeof(RGBQUAD) * 256,
			0,
			0,
			54 + sizeof(RGBQUAD) * 256 };
		BITMAPINFOHEADER hInfo = {
			sizeof(BITMAPINFOHEADER),
			width,
			height,
			1,
			8,
			BI_RGB,
			width*height,
			3780,
			3780,
			0,
			0 };
		ofstream f;
		f.open(filename, ios::binary);
		f.write((char*)&h, sizeof(h));
		f.write((char*)&hInfo, sizeof(hInfo));
		f.write((char*)&this->rgbquad, sizeof(RGBQUAD) * 256);
		f.write((char*)this->term, hInfo.biSizeImage);
		f.close();
		return 1;
	}
};

Img convert2gray(Img m) {
	Img mm = Img(m.height, m.width);
	for (int y = 0; y < m.height; y++)
		for (int x = 0; x < m.rowsize; x += 3) {
			BYTE B = m.term[y*m.rowsize + x];
			BYTE G = m.term[y*m.rowsize + x + 1];
			BYTE R = m.term[y*m.rowsize + x + 2];
			BYTE gray = (BYTE)((R * 19595 + G * 38469 + B * 7472) >> 16);
			mm.term[y*m.rowsize + x] = mm.term[y*m.rowsize + x + 1] = mm.term[y*m.rowsize + x + 2] = gray;
		}
	return mm;
}

unsigned long* Integral(BYTE* t, int w, int h) {
	unsigned long left;
	unsigned long bottom;
	unsigned long corner;
	unsigned long* Integralterm = new unsigned long[w*h];
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (i != 0 && j != 0) {
				left = Integralterm[i*w + j - 1];
				bottom = Integralterm[(i - 1)*w + j];
				corner = Integralterm[(i - 1)*w + j - 1];
			}
			else {
				if (i == 0 && j != 0) {
					left = Integralterm[i*w + j - 1];
					bottom = 0;
					corner = 0;
				}
				else {
					if (i != 0 && j == 0) {
						left = 0;
						corner = 0;
						bottom = Integralterm[(i - 1)*w + j];
					}
					else {
						left = 0;
						bottom = 0;
						corner = 0;
					}
				}
			}
			Integralterm[i*w + j] = t[i*w + j] + left + bottom - corner;
		}
	}
	return Integralterm;
}

BYTE* AdaptiveThreshold(unsigned long* intImg, BYTE* term, int w, int h, int s, int t) {
	BYTE* biI = new BYTE[w*h];
	int x1, x2, y1, y2, tt, sum, left, bottom, corner, curr;
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++) {
			if (j - s / 2 >= 0)
				x1 = j - s / 2;
			else
				x1 = 0;
			if (j + s / 2 <= w - 1)
				x2 = j + s / 2;
			else
				x2 = w - 1;
			if (i - s / 2 >= 0)
				y1 = i - s / 2;
			else
				y1 = 0;
			if (i + s / 2 <= h - 1)
				y2 = i + s / 2;
			else
				y2 = h - 1;
			tt = (x2 - x1 + 1)*(y2 - y1 + 1);
			if (x1 != 0)
				left = intImg[y2*w + x1 - 1];
			else
				left = 0;
			if (y1 != 0)
				bottom = intImg[(y1 - 1)*w + x2];
			else
				bottom = 0;
			if (y1 != 0 && x1 != 0)
				corner = intImg[(y1 - 1)*w + x1 - 1];
			else
				corner = 0;
			curr = term[i*w + j] * tt;
			sum = intImg[y2*w + x2] - left - bottom + corner;
			if (curr * 100 <= sum * (100 - t))
				biI[i*w + j] = 0;
			else
				biI[i*w + j] = 255;
			/*            if (j == 0)
			cout << "\n";
			cout << (unsigned long)biI[i*w+j] << " ";*/
		}
	return biI;
}

int main() {
	clock_t start;
	start = clock();
	Img m, mm;
	cout << "Duration: " << (clock() - start) / (double)CLOCKS_PER_SEC << endl;
	m.Load("test.bmp");
	cout << "Duration: " << (clock() - start) / (double)CLOCKS_PER_SEC << endl;
	m.Save2gray("gray.bmp");
	cout << "Duration: " << (clock() - start) / (double)CLOCKS_PER_SEC << endl;
	mm.Loadgray("gray.bmp");
	cout << "Duration: " << (clock() - start) / (double)CLOCKS_PER_SEC << endl;
	unsigned long* Inte;
	Inte = Integral(mm.term, mm.width, mm.height);
	cout << "Duration: " << (clock() - start) / (double)CLOCKS_PER_SEC << endl;
	BYTE* binary;
	binary = AdaptiveThreshold(Inte, mm.term, mm.width, mm.height, mm.width / 16, 15);
	cout << "Duration: " << (clock() - start) / (double)CLOCKS_PER_SEC << endl;
	mm.term = binary;
	mm.SaveAsgray("binary.bmp");
	cout << "Duration: " << (clock() - start) / (double)CLOCKS_PER_SEC << endl;
}
