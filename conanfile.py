import os

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain


class NbnCppApiCoreConan(ConanFile):
    name = "nbn-cpp-api-core"

    def set_version(self):
        with open(os.path.join(self.recipe_folder, "VERSION"), encoding="utf-8") as version_file:
            self.version = version_file.read().strip()

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {
        "shared": True,
    }

    exports_sources = "CMakeLists.txt", "VERSION", "cmake/*", "src/*", "tests/*", "benchmark/*", "scripts/*", "README.md"

    def build_requirements(self):
        self.tool_requires("nbn-cpp-api-build-system/[>=1.0 <2.0]")

    def generate(self):
        tc = CMakeToolchain(self, generator="Ninja")
        tc.variables["NBN_BUILD_SHARED_LIBS"] = self.options.shared
        tc.variables["NBN_BUILD_STATIC_LIBS"] = not self.options.shared
        tc.variables["NBN_BUILD_TESTS"] = False
        tc.variables["NBN_CORE_DOXYGEN_ENABLE"] = False
        tc.variables["NBN_CLANG_TIDY_ENABLE"] = False
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["nbn-core"]
        if self.settings.os == "Linux" and self.settings.compiler in ("gcc", "clang"):
            self.cpp_info.system_libs = ["stdc++exp"]
        self.cpp_info.set_property("cmake_file_name", "nbn-cpp-api-core")
        self.cpp_info.set_property("cmake_target_name", "nbn::core")
