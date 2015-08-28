// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: helloworld.proto
#region Designer generated code

using System;
using System.Threading;
using System.Threading.Tasks;
using Grpc.Core;

namespace helloworld {
  public static class Greeter
  {
    static readonly string __ServiceName = "helloworld.Greeter";

    static readonly Marshaller<global::helloworld.HelloRequest> __Marshaller_HelloRequest = Marshallers.Create((arg) => arg.ToByteArray(), global::helloworld.HelloRequest.ParseFrom);
    static readonly Marshaller<global::helloworld.HelloReply> __Marshaller_HelloReply = Marshallers.Create((arg) => arg.ToByteArray(), global::helloworld.HelloReply.ParseFrom);

    static readonly Method<global::helloworld.HelloRequest, global::helloworld.HelloReply> __Method_SayHello = new Method<global::helloworld.HelloRequest, global::helloworld.HelloReply>(
        MethodType.Unary,
        "SayHello",
        __Marshaller_HelloRequest,
        __Marshaller_HelloReply);

    // client-side stub interface
    public interface IGreeterClient
    {
      global::helloworld.HelloReply SayHello(global::helloworld.HelloRequest request, CancellationToken token = default(CancellationToken));
      Task<global::helloworld.HelloReply> SayHelloAsync(global::helloworld.HelloRequest request, CancellationToken token = default(CancellationToken));
    }

    // server-side interface
    public interface IGreeter
    {
      Task<global::helloworld.HelloReply> SayHello(ServerCallContext context, global::helloworld.HelloRequest request);
    }

    // client stub
    public class GreeterClient : AbstractStub<GreeterClient, StubConfiguration>, IGreeterClient
    {
      public GreeterClient(Channel channel) : this(channel, StubConfiguration.Default)
      {
      }
      public GreeterClient(Channel channel, StubConfiguration config) : base(channel, config)
      {
      }
      public global::helloworld.HelloReply SayHello(global::helloworld.HelloRequest request, CancellationToken token = default(CancellationToken))
      {
        var call = CreateCall(__ServiceName, __Method_SayHello);
        return Calls.BlockingUnaryCall(call, request, token);
      }
      public Task<global::helloworld.HelloReply> SayHelloAsync(global::helloworld.HelloRequest request, CancellationToken token = default(CancellationToken))
      {
        var call = CreateCall(__ServiceName, __Method_SayHello);
        return Calls.AsyncUnaryCall(call, request, token);
      }
    }

    // creates service definition that can be registered with a server
    public static ServerServiceDefinition BindService(IGreeter serviceImpl)
    {
      return ServerServiceDefinition.CreateBuilder(__ServiceName)
          .AddMethod(__Method_SayHello, serviceImpl.SayHello).Build();
    }

    // creates a new client stub
    public static IGreeterClient NewStub(Channel channel)
    {
      return new GreeterClient(channel);
    }

    // creates a new client stub
    public static IGreeterClient NewStub(Channel channel, StubConfiguration config)
    {
      return new GreeterClient(channel, config);
    }
  }
}
#endregion