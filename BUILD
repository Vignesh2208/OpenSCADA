#load("@build_stack_rules_proto//cpp:cpp_proto_library.bzl", "cpp_proto_library")

load("@build_stack_rules_proto//cpp:cpp_grpc_library.bzl", "cpp_grpc_library")
load("@build_stack_rules_proto//python:python_grpc_library.bzl", "python_grpc_library")

proto_library(
    name = "access_service_proto",
    srcs = ["src/pc_emulator/proto/mem_access.proto"],
    deps = [],
    visibility = ["//visibility:public"],
)

cpp_grpc_library(
    name = "access_service_proto_cpp",
    deps = [":access_service_proto"],
    visibility = ["//visibility:public"],  
)

python_grpc_library(
  name = "py_access_service_proto",
  deps = [":access_service_proto"],
  visibility = ["//visibility:public"],
)

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
            "@kronoslib//:kronosapi",
            ":pc_system_specification_cc_proto"],
    
    copts = ["-fpermissive -Wno-reorder -DDIR=\"$$PWD/\" -Wno-sign-compare -Wno-unused-variable"],
    visibility = ["//visibility:public"],
    alwayslink = True,
)

cc_library(
    name="grpc_ext_lib",
    hdrs = ["src/pc_emulator/grpc_ext/include/access_service_impl.h",
            "src/pc_emulator/grpc_ext/include/ext_interface_grpc_api.h"],
    srcs = [
        "src/pc_emulator/grpc_ext/access_service_impl.cc",
        "src/pc_emulator/grpc_ext/ext_interface_grpc_api.cc"],
    deps = [
            ":pc_emulator_lib", ":access_service_proto_cpp", 
            "@boost//:filesystem"],
    
    copts = ["-fpermissive -Wno-reorder -DDIR=\"$$PWD/\" -Wno-sign-compare"],
    visibility = ["//visibility:public"],
    alwayslink = True,
)

cc_binary(
    name="pc_grpc_server",
    srcs = ["src/pc_emulator/grpc_ext/grpc_server_main.cc"],
    deps = [":grpc_ext_lib", ":access_service_proto_cpp"],
    copts = ["-fpermissive -Wno-reorder -DDIR=\"$$PWD/\" -Wno-sign-compare"],
    visibility = ["//visibility:public"],
    linkstatic = 1,
)


py_binary(
    name="simulation",
    srcs = [
        "examples/inverted_pendulum/simulation.py",
    ],
    deps = [":py_access_service_proto"],
)

py_binary(
    name="simulation_2cpus",
    srcs = [
        "examples/inverted_pendulum_2cpus/simulation_2cpus.py",
    ],
    deps = [":py_access_service_proto"],
)

cc_binary(
    name="example_comm_module",
    srcs = [
        "examples/idle_plc/comm_module.cc"
    ],
    deps = [":pc_emulator_lib"], 
    copts = ["-Iexternal/gtest/include -I/usr/local/include -fpermissive -Wno-reorder"],
    linkstatic = 1,

)

cc_binary(
    name="example_hmi",
    srcs = [
        "examples/inverted_pendulum/hmi.cc"
    ],
    deps = ["@boost//:algorithm", "@modbuslib//:modbusapi"], 
    copts = ["-Iexternal/gtest/include -I/usr/local/include -fpermissive -Wno-reorder"],
    linkstatic = 1,
)

cc_binary(
    name="example_hmi_2conns",
    srcs = [
        "examples/inverted_pendulum_2cpus/hmi.cc"
    ],
    deps = ["@boost//:algorithm", "@modbuslib//:modbusapi"], 
    copts = ["-Iexternal/gtest/include -I/usr/local/include -fpermissive -Wno-reorder"],
    linkstatic = 1,
)

cc_binary(
    name="modbus_comm_module",
    srcs = [
        "contrib/modbus_comm_module.cc"
    ],
    deps = [":pc_emulator_lib", "@modbuslib//:modbusapi"], 
    copts = ["-Iexternal/gtest/include -fpermissive -Wno-reorder -Wno-sign-compare -Wno-delete-non-virtual-dtor"],
    linkstatic = 1,
)

cc_binary(
    name="plc_runner",
    srcs = [
        "contrib/plc_runner.cc"
    ],
    deps = [":pc_emulator_lib",  "@kronoslib//:kronosapi"], 
    copts = ["-Iexternal/gtest/include -I/usr/local/include -fpermissive -Wno-reorder -Wno-switch -Wno-delete-non-virtual-dtor"],
    linkstatic = 1, 
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
    name = "execution_test",
    srcs = ["src/pc_emulator/tests/execution_tests/execution_test.cc"],
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
