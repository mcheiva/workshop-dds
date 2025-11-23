from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.cmake import CMakeToolchain, CMakeDeps
from conan.tools.files import collect_libs

required_conan_version = ">=2.0.6"


class EIVAWorkshopRecipe(ConanFile):
    name = "eiva-workshop"
    version = "0.1.0"
    license = "N/A"
    author = "EIVA"
    url = ""
    description = "EIVA C++ Middleware Interface"
    topics = ("EIVA", "DDS")
    settings = "os", "compiler", "build_type", "arch"
    package_type = "library"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }

    default_options = {
        "shared": False,
        "fPIC": True,
    }

    def configure(self):
        if not self.options.shared:
            # If os=Windows, fPIC will have been removed in config_options()
            # use rm_safe to avoid double delete errors
            self.options.rm_safe("fPIC")
        # self.options["opencv"].with_webp = False

    def requirements(self):
        self.requires("eiva-fast-dds/3.3.0")

    def build_requirements(self):
        self.tool_requires("eiva-fast-dds-gen/4.1.0")
        self.tool_requires("openjdk/19.0.2")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        CMakeDeps(self).generate()

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)
        self.cpp_info.set_property("cmake_file_name", "ddsbus")
        self.cpp_info.set_property("cmake_target_name", "ddsbus")
