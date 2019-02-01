cc_library(
    name = "boost",
    srcs = glob(["lib/*.so*"]),
    hdrs = glob(["include/**/*.hpp", "include/**/*.h"]),
    includes = ["include"],
    visibility = ["//visibility:public"],
    #linkopts = ["-lpython3"],
    linkstatic = 1,
)