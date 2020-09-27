#!/bin/bash


if [ "$1" == "install" ]; then
    echo "Build Started with Bazel ..."

    echo "Building Emulator library ..."
    bazel build :pc_emulator_lib

    echo "Building Python Proto libs ..."
    bazel build :py_access_service_proto

    echo "Building LibModbus extensions ..."
    cd contrib/libmodbus && ./autogen.sh && ./configure && make install
    cd ../../
    
    sudo ldconfig

    echo "Building GRPC extensions ..."
    bazel build :grpc_ext_lib

    echo "Building GRPC server ..."
    bazel build :pc_grpc_server
    sudo cp bazel-bin/pc_grpc_server /usr/bin

    echo "Building Modbus comm module ..."
    bazel build :modbus_comm_module
    sudo cp bazel-bin/modbus_comm_module /usr/bin

    echo "Building PLC Runnable ..."
    bazel build :plc_runner
    sudo cp bazel-bin/plc_runner /usr/bin

    echo "Building Example HMI ..."
    bazel build :example_hmi
    sudo cp bazel-bin/example_hmi /usr/bin

    sudo cp bazel-genfiles/py_access_service_proto_pb/src/pc_emulator/proto/*.py src/pc_emulator/proto
    sudo chmod 777 src/pc_emulator/proto/*.py 

    echo "Build and Install finished ..."
else
    if [ "$1" == "uninstall" ]; then
        echo "Cleaning up installation ..."
        bazel clean
        rm /usr/bin/pc_grpc_server || true
        rm /usr/bin/modbus_comm_module || true
        rm /usr/bin/plc_runner || true
        rm /usr/bin/example_hmi || true
	rm src/pc_emulator/proto/*.py
        cd contrib/libmodbus && make uninstall
        cd ../../ 
        echo "Cleanup finished ..."
    fi
fi
