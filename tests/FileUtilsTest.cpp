#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include <string>

#include "pickup/utils/FileUtils.h"

using namespace pickup::utils;

class FileUtilsTest : public ::testing::Test {
 protected:
  std::string tempFile_;

  void SetUp() override {
    tempFile_ = std::tmpnam(nullptr);
  }

  void TearDown() override {
    removeFile(tempFile_);
  }

  void createTempFile(const std::string& content = "test") {
    std::ofstream ofs(tempFile_);
    ofs << content;
    ofs.close();
  }
};

TEST_F(FileUtilsTest, FileExists) {
  EXPECT_FALSE(fileExists(tempFile_));
  createTempFile();
  EXPECT_TRUE(fileExists(tempFile_));
}

TEST_F(FileUtilsTest, RemoveFile) {
  createTempFile();
  EXPECT_TRUE(fileExists(tempFile_));
  EXPECT_TRUE(removeFile(tempFile_));
  EXPECT_FALSE(fileExists(tempFile_));
}

TEST_F(FileUtilsTest, RemoveNonExistent) {
  EXPECT_FALSE(removeFile(""));
}

TEST_F(FileUtilsTest, RemoveNonExistentPath) {
  EXPECT_FALSE(removeFile("C:\\nonexistent_dir_12345\\file.txt"));
}

TEST_F(FileUtilsTest, FileExistsWithContent) {
  createTempFile("hello world");
  EXPECT_TRUE(fileExists(tempFile_));
}
