#include <gtest/gtest.h>

#include "pickup/utils/DynamicLibrary.h"

using namespace pickup::utils;

TEST(DynamicLibraryTest, DefaultState) {
  DynamicLibrary lib;
  EXPECT_FALSE(lib.isLoaded());
  EXPECT_TRUE(lib.path().empty());
  EXPECT_TRUE(lib.lastError().empty());
}

TEST(DynamicLibraryTest, LoadNonexistentFile) {
  DynamicLibrary lib;
  EXPECT_FALSE(lib.load("C:\\nonexistent_library_12345.dll"));
  EXPECT_FALSE(lib.isLoaded());
  EXPECT_FALSE(lib.lastError().empty());
}

TEST(DynamicLibraryTest, GetSymbolOnUnloaded) {
  DynamicLibrary lib;
  EXPECT_EQ(lib.getSymbol("someFunction"), nullptr);
  EXPECT_FALSE(lib.lastError().empty());
}

TEST(DynamicLibraryTest, MoveConstructor) {
  DynamicLibrary lib1;
  DynamicLibrary lib2(std::move(lib1));
  EXPECT_FALSE(lib2.isLoaded());
  EXPECT_TRUE(lib1.path().empty());
  EXPECT_TRUE(lib2.lastError().empty());
}

TEST(DynamicLibraryTest, MoveAssignment) {
  DynamicLibrary lib1;
  DynamicLibrary lib2;
  lib2 = std::move(lib1);
  EXPECT_FALSE(lib2.isLoaded());
  EXPECT_TRUE(lib1.path().empty());
}

TEST(DynamicLibraryTest, UnloadOnDestruction) {
  {
    DynamicLibrary lib;
    EXPECT_FALSE(lib.isLoaded());
  }
}

TEST(DynamicLibraryTest, RepeatedLoad) {
  DynamicLibrary lib;
  EXPECT_FALSE(lib.load("C:\\nonexistent_1.dll"));
  EXPECT_FALSE(lib.load("C:\\nonexistent_2.dll"));
  EXPECT_FALSE(lib.isLoaded());
}

TEST(DynamicLibraryTest, UnloadWithoutLoad) {
  DynamicLibrary lib;
  EXPECT_NO_THROW(lib.unload());
}

TEST(DynamicLibraryTest, SelfMoveAssignmentNoCrash) {
  DynamicLibrary lib;
  lib = std::move(lib);
  EXPECT_FALSE(lib.isLoaded());
}
