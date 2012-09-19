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
// Note: requires additional types generated from: trade.proto
namespace entity
{
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"StringParam")]
  public partial class StringParam : global::ProtoBuf.IExtensible
  {
    public StringParam() {}
    
    private string _Data;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"Data", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string Data
    {
      get { return _Data; }
      set { _Data = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"IntParam")]
  public partial class IntParam : global::ProtoBuf.IExtensible
  {
    public IntParam() {}
    
    private int _Data;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"Data", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int Data
    {
      get { return _Data; }
      set { _Data = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
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
    private double _Last;
    [global::ProtoBuf.ProtoMember(5, IsRequired = true, Name=@"Last", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public double Last
    {
      get { return _Last; }
      set { _Last = value; }
    }
    private double _Ask;
    [global::ProtoBuf.ProtoMember(6, IsRequired = true, Name=@"Ask", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public double Ask
    {
      get { return _Ask; }
      set { _Ask = value; }
    }
    private double _Bid;
    [global::ProtoBuf.ProtoMember(7, IsRequired = true, Name=@"Bid", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public double Bid
    {
      get { return _Bid; }
      set { _Bid = value; }
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
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"PorfOpenPosiParam")]
  public partial class PorfOpenPosiParam : global::ProtoBuf.IExtensible
  {
    public PorfOpenPosiParam() {}
    
    private string _PortfId;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"PortfId", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string PortfId
    {
      get { return _PortfId; }
      set { _PortfId = value; }
    }
    private int _Quantity;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"Quantity", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int Quantity
    {
      get { return _Quantity; }
      set { _Quantity = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"ClosePositionParam")]
  public partial class ClosePositionParam : global::ProtoBuf.IExtensible
  {
    public ClosePositionParam() {}
    
    private trade.MultiLegOrder _MultiLegOrder;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"MultiLegOrder", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public trade.MultiLegOrder MultiLegOrder
    {
      get { return _MultiLegOrder; }
      set { _MultiLegOrder = value; }
    }

    private string _LegOrdRef = "";
    [global::ProtoBuf.ProtoMember(2, IsRequired = false, Name=@"LegOrdRef", DataFormat = global::ProtoBuf.DataFormat.Default)]
    [global::System.ComponentModel.DefaultValue("")]
    public string LegOrdRef
    {
      get { return _LegOrdRef; }
      set { _LegOrdRef = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"LegOrderUpdateParam")]
  public partial class LegOrderUpdateParam : global::ProtoBuf.IExtensible
  {
    public LegOrderUpdateParam() {}
    
    private string _PortfId;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"PortfId", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string PortfId
    {
      get { return _PortfId; }
      set { _PortfId = value; }
    }
    private string _MultiLegOrderId;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"MultiLegOrderId", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string MultiLegOrderId
    {
      get { return _MultiLegOrderId; }
      set { _MultiLegOrderId = value; }
    }
    private string _LegOrderRef;
    [global::ProtoBuf.ProtoMember(3, IsRequired = true, Name=@"LegOrderRef", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string LegOrderRef
    {
      get { return _LegOrderRef; }
      set { _LegOrderRef = value; }
    }
    private trade.Order _LegOrder;
    [global::ProtoBuf.ProtoMember(4, IsRequired = true, Name=@"LegOrder", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public trade.Order LegOrder
    {
      get { return _LegOrder; }
      set { _LegOrder = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"ModifyStrategyParam")]
  public partial class ModifyStrategyParam : global::ProtoBuf.IExtensible
  {
    public ModifyStrategyParam() {}
    
    private string _PortfId;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"PortfId", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string PortfId
    {
      get { return _PortfId; }
      set { _PortfId = value; }
    }
    private bool _Enabled;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"Enabled", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public bool Enabled
    {
      get { return _Enabled; }
      set { _Enabled = value; }
    }
    private string _IsAutoOpen;
    [global::ProtoBuf.ProtoMember(3, IsRequired = true, Name=@"IsAutoOpen", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string IsAutoOpen
    {
      get { return _IsAutoOpen; }
      set { _IsAutoOpen = value; }
    }
    private string _IsAutoClose;
    [global::ProtoBuf.ProtoMember(4, IsRequired = true, Name=@"IsAutoClose", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string IsAutoClose
    {
      get { return _IsAutoClose; }
      set { _IsAutoClose = value; }
    }
    private entity.PosiDirectionType _Side;
    [global::ProtoBuf.ProtoMember(5, IsRequired = true, Name=@"Side", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public entity.PosiDirectionType Side
    {
      get { return _Side; }
      set { _Side = value; }
    }

    private double _OpenPosiThreshold = default(double);
    [global::ProtoBuf.ProtoMember(6, IsRequired = false, Name=@"OpenPosiThreshold", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(double))]
    public double OpenPosiThreshold
    {
      get { return _OpenPosiThreshold; }
      set { _OpenPosiThreshold = value; }
    }

    private double _StopGainThreshold = default(double);
    [global::ProtoBuf.ProtoMember(7, IsRequired = false, Name=@"StopGainThreshold", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(double))]
    public double StopGainThreshold
    {
      get { return _StopGainThreshold; }
      set { _StopGainThreshold = value; }
    }

    private double _StopLossThreshold = default(double);
    [global::ProtoBuf.ProtoMember(8, IsRequired = false, Name=@"StopLossThreshold", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(double))]
    public double StopLossThreshold
    {
      get { return _StopLossThreshold; }
      set { _StopLossThreshold = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
}