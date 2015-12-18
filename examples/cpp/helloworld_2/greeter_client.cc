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
#include <time.h>       /* timer */


#include <grpc++/grpc++.h>

#include "helloworld.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

extern void print_all_profile_stats();
extern uint64_t nanos_since_midnight();
extern int add_func_stats(std::string funcName, uint64_t start_ns, uint64_t end_nsec, std::string fName = "", std::string desc = "");


class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

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

//to get the time
/*
uint64_t nanos_since_midnight()
{

        // current time
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

        // get midnight
        time_t tnow = std::chrono::system_clock::to_time_t(now);
        tm *date = std::localtime(&tnow);
        date->tm_hour = 0;
        date->tm_min = 0;
        date->tm_sec = 0;
        auto midnight = std::chrono::system_clock::from_time_t(std::mktime(date));

        // number of nanoseconds between midnight and now, ie current time in nanos
        // The same technique can be used for time since epoch
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now - midnight).count();
}
*/

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureCredentials()).

	uint64_t start, end, start0, end0;

	 start0 = nanos_since_midnight();
	 start = nanos_since_midnight();
	  end = nanos_since_midnight();
	  end0 = nanos_since_midnight();
	  std::cout << "Client setup " << end - start << " ns" << std::endl;

	  std::cout << "Client setup0 " << end0 - start0 << " ns" << std::endl;




  start = nanos_since_midnight();
  GreeterClient greeter(
      grpc::CreateChannel("localhost:50051", grpc::InsecureCredentials()));
  end = nanos_since_midnight();
  std::cout << "Client setup " << end - start << " ns" << std::endl;

  std::string user("world");

  start = nanos_since_midnight();
  std::string reply = greeter.SayHello(user);
  end = nanos_since_midnight();
  std::cout << "The total time is " << end - start << " ns" << std::endl;

  std::cout << "Greeter received: " << reply << std::endl;

/*
  start = nanos_since_midnight();
  reply = greeter.SayHello(user);
  end = nanos_since_midnight();
  std::cout << "The total time is (second measurement) " << end - start << " ns" << std::endl;

  std::cout << "Greeter received: " << reply << std::endl;

  start = nanos_since_midnight();
  reply = greeter.SayHello(user);
  end = nanos_since_midnight();
  std::cout << "The total time is (third measurement) " << end - start << " ns" << std::endl;

  std::cout << "Greeter received: " << reply << std::endl;


  start = nanos_since_midnight();
  reply = greeter.SayHello(user);
  end = nanos_since_midnight();
  std::cout << "The total time is (forth measurement) " << end - start << " ns" << std::endl;

  std::cout << "Greeter received: " << reply << std::endl;
*/

  print_all_profile_stats();

  return 0;
}
