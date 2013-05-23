//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// Generated from: packet.proto
namespace AP
{
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"Connect")]
  public partial class Connect : global::ProtoBuf.IExtensible
  {
    public Connect() {}
    
    private string _userid;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"userid", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string userid
    {
      get { return _userid; }
      set { _userid = value; }
    }
    private string _password;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"password", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string password
    {
      get { return _password; }
      set { _password = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"ConnectAck")]
  public partial class ConnectAck : global::ProtoBuf.IExtensible
  {
    public ConnectAck() {}
    
    private bool _success;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"success", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public bool success
    {
      get { return _success; }
      set { _success = value; }
    }
    private string _session;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"session", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string session
    {
      get { return _session; }
      set { _session = value; }
    }
    private bool _attach_existing;
    [global::ProtoBuf.ProtoMember(3, IsRequired = true, Name=@"attach_existing", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public bool attach_existing
    {
      get { return _attach_existing; }
      set { _attach_existing = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"Request")]
  public partial class Request : global::ProtoBuf.IExtensible
  {
    public Request() {}
    
    private string _session;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"session", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string session
    {
      get { return _session; }
      set { _session = value; }
    }
    private string _method;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"method", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string method
    {
      get { return _method; }
      set { _method = value; }
    }
    private byte[] _param_data;
    [global::ProtoBuf.ProtoMember(3, IsRequired = true, Name=@"param_data", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public byte[] param_data
    {
      get { return _param_data; }
      set { _param_data = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"Response")]
  public partial class Response : global::ProtoBuf.IExtensible
  {
    public Response() {}
    
    private string _method;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"method", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string method
    {
      get { return _method; }
      set { _method = value; }
    }
    private byte[] _return_data;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"return_data", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public byte[] return_data
    {
      get { return _return_data; }
      set { _return_data = value; }
    }
    private bool _invoke;
    [global::ProtoBuf.ProtoMember(3, IsRequired = true, Name=@"invoke", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public bool invoke
    {
      get { return _invoke; }
      set { _invoke = value; }
    }
    private string _error;
    [global::ProtoBuf.ProtoMember(4, IsRequired = true, Name=@"error", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string error
    {
      get { return _error; }
      set { _error = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"CallbackReq")]
  public partial class CallbackReq : global::ProtoBuf.IExtensible
  {
    public CallbackReq() {}
    
    private string _method;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"method", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string method
    {
      get { return _method; }
      set { _method = value; }
    }
    private byte[] _param_data;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"param_data", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public byte[] param_data
    {
      get { return _param_data; }
      set { _param_data = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"CallbackRsp")]
  public partial class CallbackRsp : global::ProtoBuf.IExtensible
  {
    public CallbackRsp() {}
    
    private string _session;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"session", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string session
    {
      get { return _session; }
      set { _session = value; }
    }
    private string _method;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"method", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string method
    {
      get { return _method; }
      set { _method = value; }
    }
    private byte[] _return_data;
    [global::ProtoBuf.ProtoMember(3, IsRequired = true, Name=@"return_data", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public byte[] return_data
    {
      get { return _return_data; }
      set { _return_data = value; }
    }
    private bool _invoke;
    [global::ProtoBuf.ProtoMember(4, IsRequired = true, Name=@"invoke", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public bool invoke
    {
      get { return _invoke; }
      set { _invoke = value; }
    }
    private string _error;
    [global::ProtoBuf.ProtoMember(5, IsRequired = true, Name=@"error", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string error
    {
      get { return _error; }
      set { _error = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"Heartbeat")]
  public partial class Heartbeat : global::ProtoBuf.IExtensible
  {
    public Heartbeat() {}
    
    private int _timestamp;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"timestamp", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int timestamp
    {
      get { return _timestamp; }
      set { _timestamp = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
}