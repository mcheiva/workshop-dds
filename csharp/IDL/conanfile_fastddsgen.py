import os

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.layout import basic_layout

required_conan_version = ">=1.53.0"


class EivaFastDDSGenConan(ConanFile):
    name = "eiva-fast-dds-gen"
    license = "Apache-2.0"
    homepage = "https://fast-dds.docs.eprosima.com/"
    url = "https://github.com/conan-io/conan-center-index"
    description = "The most complete OSS DDS implementation for embedded systems."
    topics = ("dds", "middleware", "ipc")
    package_type = "application"
    settings = "os", "arch", "compiler", "build_type"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }

    default_options = {
        "shared": False,
        "fPIC": True,
    }

    def requirements(self):
        self.requires("eiva-fast-dds/3.1.0")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def source(self):
        git_url = "https://github.com/eivacom/Fast-DDS-Gen"
        git_hash = "feature/CSharpSupport"

        self.run(f"git clone --recursive {git_url} Fast-DDS-Gen")
        self.run(f"cd Fast-DDS-Gen && git checkout {git_hash}")
        
        gradle_wrapper = os.path.join(self.source_folder, "Fast-DDS-Gen", "gradlew")
        self.run(f"{gradle_wrapper} assemble", cwd=os.path.join(self.source_folder, "Fast-DDS-Gen"))
        
    def build_requirements(self):
        self.tool_requires("openjdk/19.0.2")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")
    
    def layout(self):
        cmake_layout(self, src_folder=".", build_folder="build-fastddsgen")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.generate()
        tc = CMakeDeps(self)
        tc.check_components_exist = True
        tc.generate()
    
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()