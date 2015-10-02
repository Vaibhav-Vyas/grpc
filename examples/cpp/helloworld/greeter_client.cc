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

#include "helloworld.grpc.pb.h"
#include "./benchmarkUtils.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user,
                       struct timespec *pStart,
                       struct timespec *pEnd,
                       uint64_t *pDiff) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // Measuring RPC msg overhead.
    clock_gettime(CLOCK_MONOTONIC, pStart);

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply); 

    clock_gettime(CLOCK_MONOTONIC, pEnd);
    *pDiff = diffTime(*pStart, *pEnd);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

//! Function to measure the Round Trip time of GRPC with small message size.
int
measureRoundTripTime(GreeterClient *pGreeter,
    char *pServIPPort, 
    int gccOptimizationMode,
    int maxAttempts);

int main(int argc, char** argv) {
  int gccOptimizationMode;
  int maxAttempts = 10;
  char servIPPort[25] = {0};
  int rc = 0;

  if(argc != 3)
  {
          printf("\n Usage: %s <ip of server:port> <optimization_enabled ? 1 : 0>\n", argv[0]);
          return 1;
  }
  else
  {
      printf("IP =%s", argv[1]);
      sprintf(servIPPort, "%s __", argv[1]);
      servIPPort[strlen(argv[1])] = '\0';
      //strcpy(servIPPort, "%s\0", argv[1]);
      gccOptimizationMode = atoi(argv[2]);
  }

  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureCredentials()).
  GreeterClient greeter(
      grpc::CreateChannel(servIPPort, grpc::InsecureCredentials()));

  rc = measureRoundTripTime(&greeter,
                       servIPPort,
                       gccOptimizationMode,
                       maxAttempts); 
}

int
measureRoundTripTime(GreeterClient *pGreeter,
    char *pServIPPort, 
    int gccOptimizationMode,
    int maxAttempts)
{
    // For Timing measurement.
    struct timespec start; 
    struct timespec end;
    uint64_t diffNanoSec;
    int run;
    vector<string> strMsg;
    std::string user("world");
    char experimentTitle[64];


    sprintf(experimentTitle,"Q4_RoundTripCalc_OptimzationMode_%d_RTT_SmallMsg_IP_%s",
        gccOptimizationMode, pServIPPort);

    cout << "================================================" << std::endl;
    cout << "4) Measuring GRPC roundtrip time (small msg => string msg." << std::endl;
    cout << "================================================" << std::endl;
    for (run = 0; run < maxAttempts; run++)
    {
      std::string reply = pGreeter->SayHello(user, &start, &end, &diffNanoSec);
      monotonicClk[run] = (uint64_t) diffNanoSec;

      //std::cout << "Greeter received: " << reply << std::endl;
      printf("\t * RTT Attempts: %d), String Type, Sent Msg=,%s, Received Msg=,%s, Time taken = ,%llu,nanoseconds\n",
              run, user.c_str(), reply.c_str(), 
             (long long unsigned int)diffNanoSec);
    }
    createCSVReport(experimentTitle, maxAttempts, &strMsg);

    return 0;
}
