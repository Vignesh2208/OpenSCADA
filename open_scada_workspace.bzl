load("//third_party/com_github_robotlocomotion_drake:tools/workspace/github.bzl", "github_archive")
load("//third_party/com_github_robotlocomotion_drake:tools/workspace/pkg_config.bzl", "pkg_config_repository")
load("//third_party/com_github_tensorflow_tensorflow/py:python_configure.bzl", "python_configure")

def openscada_workspace():
    pkg_config_repository(
        name = "ibex",  # LGPL3
        modname = "ibex",
        pkg_config_paths = [
            # macOS
            "/usr/local/opt/ibex@2.7.4/share/pkgconfig",
            "/usr/local/opt/clp/lib/pkgconfig",  # dep of ibex, EPL
            "/usr/local/opt/coinutils/lib/pkgconfig",  # dep of clp, EPL
            # Linux
            "/opt/libibex/2.7.4/share/pkgconfig",
        ],
    )
    pkg_config_repository(
        name = "nlopt",  # LGPL2 + MIT
        modname = "nlopt",
        pkg_config_paths = [
            "/usr/local/opt/nlopt/lib/pkgconfig",
        ],
    )

    python_configure(name = "local_config_python")

    github_archive(
        name = "pybind11",  # BSD
        repository = "pybind/pybind11",
        commit = "v2.2.4",
        sha256 = "b69e83658513215b8d1443544d0549b7d231b9f201f6fc787a2b2218b408181e",
        build_file = str(Label("//tools:pybind11.BUILD.bazel")),
    )
