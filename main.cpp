#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "AppImageJpeg.h"

namespace fs = std::filesystem;
using namespace fs;

int main()
{
	CAppImageJpeg image;

	image.ReadFile("./images/article1.jpg");
	image.ParseMarker();


	return (0);

}