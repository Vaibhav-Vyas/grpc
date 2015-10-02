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

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::HelloRequestInt;
using helloworld::HelloRequestDouble;
using helloworld::HelloRequestComplex;
using helloworld::Greeter;

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name()); 
    return Status::OK;
  }

  Status RecordRoute(ServerContext* context, ServerReader<HelloRequest>* reader,
                     HelloReply* summary) override {
    HelloRequest point;
    int point_count = 0;
    long bytes_recv = 0;

    std::cout <<"Before Stream" << point_count;    

    while (reader->Read(&point)) {
      point_count++;
      
      bytes_recv += strlen(point.name().c_str());
      std::cout <<"    Stream " << point_count;    
    }

    std::string result;
    result ="Total = ";
    result += static_cast<long>(point_count);
    summary->set_message(std::string("Inside Stream: Received Bytes = " + result));

    return Status::OK;
  }

  Status SayHelloInt(ServerContext* context, const HelloRequestInt* request,
                  HelloReply* reply) override {
    std::string prefix("Int= ");
    //std::string buf;
    //google::protobuf::TextFormat::PrintToString(request, &buf);

    std::cout << "Inside" << prefix;
    
    reply->set_message(prefix); // + buf); // + Integer.toString(request->empid()));
    return Status::OK;
  }

    Status SayHelloDouble(ServerContext* context, const HelloRequestDouble* request,
                  HelloReply* reply) override {
    std::string prefix("Double= ");
    std::cout << "Inside" << prefix;

    reply->set_message(prefix); // + Integer.toString(request->empid()));
    return Status::OK;
  }

   Status SayHelloComplex(ServerContext* context, const HelloRequestComplex* request,
                  HelloReply* reply) override {
    std::string prefix("Complex= ");
    std::cout << "Inside" << prefix;

    reply->set_message(prefix); // + Integer.toString(request->empid()));
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
