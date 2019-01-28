load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")


http_archive(
    name = "com_google_protobuf",
    strip_prefix = "protobuf-3.6.1.3",
    urls = ["https://github.com/google/protobuf/archive/v3.6.1.3.zip"],
)



new_local_repository(
    name = "boost",
    build_file = "boost.BUILD",
    path = "/src/boost/boost_1_61_0/release",
)


new_git_repository(
    name = "googletest",
    build_file = "gmock.BUILD",
    remote = "https://github.com/google/googletest",
    tag = "release-1.8.0",
)