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

cc_library(
    name="pc_emulator_lib",
    hdrs=[
        "src/pc_emulator/include/executor.h",
        "src/pc_emulator/include/pc_clock.h",
        "src/pc_emulator/include/pc_configuration.h",
        "src/pc_emulator/include/pc_datatype_registry.h",
        "src/pc_emulator/include/pc_datatype.h",
        "src/pc_emulator/include/pc_logger.h",
        "src/pc_emulator/include/pc_mem_unit.h",
        "src/pc_emulator/include/pc_pou_code_container.h",
        "src/pc_emulator/include/pc_resource_registry.h",
        "src/pc_emulator/include/pc_resource.h",
        "src/pc_emulator/include/pc_variable.h",
        "src/pc_emulator/include/resource_manager.h",
        "src/pc_emulator/include/task.h",
        "src/pc_emulator/include/utils.h",
        "src/pc_emulator/include/elementary_datatypes.h"],
    srcs=[
        "src/pc_emulator/core/executor.cc",
        "src/pc_emulator/core/pc_clock.cc",
        "src/pc_emulator/core/pc_configuration.cc",
        "src/pc_emulator/core/pc_datatype_registry.cc",
        "src/pc_emulator/core/pc_datatype.cc",
        "src/pc_emulator/core/pc_logger.cc",
        "src/pc_emulator/core/pc_mem_unit.cc",
        "src/pc_emulator/core/pc_pou_code_container.cc",
        "src/pc_emulator/core/pc_resource_registry.cc",
        "src/pc_emulator/core/pc_resource.cc",
        "src/pc_emulator/core/pc_variable.cc",
        "src/pc_emulator/core/resource_manager.cc",
        "src/pc_emulator/core/task.cc",
        "src/pc_emulator/core/utils.cc"],

    deps = ["@boost//:boost",
            ":pc_configuration_cc_proto"],
    
    copts = ["-fpermissive -fno-implicit-templates -Wreorder"],
    visibility = ["//visibility:public"],
)