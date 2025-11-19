from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import (
    apply_conandata_patches,
    collect_libs,
    copy,
    export_conandata_patches,
    get,
    rename,
    rm,
    rmdir,
    save,
)
from conan.tools.microsoft import check_min_vs, is_msvc_static_runtime, is_msvc, msvc_runtime_flag
from conan.tools.scm import Version
import os
import textwrap
import shutil

required_conan_version = ">=2.0.6"
class EivaCSharpIdl(ConanFile):
    name = "eiva-csharp-idl"
    version = "1.0.0"
    license = "Apache-2.0"
    homepage = "https://fast-dds.docs.eprosima.com/"
    url = "https://github.com/conan-io/conan-center-index"
    description = "The most complete OSS DDS implementation for embedded systems."
    topics = ("dds", "middleware", "ipc")
    package_type = "library"
    settings = "os", "arch", "compiler", "build_type"
    # comment to trigger change detection

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }

    default_options = {
        "shared": False,
        "fPIC": True,
    }

    def export_sources(self):
        export_conandata_patches(self)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self, src_folder="generated", build_folder="build-swig")

    def requirements(self):
        self.requires("eiva-swig/4.3.0")
        self.requires("eiva-fast-dds/3.1.0")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)

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

        cmake.install()
