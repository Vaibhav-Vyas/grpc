/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include <stdint.h>
#include "helloworld.grpc.pb.h"

//#define GRPC_PROFILE_DEBUG_MODE     1

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

extern void print_all_profile_stats();
extern uint64_t nanos_since_midnight();
//extern int add_func_stats(std::string funcName, uint64_t start_ns, uint64_t end_nsec, std::string fName = "", std::string desc = "");

/*
  //Windows
#ifdef _WIN32

        #include <intrin.h>
        uint64_t rdtsc(){
                return __rdtsc();
        }

  //Linux/GCC
#else

        uint64_t rdtsc(){
                unsigned int lo,hi;
                __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
                return ((uint64_t)hi << 32) | lo;
        }

#endif

struct FuncStats
{
    std::string funcName;
    std::string fileName;
    std::string description;
    uint64_t start_ns;
    uint64_t end_ns;
    uint64_t duration_ns;

    FuncStats()
    {
    }

    FuncStats(std::string func_name, uint64_t start_nsec, uint64_t end_nsec, std::string file_name = "", std::string desc = "")
    {
        funcName = func_name;
        start_ns = start_nsec;
        end_ns = end_nsec;
        duration_ns = end_nsec - start_nsec;
        fileName = (file_name.length() > 0) ? file_name : " ";
        description = (desc.length() > 0) ? desc : " ";
    }
};

std::vector<FuncStats> funcProfilerOwn;
 
int add_func_stats(std::string func_name, uint64_t start_ns, uint64_t end_nsec, std::string file_name, std::string desc)
{

    #define GRPC_PROFILE_DEBUG_MODE     1

    FuncStats currFuncStat(func_name, start_ns, end_nsec, file_name, desc);

    if (GRPC_PROFILE_DEBUG_MODE)
    {
        std::cout << func_name                                                                                          \
                << ", Start:" << currFuncStat.start_ns << " ns,"                                \
                        << ", End:" << currFuncStat.end_ns << " ns,"                            \
            << " Duration:" << currFuncStat.duration_ns << " ns,"          \
            << " Description:" << currFuncStat.description << ","          \
            << " FileName:" << currFuncStat.fileName << std::endl;
    }
    funcProfilerOwn.push_back(currFuncStat);
    return 0;
}


void print_all_profile_stats()
{
    FuncStats this_stat;
    for (unsigned int i = 0; i <  (unsigned int)funcProfiler.size(); i++)
    {
        this_stat = funcProfiler[i];
        printf("%s,%ld,%ld\n", this_stat.funcName.c_str(), this_stat.start_ns, this_stat.duration_ns);
    }

}

uint64_t nanos_since_midnight()
{
    return rdtsc();//std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
*/

//#endif  // GRPCXX_IMPL_CLIENT_UNARY_CALL_H

// Logic and data nehind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    return Status::OK;
  }
};

void RunServer() {
  uint64_t start, end, start0, end0;

  start0 = nanos_since_midnight();
  start = nanos_since_midnight();
  std::string server_address("0.0.0.0:50051");
  end = nanos_since_midnight();
  add_func_stats("server_address", start, end,  std::string(__FILE__), "IP address allocator"); 

  start = nanos_since_midnight();
  GreeterServiceImpl service;
  end = nanos_since_midnight();
  add_func_stats("GreeterServiceImpl ",start, end, std::string(__FILE__)," service init: generic server, thread pool, msg size: max (64M)-1, compression method");

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  start = nanos_since_midnight();
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  end = nanos_since_midnight();
  add_func_stats("ServerBuilder.AddListeningPort ", start, end, std::string(__FILE__), " add listening port in the namespace");

  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  start = nanos_since_midnight();
  builder.RegisterService(&service);
  end = nanos_since_midnight();
  add_func_stats("ServerBuilder.RegisterService", start, end, std::string(__FILE__), " register service and its methods in the namespace ");

  // Finally assemble the server.
  start = nanos_since_midnight();
  std::unique_ptr<Server> server(builder.BuildAndStart());

  end = nanos_since_midnight();
  add_func_stats("server(builder.BuildAndStart()", start, end, "assemble the server: check cannot be both async and sync services; create standard thread pool; assign thread pool ownership, completion queue, and start the service");

  end0 = nanos_since_midnight();
  add_func_stats("Runserver ", start, end, std::string(__FILE__), "Total setup overhead ");

  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.

  add_func_stats("CallOpServerSendStatus() : send_status_available_(false) ", start, end -start);

  print_all_profile_stats(); 
  server->Wait();
}

int main(int argc, char** argv) {

  RunServer();

  return 0;
}



