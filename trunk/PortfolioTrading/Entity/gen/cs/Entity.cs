//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// Generated from: message.proto
// Note: requires additional types generated from: enum.proto
namespace entity
{
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"LegItem")]
  public partial class LegItem : global::ProtoBuf.IExtensible
  {
    public LegItem() {}
    
    private string _Symbol;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"Symbol", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string Symbol
    {
      get { return _Symbol; }
      set { _Symbol = value; }
    }
    private int _Ratio;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"Ratio", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int Ratio
    {
      get { return _Ratio; }
      set { _Ratio = value; }
    }
    private entity.PosiDirectionType _Side;
    [global::ProtoBuf.ProtoMember(3, IsRequired = true, Name=@"Side", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public entity.PosiDirectionType Side
    {
      get { return _Side; }
      set { _Side = value; }
    }
    private entity.LegStatus _Status;
    [global::ProtoBuf.ProtoMember(4, IsRequired = true, Name=@"Status", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public entity.LegStatus Status
    {
      get { return _Status; }
      set { _Status = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"PortfolioItem")]
  public partial class PortfolioItem : global::ProtoBuf.IExtensible
  {
    public PortfolioItem() {}
    
    private string _ID;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"ID", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string ID
    {
      get { return _ID; }
      set { _ID = value; }
    }
    private int _Quantity;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"Quantity", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int Quantity
    {
      get { return _Quantity; }
      set { _Quantity = value; }
    }
    private double _Diff;
    [global::ProtoBuf.ProtoMember(3, IsRequired = true, Name=@"Diff", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public double Diff
    {
      get { return _Diff; }
      set { _Diff = value; }
    }
    private bool _AutoOpen;
    [global::ProtoBuf.ProtoMember(4, IsRequired = true, Name=@"AutoOpen", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public bool AutoOpen
    {
      get { return _AutoOpen; }
      set { _AutoOpen = value; }
    }
    private bool _AutoClose;
    [global::ProtoBuf.ProtoMember(5, IsRequired = true, Name=@"AutoClose", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public bool AutoClose
    {
      get { return _AutoClose; }
      set { _AutoClose = value; }
    }
    private readonly global::System.Collections.Generic.List<entity.LegItem> _Legs = new global::System.Collections.Generic.List<entity.LegItem>();
    [global::ProtoBuf.ProtoMember(6, Name=@"Legs", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public global::System.Collections.Generic.List<entity.LegItem> Legs
    {
      get { return _Legs; }
    }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"ConnectParam")]
  public partial class ConnectParam : global::ProtoBuf.IExtensible
  {
    public ConnectParam() {}
    
    private string _QuoteAddress;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"QuoteAddress", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string QuoteAddress
    {
      get { return _QuoteAddress; }
      set { _QuoteAddress = value; }
    }
    private string _StreamFolder;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"StreamFolder", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string StreamFolder
    {
      get { return _StreamFolder; }
      set { _StreamFolder = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"OperationReturn")]
  public partial class OperationReturn : global::ProtoBuf.IExtensible
  {
    public OperationReturn() {}
    
    private bool _Success;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"Success", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public bool Success
    {
      get { return _Success; }
      set { _Success = value; }
    }
    private string _ErrorMessage;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"ErrorMessage", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string ErrorMessage
    {
      get { return _ErrorMessage; }
      set { _ErrorMessage = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"LoginParam")]
  public partial class LoginParam : global::ProtoBuf.IExtensible
  {
    public LoginParam() {}
    
    private string _BrokerId;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"BrokerId", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string BrokerId
    {
      get { return _BrokerId; }
      set { _BrokerId = value; }
    }
    private string _UserId;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"UserId", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string UserId
    {
      get { return _UserId; }
      set { _UserId = value; }
    }
    private string _Password;
    [global::ProtoBuf.ProtoMember(3, IsRequired = true, Name=@"Password", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string Password
    {
      get { return _Password; }
      set { _Password = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"RegQuoteParam")]
  public partial class RegQuoteParam : global::ProtoBuf.IExtensible
  {
    public RegQuoteParam() {}
    
    private readonly global::System.Collections.Generic.List<string> _Symbols = new global::System.Collections.Generic.List<string>();
    [global::ProtoBuf.ProtoMember(1, Name=@"Symbols", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public global::System.Collections.Generic.List<string> Symbols
    {
      get { return _Symbols; }
    }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
}