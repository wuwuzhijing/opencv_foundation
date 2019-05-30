// cv_proc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "cv_proc.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <limits>
#include <bitset>
#include <string>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <QDir>
#include<Windows.h>
#include <String>
#include<iomanip>
using namespace std;

using byte = unsigned char;
constexpr std::size_t BITS_PER_BYTE = std::numeric_limits<byte>::digits;
using bits_in_byte = std::bitset<BITS_PER_BYTE>;

void decodeBmp(char* filename) {
	errno_t err;
	FILE* fp;
	if (err = fopen_s(&fp, "D:/1.bmp", "r"))
	{
		std::cout << "fail to open." << std::endl;
		exit(-1);
	}
	BITMAPFILEHEADER bmpFileHeader;
	fread(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	BITMAPINFOHEADER bmpInfoHeader;
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	std::cout << std::setiosflags(std::ios::left);   //设置输出格式为左对齐	//B,M的ASCII码的16进制形式分别为0x42，0x4d，0x4d42的值为19778，当bfType为19778时，就说明是bmp图片	
	std::cout << std::setw(40) << "bfType：" << std::setw(20) << bmpFileHeader.bfType << std::endl;
	std::cout << std::setw(40) << "图片大小为：" << bmpFileHeader.bfSize << "Byte，即" << bmpFileHeader.bfSize / 1024 << "kB" << std::endl;
	std::cout << std::setw(40) << "文件头位置到图像数据位置的偏移量为：" << bmpFileHeader.bfOffBits << "Byte" << std::endl;
	std::cout << std::setw(40) << "bmp信息头大小为：" << bmpInfoHeader.biSize << "Byte" << std::endl;
	std::cout << std::setw(40) << "图片宽度为：" << bmpInfoHeader.biWidth << "pixel" << std::endl;
	std::cout << std::setw(40) << "图片高度为：" << bmpInfoHeader.biHeight << "pixel" << std::endl;
	std::cout << std::setw(40) << "每个像素比特数为：" << bmpInfoHeader.biBitCount << "bit，即每个像素占" << bmpInfoHeader.biBitCount / 8 << "Byte" << std::endl;
	switch (bmpInfoHeader.biCompression)
	{
	case 0:
		std::cout << std::setw(40) << "图像数据压缩类型为：" << "不压缩" << std::endl;
		break;
	case 1:
		cout << setw(40) << "图像数据压缩类型为：" << "8比特游程编码，只适用于8位位图" << endl;
		break;
	case 2:
		cout << setw(40) << "图像数据压缩类型为：" << "4比特游程编码，只适用于4位位图" << endl;
		break;
	case 3:
		cout << setw(40) << "图像数据压缩类型为：" << "比特域，用于16/32位位图" << endl;
		break;
	case 4:
		cout << setw(40) << "图像数据压缩类型为：" << "位图含JPEG图像（仅用于打印机）" << endl;
		break;
	case 5:
		cout << setw(40) << "图像数据压缩类型为：" << "位图含PNG图像（仅用于打印机）" << endl;
		break;
	}

	switch (bmpInfoHeader.biClrUsed)
	{
	case 0:
		cout << "使用了所有的颜色索引，不需要调色板" << endl;
		break;
	default:
		cout << setw(40) << "位图实际使用的彩色表中的颜色索引数：" << bmpInfoHeader.biClrUsed << endl;
		break;
	}

	switch (bmpInfoHeader.biClrImportant)
	{
	case 0:
		cout << setw(40) << "对图像显示有重要影响的颜色索引数：" << "都重要" << endl;
		break;

	default:
		cout << setw(40) << "对图像显示有重要影响的颜色索引数：" << bmpInfoHeader.biClrImportant << endl;
		break;
	}

	fclose(fp);
	return;
}


string BinToHex(const string &strBin, bool bIsUpper = false)
{
	string strHex;
	strHex.resize(strBin.size() * 2);
	for (size_t i = 0; i < strBin.size(); i++)
	{
		uint8_t cTemp = strBin[i];
		for (size_t j = 0; j < 2; j++)
		{
			uint8_t cCur = (cTemp & 0x0f);
			if (cCur < 10)
			{
				cCur += '0';
			}
			else
			{
				cCur += ((bIsUpper ? 'A' : 'a') - 10);
			}
			strHex[2 * i + 1 - j] = cCur;
			cTemp >>= 4;
		}
	}

	return strHex;
}

std::string read_bits(const char* path_to_file){
	std::string bitstring;
	std::ifstream file(path_to_file, std::ios::binary); // open in binary mode

	char c;
	while (file.get(c)) // read byte by byte
		bitstring += bits_in_byte(byte(c)).to_string(); // append as string of '0' '1'

	return bitstring;
}

int mask[] = {4096,2048,1024,512,256,128,64,32,16,8,4,2,1};
int last_iterator = sizeof(mask) / 4 - 1;

int __bit(const char *array, int strlen, int *mask, int last_iterator)
{
	int temp = 0;
	for (int i = strlen - 1; i >= 0; i--) {
		temp += (1 & (array[i] - '0')) * std::pow(2, strlen - 1 - i);
	}		
		//temp += (1 & (array[i] - '0'))*mask[last_iterator--];
	return temp;
}

void write_bits(std::string bitstring, const char* path_to_file)
{
	// pad with zeroes to make it represent an integral multiple of bytes
	while (bitstring.size() % BITS_PER_BYTE) 
		bitstring += '0';

	std::ofstream file(path_to_file, std::ios::binary); // open in binary mode
	std::size_t i = 0;
	file << "file format: ";
	char format;
	std::string fileSize;
	int  size = 0;
	//bitstring = BinToHex(bitstring);
	//cout << bitstring;
	int index = 0;
	int offset = 0;
	for (; i < bitstring.size(); i += BITS_PER_BYTE)
	{
		int j = 0, k = 0;
		//convert each sequence of '0' or '1' to a byte
		//byte b = bits_in_byte(bitstring.substr(i, BITS_PER_BYTE)).to_ullong();
		//file << b; // and write it
		//file << bitstring.substr(i, 8);
		int value = 0;
		std::string tmp, tmp2;
		tmp = bitstring.substr(i, BITS_PER_BYTE);
		if (i == 2 * BITS_PER_BYTE || i == 10 * BITS_PER_BYTE) {
			tmp = bitstring.substr(i, BITS_PER_BYTE * 4);
			i += 3 * BITS_PER_BYTE;
		}

		//if (i < 14 * BITS_PER_BYTE)
		//	cout << __LINE__ << ": " << tmp << endl;

		if (i == 5 * BITS_PER_BYTE || i == 13 * BITS_PER_BYTE) {
			string byte1, byte2, byte3, byte4;
			byte1 = tmp.substr(0, BITS_PER_BYTE);
			byte2 = tmp.substr(BITS_PER_BYTE, BITS_PER_BYTE);
			byte3 = tmp.substr(BITS_PER_BYTE * 2, BITS_PER_BYTE);
			byte4 = tmp.substr(BITS_PER_BYTE * 3, BITS_PER_BYTE);
			tmp = byte4 + byte3 + byte2 + byte1;
			//cout << __LINE__ << ": " << tmp << endl;
		}

		value = __bit(tmp.c_str(), strlen(tmp.c_str()), mask, last_iterator);
	
		//_itoa_s(value, err_str, 10);
		
		if (i < 2 * BITS_PER_BYTE) {
			format = toascii(value);
			file << format << " ";
		}
		   
		if (i == 5 * BITS_PER_BYTE) {
			//cout << tmp << endl;		
			file << endl << "file size: ";
			file << value << " Byte" << endl;
		}

		if (i == 13 * BITS_PER_BYTE) {
			//cout << tmp << endl;
			offset = value;
			file << endl << "start address: ";
			file << value << " Byte" << endl;
		}
		if (i >= offset) {
			file << value << " ";
			index++;
			if (index == 1296) {
				file << endl;
			}
		}

	}
	//std::cout << i;
}

void deleteDir(const QString &d)
{
	QDir dir(d);
	foreach(QFileInfo inf, dir.entryInfoList(QDir::Dirs | QDir::Files)){
		if (inf.isFile())
			dir.remove(inf.absoluteFilePath());
		else if (inf.isDir() &&
			inf.fileName() != "." &&
			inf.fileName() != "..")
			deleteDir(inf.absolutePath() + "/" + inf.fileName());
	}
	dir.rmdir(dir.absolutePath());
}

int _tmain(int argc, _TCHAR* argv[])
{
	char *filename = R"(D:\1.bmp)";
	if(1)
		decodeBmp(filename);	

	QString d = "D:/2/";
	deleteDir(d);

	//1. load/copy/write an image 
	////1.1 load an image into memory/stack
	////////1.1.1 to Mat(memory)
	cv::Mat src = cv::imread(filename, cv::IMREAD_UNCHANGED);
	////////1.1.2 to binary(memory)
	if (1){
		std::string bits = read_bits(filename);
		//std::cout << bits << '\n';

		// flip every hundredth bit
		for (std::size_t i = 0; i < bits.size(); i += 100)
		{
			if (bits[i] == 0) bits[i] = '1';
			else bits[i] = '0';
		}

		bits += "011001010110111001100100001"; // append another 27 bits
		write_bits(bits, "D:/1.txt");
	}

	//std::cout << std::ifstream("main.cc.mangled").rdbuf();

	//2. copy
	cv::Mat srcCopy;
	src.copyTo(srcCopy);

	if (src.rows == 0) {
		std::cout << "Image not exist!" << std::endl;
	}

	cv::Mat dst1, dst2, dst3, dst4, dst5, dst6, dst7;
	cv::threshold(src, dst1, 225, 255, cv::THRESH_BINARY);
	//cv::threshold(src, dst2, 50, 255, cv::THRESH_BINARY_INV);
	cv::threshold(dst1, dst3, 180, 255, cv::THRESH_TOZERO);
	cv::threshold(src, dst4, 180, 255, cv::THRESH_TRIANGLE);
	cv::threshold(src, dst5, 100, 0, cv::THRESH_TRUNC);
	cv::threshold(src, dst6, 100, 0, cv::THRESH_OTSU);
	cv::threshold(src, dst7, 100, 0, cv::THRESH_MASK);

	return 0;
}

