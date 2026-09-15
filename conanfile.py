import os

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain
from conan.tools.files import copy


GENERATED_SCRIPT_EXCLUDES = ["*.pyc", "__pycache__/*"]


class NbnCppApiCoreConan(ConanFile):
    name = "nbn-cpp-api-core"

    def set_version(self):
        version_path = os.path.join(self.recipe_folder, "VERSION")
        with open(version_path, encoding="utf-8") as version_file:
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
        "README.md",
        "!**/__pycache__/*",
        "!**/*.pyc",
    )

    def build_requirements(self):
        self.tool_requires("nbn-cpp-api-build-system/[>=1.0 <2.0]")

    def layout(self):
        profile = self.conf.get("user.nbn:profile")
        source_core = self.cpp.source.components["core"]
        source_core.includedirs = ["src"]
        build_core = self.cpp.build.components["core"]
        if profile:
            build_directory = os.path.join("build", str(profile))
            build_core.includedirs = [build_directory]
            build_core.libdirs = [
                os.path.join(build_directory, "src", "nbn", "core")
            ]
        benchmark = self.cpp.source.components["benchmark"]
        benchmark.includedirs = [os.path.join("tests", "core", "utilities")]

    def generate(self):
        tc = CMakeToolchain(self, generator="Ninja")
        tc.cache_variables["NBN_BUILD_SHARED_LIBS"] = self.options.shared
        tc.cache_variables["NBN_BUILD_STATIC_LIBS"] = not self.options.shared
        tc.cache_variables["NBN_BUILD_TESTS"] = False
        tc.cache_variables["NBN_CORE_DOXYGEN_ENABLE"] = False
        tc.cache_variables["NBN_CLANG_TIDY_ENABLE"] = True
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

    def package_info(self):
        scripts_dir = os.path.join(self.package_folder, "scripts")
        self.buildenv_info.define_path(
            "NBN_CPP_API_CORE_SCRIPTS_DIR", scripts_dir
        )
        self.runenv_info.define_path(
            "NBN_CPP_API_CORE_SCRIPTS_DIR", scripts_dir
        )
        core = self.cpp_info.components["core"]
        core.libs = ["nbn-core"]
        if self.settings.os == "Linux" and self.settings.compiler in (
            "gcc",
            "clang",
        ):
            core.system_libs = ["stdc++exp"]
        self.cpp_info.set_property("cmake_file_name", "nbn-cpp-api-core")
        core.set_property("cmake_target_name", "nbn::core")
        benchmark = self.cpp_info.components["benchmark"]
        benchmark.set_property("cmake_target_name", "nbn::benchmark")
        benchmark.includedirs = ["include"]
