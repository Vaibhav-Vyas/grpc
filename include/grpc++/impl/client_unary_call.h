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
#include <grpc++/impl/grpc_profiler.h>
#include <iostream>


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
  // Profile function
  start = nanos_since_midnight();
  CompletionQueue cq;
  Call call(channel->CreateCall(method, context, &cq));
  end = nanos_since_midnight();
  add_func_stats("Call call(channel->CreateCall(method; context; &cq))", start, end, std::string(__FILE__), "Initializes obj to create a new call to given method.");

  // Profile function
  start = nanos_since_midnight();
  CallOpSet<CallOpSendInitialMetadata, CallOpSendMessage,
            CallOpRecvInitialMetadata, CallOpRecvMessage<OutputMessage>,
            CallOpClientSendClose, CallOpClientRecvStatus> ops;
  end = nanos_since_midnight();
  add_func_stats("CallOpSet<SendInitMetadata;SendMsg; RecvInitMetadata;RecvMsg;ClientSendClose; ClientRecvStatus>", start, end, std::string(__FILE__), "Initializes 6 template classes. Primary implementation of CallOpSetInterface.");

  // Profile function
  start = nanos_since_midnight();
  Status status = ops.SendMessage(request);
  if (!status.ok()) {
    return status;
  }
  end = nanos_since_midnight();
  add_func_stats("ops.SendMessage(request);", start, end, std::string(__FILE__), "Serializes the message and stores in local send buffer.");

  start = nanos_since_midnight();
  ops.SendInitialMetadata(context->send_initial_metadata_);
  end = nanos_since_midnight();
  add_func_stats("ops.SendInitialMetadata", start, end, std::string(__FILE__), "ops.SendInitialMetadata  ");

  start = nanos_since_midnight();
  ops.RecvInitialMetadata(context);
  end = nanos_since_midnight();
  add_func_stats("ops.RecvInitialMetadata", start, end, std::string(__FILE__), "ops.RecvInitialMetadata  ");

  start = nanos_since_midnight();
  ops.RecvMessage(result);
  end = nanos_since_midnight();
  add_func_stats("ops.RecvMessage", start, end, std::string(__FILE__), "ops.RecvMessage  ");

  start = nanos_since_midnight();
  ops.ClientSendClose();
  end = nanos_since_midnight();
  add_func_stats("ops.ClientSendClose", start, end, std::string(__FILE__), "ops.ClientSendClose  ");

  start = nanos_since_midnight();
  ops.ClientRecvStatus(context, &status);
  end = nanos_since_midnight();
  add_func_stats("ops.ClientRecvStatus", start, end, std::string(__FILE__), "ops.ClientRecvStatus  ");

  start = nanos_since_midnight();
  call.PerformOps(&ops);
  end = nanos_since_midnight();
  add_func_stats("ops.PerformOps", start, end, std::string(__FILE__), "call.PerformOps  ");

  start = nanos_since_midnight();
  GPR_ASSERT((cq.Pluck(&ops) && ops.got_message) || !status.ok());
  end = nanos_since_midnight();
  (" cq.Pluck(&ops)", start, end, std::string(__FILE__), "cq.Pluck(&ops)  ");

  return status;
}

}  // namespace grpc

#endif  // GRPCXX_IMPL_CLIENT_UNARY_CALL_H
