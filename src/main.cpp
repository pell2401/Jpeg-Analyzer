#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <filesystem>
#include <fstream>

#include "AppImageJpeg.h"

namespace fs = std::filesystem;
using namespace std;
using namespace fs;

int main()
{
  CAppImageJpeg image;

  vector<string> file_list;

  const string path = "./images/input/";

  if (!exists(path)) {
    create_directories(path);
  }

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

  uint64_t filesize = 0x00;

  for (const auto &file : file_list) {
    image.ReadFile(file);
    image.ParseMarker();
    filesize += image.GetFileSize();
  }

  cout << "Total filesize: " << hex << filesize / 1024.0 / 1024.0 << "[MB]"
       << endl;

  return (0);
}
