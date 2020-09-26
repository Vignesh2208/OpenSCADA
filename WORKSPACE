

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

http_archive(
    name = "com_google_protobuf",
    strip_prefix = "protobuf-3.6.1.3",
    urls = ["https://github.com/google/protobuf/archive/v3.6.1.3.zip"],
)

http_archive(
    name = "gtest",
    url = "https://github.com/google/googletest/archive/release-1.7.0.zip",
    build_file = "@//:gmock.BUILD",
    strip_prefix = "googletest-release-1.7.0",
)

git_repository(
    name = "com_github_nelhage_rules_boost",
    commit = "6d6fd834281cb8f8e758dd9ad76df86304bf1869",
    remote = "https://github.com/nelhage/rules_boost",
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

new_local_repository(
    name = "kronoslib",
    # pkg-config --variable=libdir kronosapi
    path = "/usr/lib",
    build_file_content = """
cc_library(
    name = "kronosapi",
    srcs = ["libkronosapi.so"],
    visibility = ["//visibility:public"],
    alwayslink = 1,
)
""",
)

new_local_repository(
    name = "modbuslib",
    # pkg-config --variable=libdir modbusapi
    path = "/usr/local/lib",
    build_file_content = """
cc_library(
    name = "modbusapi",
    srcs = ["libmodbus.so"],
    visibility = ["//visibility:public"],
    alwayslink = 1,
)
""",
)


http_archive(
    name = "build_stack_rules_proto",
    urls = ["https://github.com/stackb/rules_proto/archive/d86ca6bc56b1589677ec59abfa0bed784d6b7767.tar.gz"],
    sha256 = "36f11f56f6eb48a81eb6850f4fb6c3b4680e3fc2d3ceb9240430e28d32c47009",
    strip_prefix = "rules_proto-d86ca6bc56b1589677ec59abfa0bed784d6b7767",
)

load("@build_stack_rules_proto//cpp:deps.bzl", "cpp_grpc_compile")
cpp_grpc_compile()

load("@build_stack_rules_proto//cpp:deps.bzl", "cpp_grpc_library")
cpp_grpc_library()



load("@build_stack_rules_proto//python:deps.bzl", "python_grpc_library")
python_grpc_library()

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")
grpc_deps()

load("@io_bazel_rules_python//python:pip.bzl", "pip_import", "pip_repositories")
pip_repositories()

pip_import(
    name = "protobuf_py_deps",
    requirements = "@build_stack_rules_proto//python/requirements:protobuf.txt",
)

load("@protobuf_py_deps//:requirements.bzl", protobuf_pip_install = "pip_install")
protobuf_pip_install()

pip_import(
    name = "grpc_py_deps",
    requirements = "@build_stack_rules_proto//python:requirements.txt",
)

load("@grpc_py_deps//:requirements.bzl", grpc_pip_install = "pip_install")
grpc_pip_install()
