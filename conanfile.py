import os

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain
from conan.tools.files import copy


GENERATED_SCRIPT_EXCLUDES = ["*.pyc", "__pycache__/*"]


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

    exports_sources = (
        "CMakeLists.txt",
        "VERSION",
        "cmake/*",
        "src/*",
        "tests/*",
        "scripts/*",
        ".vscode/*",
        ".github/*",
        "README.md",
        "!**/__pycache__/*",
        "!**/*.pyc",
    )

    def build_requirements(self):
        self.tool_requires("nbn-cpp-api-build-system/[>=1.0 <2.0]")

    def generate(self):
        tc = CMakeToolchain(self, generator="Ninja")
        tc.variables["NBN_BUILD_SHARED_LIBS"] = self.options.shared
        tc.variables["NBN_BUILD_STATIC_LIBS"] = not self.options.shared
        tc.variables["NBN_BUILD_TESTS"] = False
        tc.variables["NBN_CORE_DOXYGEN_ENABLE"] = False
        tc.variables["NBN_LLVM_COVERAGE_ENABLE"] = False
        tc.variables["NBN_CLANG_TIDY_ENABLE"] = True
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        # Package c++
        cmake = CMake(self)
        cmake.install()
        # Package scripts
        source_root = self.source_folder
        package_root = self.package_folder
        copy(
            self,
            "*",
            src=os.path.join(source_root, "scripts"),
            dst=os.path.join(package_root, "scripts"),
            excludes=GENERATED_SCRIPT_EXCLUDES,
        )
        # Package downstream VS Code and AgentX assets separately from C++ files.
        copy(
            self,
            "*",
            src=os.path.join(source_root, ".vscode"),
            dst=os.path.join(package_root, "vscode", ".vscode"),
            excludes=GENERATED_SCRIPT_EXCLUDES,
        )
        copy(
            self,
            "*",
            src=os.path.join(source_root, ".github"),
            dst=os.path.join(package_root, "vscode", ".github"),
            excludes=GENERATED_SCRIPT_EXCLUDES,
        )


    def package_info(self):
        core = self.cpp_info.components["core"]
        core.libs = ["nbn-core"]
        if self.settings.os == "Linux" and self.settings.compiler in ("gcc", "clang"):
            core.system_libs = ["stdc++exp"]
        self.cpp_info.set_property("cmake_file_name", "nbn-cpp-api-core")
        core.set_property("cmake_target_name", "nbn::core")
        benchmark = self.cpp_info.components["benchmark"]
        benchmark.set_property("cmake_target_name", "nbn::benchmark")
        benchmark.includedirs = ["include"]
