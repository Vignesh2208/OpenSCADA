from __future__ import print_function
import logging

import grpc

import src.pc_emulator.proto.mem_access_pb2 as mem_access_proto
import src.pc_emulator.proto.mem_access_pb2_grpc as mem_access_grpc


def main():
    # NOTE(gRPC Python Team): .close() is possible on a channel and should be
    # used in circumstances in which the with statement does not fit the needs
    # of the code.
    with grpc.insecure_channel('localhost:50051') as channel:
        stub = mem_access_grpc.AccessServiceStub(channel)
        response = stub.GetActuatorOutput(
            mem_access_proto.ActuatorOutput(
                PLC_ID='DataType_Test_Configuration',
                ResourceName='CPU_001',
                MemType=1,
                ByteOffset=10,
                BitOffset=0,
                VariableDataTypeName="DT",
                ))
    print("Response client received: " + str(response))

    
if __name__ == '__main__':
    logging.basicConfig()
    main()