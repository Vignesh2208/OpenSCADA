#load("@build_stack_rules_proto//cpp:cpp_proto_library.bzl", "cpp_proto_library")

proto_library(
    name = "pc_configuration_proto",
    srcs = ["src/pc_emulator/proto/configuration.proto"],
    deps = [],
    
)


cc_proto_library(
    name = "pc_configuration_cc_proto",
    deps = [":pc_configuration_proto"],
    visibility = ["//visibility:public"],
    
)

proto_library(
    name = "pc_system_specification_proto",
    srcs = ["src/pc_emulator/proto/system_specification.proto",
            "src/pc_emulator/proto/configuration.proto"],
    deps = [],
    
)


cc_proto_library(
    name = "pc_system_specification_cc_proto",
    deps = [":pc_system_specification_proto"],
    visibility = ["//visibility:public"],
    
)



cc_library(
    name="pc_emulator_lib",
    hdrs = glob([
        "src/pc_emulator/include/**/*.h",
        "src/pc_emulator/include/*.h",
        "src/pc_emulator/ext_modules/include/*.h"
    ]),
    srcs = glob([
        "src/pc_emulator/core/**/*.cc",
        "src/pc_emulator/core/*.cc",
        "src/pc_emulator/ext_modules/*.cc"
    ]),
    deps = ["@boost//:algorithm",
            "@boost//:lexical_cast",
            "@boost//:program_options",
            "@boost//:exception",
            ":pc_system_specification_cc_proto"],
    
    copts = ["-fpermissive -Wno-reorder -DDIR=\"$$PWD/\""],
    visibility = ["//visibility:public"],
    alwayslink = True,
)

cc_test(
    name = "datatype_test",
    srcs = ["src/pc_emulator/tests/datatype_tests/datatype_test.cc"],
    copts = ["-Iexternal/gtest/include -fpermissive -Wno-reorder"],
    deps = [":pc_emulator_lib",
           "@gtest//:main"],
    linkstatic = 1,
)

cc_test(
    name = "variable_test",
    srcs = ["src/pc_emulator/tests/variable_tests/variable_test.cc"],
    copts = ["-Iexternal/gtest/include -fpermissive -Wno-reorder"],
    deps = [":pc_emulator_lib",
           "@gtest//:main"],
    linkstatic = 1,
)

cc_test(
    name = "insn_test",
    srcs = ["src/pc_emulator/tests/insn_tests/insn_test.cc"],
    copts = ["-Iexternal/gtest/include -fpermissive -Wno-reorder"],
    deps = [":pc_emulator_lib",
           "@gtest//:main"],
    linkstatic = 1,
)

cc_test(
    name = "access_test",
    srcs = ["src/pc_emulator/tests/access_variable_tests/access_variable_test.cc"],
    copts = ["-Iexternal/gtest/include -fpermissive -Wno-reorder"],
    deps = [":pc_emulator_lib",
           "@gtest//:main"],
    linkstatic = 1,
)

cc_test(
    name = "sfc_test",
    srcs = ["src/pc_emulator/tests/sfc_tests/sfc_test.cc"],
    copts = ["-Iexternal/gtest/include -fpermissive -Wno-reorder"],
    deps = [":pc_emulator_lib",
           "@gtest//:main"],
    linkstatic = 1,
)