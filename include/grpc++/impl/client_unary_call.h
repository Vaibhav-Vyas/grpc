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

#ifndef GRPCXX_IMPL_CLIENT_UNARY_CALL_H
#define GRPCXX_IMPL_CLIENT_UNARY_CALL_H

#include <grpc++/impl/call.h>
#include <grpc++/support/config.h>
#include <grpc++/support/status.h>
#include <iostream>
#include <time.h>       /* timer */

#define GRPC_PROFILE_DEBUG_MODE     1

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

std::vector<FuncStats> funcProfiler;

int add_func_stats(std::string func_name, uint64_t start_ns, uint64_t end_nsec, std::string file_name, std::string desc)
{
    FuncStats currFuncStat(func_name, start_ns, end_nsec, file_name, desc);

    if (GRPC_PROFILE_DEBUG_MODE)
    {
        std::cout << func_name << ", Start:" << currFuncStat.start_ns << " ns,"  \
            << ", Duration:" << currFuncStat.duration_ns << " ns,"          \
            << ", Description:" << currFuncStat.description << ","          \
            << ", FileName:" << currFuncStat.fileName << std::endl;
    }
    funcProfiler.push_back(currFuncStat);
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

//to get the time
uint64_t nanos_since_midnight()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

namespace grpc {

class Channel;
class ClientContext;
class CompletionQueue;
class RpcMethod;

uint64_t start, end;

// Wrapper that performs a blocking unary call
template <class InputMessage, class OutputMessage>
Status BlockingUnaryCall(Channel* channel, const RpcMethod& method,
                         ClientContext* context, const InputMessage& request,
                         OutputMessage* result) {
  CompletionQueue cq;
  Call call(channel->CreateCall(method, context, &cq));
  CallOpSet<CallOpSendInitialMetadata, CallOpSendMessage,
            CallOpRecvInitialMetadata, CallOpRecvMessage<OutputMessage>,
            CallOpClientSendClose, CallOpClientRecvStatus> ops;
  Status status = ops.SendMessage(request);
  if (!status.ok()) {
    return status;
  }

  start = nanos_since_midnight();
  ops.SendInitialMetadata(context->send_initial_metadata_);
  end = nanos_since_midnight();
  add_func_stats(" ops.SendInitialMetadata", start, end, std::string(__FILE__), "ops.SendInitialMetadata  ");

  start = nanos_since_midnight();
  ops.RecvInitialMetadata(context);
  end = nanos_since_midnight();
  add_func_stats(" ops.RecvInitialMetadata", start, end, std::string(__FILE__), "ops.RecvInitialMetadata  ");

  start = nanos_since_midnight();
  ops.RecvMessage(result);
  end = nanos_since_midnight();
  add_func_stats(" ops.RecvMessage", start, end, std::string(__FILE__), "ops.RecvMessage  ");

  start = nanos_since_midnight();
  ops.ClientSendClose();
  end = nanos_since_midnight();
  add_func_stats(" ops.ClientSendClose", start, end, std::string(__FILE__), "ops.ClientSendClose  ");

  start = nanos_since_midnight();
  ops.ClientRecvStatus(context, &status);
  end = nanos_since_midnight();
  add_func_stats(" ops.ClientRecvStatus", start, end, std::string(__FILE__), "ops.ClientRecvStatus  ");

  start = nanos_since_midnight();
  call.PerformOps(&ops);
  end = nanos_since_midnight();
  add_func_stats(" ops.PerformOps", start, end, std::string(__FILE__), "call.PerformOps  ");

  start = nanos_since_midnight();
  GPR_ASSERT((cq.Pluck(&ops) && ops.got_message) || !status.ok());
  end = nanos_since_midnight();
  add_func_stats(" cq.Pluck(&ops)", start, end, std::string(__FILE__), "cq.Pluck(&ops)  ");

  return status;
}

}  // namespace grpc

#endif  // GRPCXX_IMPL_CLIENT_UNARY_CALL_H
