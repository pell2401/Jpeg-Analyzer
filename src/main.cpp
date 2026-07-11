#include <iostream>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "AppImageJpeg.h"

namespace fs = std::filesystem;
using namespace std;
using namespace fs;

int main() {
  CAppImageJpeg image;

  vector<string> file_list;

  const string path = "./images/input/";

  for (const auto &file : fs::directory_iterator(path)) {
    if (fs::is_regular_file(file)) {

      const auto &filename = file.path().filename().string();
      const auto &extension = file.path().extension().string();

      if (extension == ".jpg" || extension == ".JPG" || extension == ".jpeg" ||
          extension == ".JPEG") {
        file_list.push_back(path + filename);
      }
    }
  }

  for (const auto &file : file_list) {
    image.ReadFile(file);
    image.ParseMarker();
  }

  return (0);
}
