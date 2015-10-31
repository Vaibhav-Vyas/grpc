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
using grpc::ClientWriter;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloRequestInt;
using helloworld::HelloRequestDouble;
using helloworld::HelloRequestComplex;
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


   // Assambles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHelloString(const std::string& user,
                       struct timespec *pStart,
                       struct timespec *pEnd,
                       uint64_t *pDiff) {
    std::string buf;
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;


    // Measuring RPC msg marshalling overhead.
    clock_gettime(CLOCK_MONOTONIC, pStart);

    request.SerializeToString(&buf);
    clock_gettime(CLOCK_MONOTONIC, pEnd);
    *pDiff = diffTime(*pStart, *pEnd);

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply); 

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      return "RPC failed";
    }
  }



void RecordRoute(long bytesPerPacket,
           int noOfLoops,
           struct timespec *pStart,
           struct timespec *pEnd,
           uint64_t *pDiff) {
    HelloRequest point;
    HelloReply stats;
    ClientContext context;

    char *arr = (char *)malloc(bytesPerPacket);
    memset(arr, 'A', bytesPerPacket);
    string largeStr(arr);
    point.set_name(largeStr);

    // Measuring RPC msg marshalling overhead.
    clock_gettime(CLOCK_MONOTONIC, pStart);

    std::unique_ptr<ClientWriter<HelloRequest> > writer(
        stub_->RecordRoute(&context, &stats));
    for (int i = 0; i < noOfLoops; i++) {

      if (!writer->Write(point)) {
        // Broken stream.
        break;
      }

    }
    writer->WritesDone();
    Status status = writer->Finish();

    clock_gettime(CLOCK_MONOTONIC, pEnd);
    *pDiff = diffTime(*pStart, *pEnd);

    if (status.ok()) {
      std::cout << "Finished: Server Sent Msg" << stats.message() << std::endl;
    } else {
      std::cout << "RecordRoute rpc failed." << std::endl;
    }
    free(arr);
  }





  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHelloInt(const ::google::protobuf::int32 empid,
                       struct timespec *pStart,
                       struct timespec *pEnd,
                       uint64_t *pDiff) {
    std::string buf;
    // Data we are sending to the server.

    HelloRequestInt request;

    request.set_empid(empid);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // Measuring RPC msg marshalling overhead.
    clock_gettime(CLOCK_MONOTONIC, pStart);

    request.SerializeToString(&buf);
    clock_gettime(CLOCK_MONOTONIC, pEnd);
    *pDiff = diffTime(*pStart, *pEnd);

    // The actual RPC.
    Status status = stub_->SayHelloInt(&context, request, &reply); 
    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      return "RPC failed";
    }
  }

  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHelloDouble(const double salary,
                       struct timespec *pStart,
                       struct timespec *pEnd,
                       uint64_t *pDiff) {
    std::string buf;
    // Data we are sending to the server.

    HelloRequestDouble request;

    request.set_salary(salary);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // Measuring RPC msg marshalling overhead.
    clock_gettime(CLOCK_MONOTONIC, pStart);

    request.SerializeToString(&buf);
    clock_gettime(CLOCK_MONOTONIC, pEnd);
    *pDiff = diffTime(*pStart, *pEnd);

    // The actual RPC.
    Status status = stub_->SayHelloDouble(&context, request, &reply); 
    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      return "RPC failed";
    }
  }

  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHelloComplex(const ::google::protobuf::int32 empid,
                       const std::string& user,
                       const double salary,
                       const ::google::protobuf::int32 empidManager,
                       const std::string& userManager,
                       const double salaryManager,
                       struct timespec *pStart,
                       struct timespec *pEnd,
                       uint64_t *pDiff) {
    std::string buf; 

    // Container for the data we expect from the server.
    HelloReply reply;

    // Data we are sending to the server.
    HelloRequestComplex request;

    // Option 1: Get allocated child object ptr from the parent.
    HelloRequestComplex *pObjhrc = request.mutable_objhrc();

    pObjhrc->set_empid(empidManager);
    pObjhrc->set_name(userManager);
    pObjhrc->set_salary(salaryManager);

    request.set_empid(empid);
    request.set_name(user);
    request.set_salary(salary);

    // Option 2:
    // HelloRequestComplex *pObjhrc(new HelloRequestComplex());
    // pObjhrc->set_empid(empidManager);
    // pObjhrc->set_name(userManager);
    // pObjhrc->set_salary(salaryManager);
    // request.set_allocated_objhrc(&objhrc);


    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // Measuring RPC msg marshalling overhead.
    clock_gettime(CLOCK_MONOTONIC, pStart);

    request.SerializeToString(&buf);
    clock_gettime(CLOCK_MONOTONIC, pEnd);
    *pDiff = diffTime(*pStart, *pEnd);

    // The actual RPC.
    Status status = stub_->SayHelloComplex(&context, request, &reply); 
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
  int maxAttempts = 100;
  char servIPPort[25] = {0};
  int rc = 0;

  if(argc != 3)
  {
          printf("\n Usage: %s <ip of server:port> <optimization_enabled ? 1 : 0>\n", argv[0]);
          return 1;
  }
  else
  {
      printf("IP =%s\n", argv[1]);
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
    char experimentTitle[80];

    ::google::protobuf::int32 empId = 1000;
    std::string user("empName");
    double salary = 50123.56;
    // Manager data
    ::google::protobuf::int32 empIdManager = 450;
    std::string userManager("Manager");
    double salaryManager = 241041.85;



    cout << "================================================" << std::endl;
    cout << "7) Measuring GRPC Bandwidth for large packets." << std::endl;
    cout << "================================================" << std::endl;
    int diffPacketsCount = 15;
    long bytesPerPacket[] = {65500, 32*1024, 16*1024, 8*1024, 4*1024, 2*1024, 1024, 512, 255, 128, 64, 32, 16, 8, 4, 2};
    long loopCount = 1000;
    vector<string> bwTable;
    char result[512];

    unsigned long long sumBytes;

    sprintf(experimentTitle,"Part2_Q7_Bandwidth_LargeSize_OptimizationMode_%d_IP_%s",
        gccOptimizationMode, pServIPPort);

    for (int i = 0; i < diffPacketsCount; i++)
    {
        sumBytes = bytesPerPacket[1] * loopCount;
        pGreeter->RecordRoute(bytesPerPacket[i], loopCount,
                  &start, &end, &diffNanoSec);


        float sumBytesMB = (float)sumBytes * 1.0 / (1024 * 1024.0);
        float totalTimeTakeSec = (diffNanoSec  / (1024 * 1024.0 * 1024));
        float bwMBps = (float)sumBytesMB / totalTimeTakeSec;
/*
        printf("***********************************\n");
        printf(" Bandwidth Summary.\n");
        printf("***********************************\n");
        printf("\t * Total Sent Bytes =,%llu, \n", (long long unsigned int)sumBytes);
        printf("\t * Total Sent (MB) =,%f, \n", sumBytesMB);
        printf("\t * Total Time taken (nanoSec) =,%llu, \n", (long long unsigned int)diffNanoSec);
        printf("\t * Total Time taken (Sec) =,%f, \n", totalTimeTakeSec);
        printf("***********************************\n");
        printf("\t * Bandwidth (MBps) =,%f,\n", bwMBps);
        printf("***********************************\n");
*/

        sprintf(result, "* BW: Large Msg: Attempts:%d),Packet Size=,%ld, Total Sent(bytes)=,%llu,Total Sent (MB)=,%f,Time taken(nanosec)=,%llu, Time taken (seconds) = %f,Bandwidth (MBps) =,%f,\n",
                  loopCount, bytesPerPacket[i], (unsigned long long)(sumBytes),
                  sumBytesMB, (long long unsigned int)diffNanoSec, totalTimeTakeSec, bwMBps);
        bwTable.push_back(string(result));
    }
    for (int i = 0; i < diffPacketsCount; i++)
    {
      printf("%s\n",bwTable[i].c_str());
    }
    
    if (1)
      return 0;

    cout << "================================================" << std::endl;
    cout << "1.a) Measuring GRPC Marshall time for integers." << std::endl;
    cout << "================================================" << std::endl;
    sprintf(experimentTitle,"Part2_Q1_a_MarshallInt_OptimizationMode_%d_IP_%s",
        gccOptimizationMode, pServIPPort);
    for (run = 0; run < maxAttempts; run++)
    {
      std::string reply = pGreeter->SayHelloInt(empId, &start, &end, &diffNanoSec);
      monotonicClk[run] = (uint64_t) diffNanoSec;

      //std::cout << "Greeter received: " << reply << std::endl;
      printf("\t * Marshall Int: Attempts: %d), Int32 Type, Sent Msg=,%d, Received Msg=,%s, Time taken = ,%llu,nanoseconds\n",
              run, empId, reply.c_str(), 
             (long long unsigned int)diffNanoSec);
    }
    createCSVReport(experimentTitle, maxAttempts, &strMsg);


    cout << "================================================" << std::endl;
    cout << "1.b) Measuring GRPC Marshall time for Doubles." << std::endl;
    cout << "================================================" << std::endl;
    sprintf(experimentTitle,"Part2_Q1_b_MarshallDouble_OptimizationMode_%d_IP_%s",
        gccOptimizationMode, pServIPPort);
    for (run = 0; run < maxAttempts; run++)
    {
      std::string reply = pGreeter->SayHelloDouble(salary, &start, &end, &diffNanoSec);
      monotonicClk[run] = (uint64_t) diffNanoSec;

      //std::cout << "Greeter received: " << reply << std::endl;
      printf("\t * Marshall Int: Attempts: %d), Int32 Type, Sent Msg=,%f, Received Msg=,%s, Time taken = ,%llu,nanoseconds\n",
              run, salary, reply.c_str(), 
             (long long unsigned int)diffNanoSec);
    }
    createCSVReport(experimentTitle, maxAttempts, &strMsg);



    cout << "================================================" << std::endl;
    cout << "1.c) Measuring GRPC Marshall time for string." << std::endl;
    cout << "================================================" << std::endl;
    sprintf(experimentTitle,"Part2_Q1_c_MarshallString_OptimizationMode_%d_IP_%s",
        gccOptimizationMode, pServIPPort);
    for (run = 0; run < maxAttempts; run++)
    {
      std::string reply = pGreeter->SayHelloString(user, &start, &end, &diffNanoSec);
      monotonicClk[run] = (uint64_t) diffNanoSec;

      //std::cout << "Greeter received: " << reply << std::endl;
      printf("\t * Marshall String: Attempts: %d), String Type, Sent Msg=,%s, Received Msg=,%s, Time taken = ,%llu,nanoseconds\n",
              run, user.c_str(), reply.c_str(), 
             (long long unsigned int)diffNanoSec);
    }
    createCSVReport(experimentTitle, maxAttempts, &strMsg);


    cout << "================================================" << std::endl;
    cout << "1.d) Measuring GRPC Marshall time for Complex datatype." << std::endl;
    cout << "================================================" << std::endl;
    sprintf(experimentTitle,"Part2_Q1_d_MarshallComplex_OptimizationMode_%d_IP_%s",
        gccOptimizationMode, pServIPPort);
    for (run = 0; run < maxAttempts; run++)
    {
      std::string reply = pGreeter->SayHelloComplex(empId, user, salary,
            empIdManager, userManager, salaryManager,
            &start, &end, &diffNanoSec);
      monotonicClk[run] = (uint64_t) diffNanoSec;

      //std::cout << "Greeter received: " << reply << std::endl;
      printf("\t * Marshall String: Attempts: %d), String Type, Sent Msg=,%s, Received Msg=,%s, Time taken = ,%llu,nanoseconds\n",
              run, user.c_str(), reply.c_str(), 
             (long long unsigned int)diffNanoSec);
    }
    createCSVReport(experimentTitle, maxAttempts, &strMsg);



    cout << "================================================" << std::endl;
    cout << "4) Measuring GRPC roundtrip time (small msg => string msg." << std::endl;
    cout << "================================================" << std::endl;
    sprintf(experimentTitle,"Part2_Q4_RoundTripCalc_OptimizationMode_%d_SmallMsg_IP_%s",
        gccOptimizationMode, pServIPPort);
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
