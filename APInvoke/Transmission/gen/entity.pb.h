// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: entity.proto

#ifndef PROTOBUF_entity_2eproto__INCLUDED
#define PROTOBUF_entity_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
// @@protoc_insertion_point(includes)

namespace Entity {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_entity_2eproto();
void protobuf_AssignDesc_entity_2eproto();
void protobuf_ShutdownFile_entity_2eproto();

class TestHelloParams;
class TestHelloReturn;
class TestCallbackParams;
class TestCallbackReturn;

// ===================================================================

class TestHelloParams : public ::google::protobuf::Message {
 public:
  TestHelloParams();
  virtual ~TestHelloParams();
  
  TestHelloParams(const TestHelloParams& from);
  
  inline TestHelloParams& operator=(const TestHelloParams& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const TestHelloParams& default_instance();
  
  void Swap(TestHelloParams* other);
  
  // implements Message ----------------------------------------------
  
  TestHelloParams* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const TestHelloParams& from);
  void MergeFrom(const TestHelloParams& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required string param1 = 1;
  inline bool has_param1() const;
  inline void clear_param1();
  static const int kParam1FieldNumber = 1;
  inline const ::std::string& param1() const;
  inline void set_param1(const ::std::string& value);
  inline void set_param1(const char* value);
  inline void set_param1(const char* value, size_t size);
  inline ::std::string* mutable_param1();
  inline ::std::string* release_param1();
  
  // required string param2 = 2;
  inline bool has_param2() const;
  inline void clear_param2();
  static const int kParam2FieldNumber = 2;
  inline const ::std::string& param2() const;
  inline void set_param2(const ::std::string& value);
  inline void set_param2(const char* value);
  inline void set_param2(const char* value, size_t size);
  inline ::std::string* mutable_param2();
  inline ::std::string* release_param2();
  
  // @@protoc_insertion_point(class_scope:Entity.TestHelloParams)
 private:
  inline void set_has_param1();
  inline void clear_has_param1();
  inline void set_has_param2();
  inline void clear_has_param2();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* param1_;
  ::std::string* param2_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  friend void  protobuf_AddDesc_entity_2eproto();
  friend void protobuf_AssignDesc_entity_2eproto();
  friend void protobuf_ShutdownFile_entity_2eproto();
  
  void InitAsDefaultInstance();
  static TestHelloParams* default_instance_;
};
// -------------------------------------------------------------------

class TestHelloReturn : public ::google::protobuf::Message {
 public:
  TestHelloReturn();
  virtual ~TestHelloReturn();
  
  TestHelloReturn(const TestHelloReturn& from);
  
  inline TestHelloReturn& operator=(const TestHelloReturn& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const TestHelloReturn& default_instance();
  
  void Swap(TestHelloReturn* other);
  
  // implements Message ----------------------------------------------
  
  TestHelloReturn* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const TestHelloReturn& from);
  void MergeFrom(const TestHelloReturn& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required bool success = 1;
  inline bool has_success() const;
  inline void clear_success();
  static const int kSuccessFieldNumber = 1;
  inline bool success() const;
  inline void set_success(bool value);
  
  // @@protoc_insertion_point(class_scope:Entity.TestHelloReturn)
 private:
  inline void set_has_success();
  inline void clear_has_success();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  bool success_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_entity_2eproto();
  friend void protobuf_AssignDesc_entity_2eproto();
  friend void protobuf_ShutdownFile_entity_2eproto();
  
  void InitAsDefaultInstance();
  static TestHelloReturn* default_instance_;
};
// -------------------------------------------------------------------

class TestCallbackParams : public ::google::protobuf::Message {
 public:
  TestCallbackParams();
  virtual ~TestCallbackParams();
  
  TestCallbackParams(const TestCallbackParams& from);
  
  inline TestCallbackParams& operator=(const TestCallbackParams& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const TestCallbackParams& default_instance();
  
  void Swap(TestCallbackParams* other);
  
  // implements Message ----------------------------------------------
  
  TestCallbackParams* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const TestCallbackParams& from);
  void MergeFrom(const TestCallbackParams& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required string param3 = 1;
  inline bool has_param3() const;
  inline void clear_param3();
  static const int kParam3FieldNumber = 1;
  inline const ::std::string& param3() const;
  inline void set_param3(const ::std::string& value);
  inline void set_param3(const char* value);
  inline void set_param3(const char* value, size_t size);
  inline ::std::string* mutable_param3();
  inline ::std::string* release_param3();
  
  // required string param4 = 2;
  inline bool has_param4() const;
  inline void clear_param4();
  static const int kParam4FieldNumber = 2;
  inline const ::std::string& param4() const;
  inline void set_param4(const ::std::string& value);
  inline void set_param4(const char* value);
  inline void set_param4(const char* value, size_t size);
  inline ::std::string* mutable_param4();
  inline ::std::string* release_param4();
  
  // @@protoc_insertion_point(class_scope:Entity.TestCallbackParams)
 private:
  inline void set_has_param3();
  inline void clear_has_param3();
  inline void set_has_param4();
  inline void clear_has_param4();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* param3_;
  ::std::string* param4_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  friend void  protobuf_AddDesc_entity_2eproto();
  friend void protobuf_AssignDesc_entity_2eproto();
  friend void protobuf_ShutdownFile_entity_2eproto();
  
  void InitAsDefaultInstance();
  static TestCallbackParams* default_instance_;
};
// -------------------------------------------------------------------

class TestCallbackReturn : public ::google::protobuf::Message {
 public:
  TestCallbackReturn();
  virtual ~TestCallbackReturn();
  
  TestCallbackReturn(const TestCallbackReturn& from);
  
  inline TestCallbackReturn& operator=(const TestCallbackReturn& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const TestCallbackReturn& default_instance();
  
  void Swap(TestCallbackReturn* other);
  
  // implements Message ----------------------------------------------
  
  TestCallbackReturn* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const TestCallbackReturn& from);
  void MergeFrom(const TestCallbackReturn& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required int32 RetVal = 1;
  inline bool has_retval() const;
  inline void clear_retval();
  static const int kRetValFieldNumber = 1;
  inline ::google::protobuf::int32 retval() const;
  inline void set_retval(::google::protobuf::int32 value);
  
  // @@protoc_insertion_point(class_scope:Entity.TestCallbackReturn)
 private:
  inline void set_has_retval();
  inline void clear_has_retval();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::int32 retval_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_entity_2eproto();
  friend void protobuf_AssignDesc_entity_2eproto();
  friend void protobuf_ShutdownFile_entity_2eproto();
  
  void InitAsDefaultInstance();
  static TestCallbackReturn* default_instance_;
};
// ===================================================================


// ===================================================================

// TestHelloParams

// required string param1 = 1;
inline bool TestHelloParams::has_param1() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void TestHelloParams::set_has_param1() {
  _has_bits_[0] |= 0x00000001u;
}
inline void TestHelloParams::clear_has_param1() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void TestHelloParams::clear_param1() {
  if (param1_ != &::google::protobuf::internal::kEmptyString) {
    param1_->clear();
  }
  clear_has_param1();
}
inline const ::std::string& TestHelloParams::param1() const {
  return *param1_;
}
inline void TestHelloParams::set_param1(const ::std::string& value) {
  set_has_param1();
  if (param1_ == &::google::protobuf::internal::kEmptyString) {
    param1_ = new ::std::string;
  }
  param1_->assign(value);
}
inline void TestHelloParams::set_param1(const char* value) {
  set_has_param1();
  if (param1_ == &::google::protobuf::internal::kEmptyString) {
    param1_ = new ::std::string;
  }
  param1_->assign(value);
}
inline void TestHelloParams::set_param1(const char* value, size_t size) {
  set_has_param1();
  if (param1_ == &::google::protobuf::internal::kEmptyString) {
    param1_ = new ::std::string;
  }
  param1_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* TestHelloParams::mutable_param1() {
  set_has_param1();
  if (param1_ == &::google::protobuf::internal::kEmptyString) {
    param1_ = new ::std::string;
  }
  return param1_;
}
inline ::std::string* TestHelloParams::release_param1() {
  clear_has_param1();
  if (param1_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = param1_;
    param1_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// required string param2 = 2;
inline bool TestHelloParams::has_param2() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void TestHelloParams::set_has_param2() {
  _has_bits_[0] |= 0x00000002u;
}
inline void TestHelloParams::clear_has_param2() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void TestHelloParams::clear_param2() {
  if (param2_ != &::google::protobuf::internal::kEmptyString) {
    param2_->clear();
  }
  clear_has_param2();
}
inline const ::std::string& TestHelloParams::param2() const {
  return *param2_;
}
inline void TestHelloParams::set_param2(const ::std::string& value) {
  set_has_param2();
  if (param2_ == &::google::protobuf::internal::kEmptyString) {
    param2_ = new ::std::string;
  }
  param2_->assign(value);
}
inline void TestHelloParams::set_param2(const char* value) {
  set_has_param2();
  if (param2_ == &::google::protobuf::internal::kEmptyString) {
    param2_ = new ::std::string;
  }
  param2_->assign(value);
}
inline void TestHelloParams::set_param2(const char* value, size_t size) {
  set_has_param2();
  if (param2_ == &::google::protobuf::internal::kEmptyString) {
    param2_ = new ::std::string;
  }
  param2_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* TestHelloParams::mutable_param2() {
  set_has_param2();
  if (param2_ == &::google::protobuf::internal::kEmptyString) {
    param2_ = new ::std::string;
  }
  return param2_;
}
inline ::std::string* TestHelloParams::release_param2() {
  clear_has_param2();
  if (param2_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = param2_;
    param2_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// -------------------------------------------------------------------

// TestHelloReturn

// required bool success = 1;
inline bool TestHelloReturn::has_success() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void TestHelloReturn::set_has_success() {
  _has_bits_[0] |= 0x00000001u;
}
inline void TestHelloReturn::clear_has_success() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void TestHelloReturn::clear_success() {
  success_ = false;
  clear_has_success();
}
inline bool TestHelloReturn::success() const {
  return success_;
}
inline void TestHelloReturn::set_success(bool value) {
  set_has_success();
  success_ = value;
}

// -------------------------------------------------------------------

// TestCallbackParams

// required string param3 = 1;
inline bool TestCallbackParams::has_param3() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void TestCallbackParams::set_has_param3() {
  _has_bits_[0] |= 0x00000001u;
}
inline void TestCallbackParams::clear_has_param3() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void TestCallbackParams::clear_param3() {
  if (param3_ != &::google::protobuf::internal::kEmptyString) {
    param3_->clear();
  }
  clear_has_param3();
}
inline const ::std::string& TestCallbackParams::param3() const {
  return *param3_;
}
inline void TestCallbackParams::set_param3(const ::std::string& value) {
  set_has_param3();
  if (param3_ == &::google::protobuf::internal::kEmptyString) {
    param3_ = new ::std::string;
  }
  param3_->assign(value);
}
inline void TestCallbackParams::set_param3(const char* value) {
  set_has_param3();
  if (param3_ == &::google::protobuf::internal::kEmptyString) {
    param3_ = new ::std::string;
  }
  param3_->assign(value);
}
inline void TestCallbackParams::set_param3(const char* value, size_t size) {
  set_has_param3();
  if (param3_ == &::google::protobuf::internal::kEmptyString) {
    param3_ = new ::std::string;
  }
  param3_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* TestCallbackParams::mutable_param3() {
  set_has_param3();
  if (param3_ == &::google::protobuf::internal::kEmptyString) {
    param3_ = new ::std::string;
  }
  return param3_;
}
inline ::std::string* TestCallbackParams::release_param3() {
  clear_has_param3();
  if (param3_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = param3_;
    param3_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// required string param4 = 2;
inline bool TestCallbackParams::has_param4() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void TestCallbackParams::set_has_param4() {
  _has_bits_[0] |= 0x00000002u;
}
inline void TestCallbackParams::clear_has_param4() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void TestCallbackParams::clear_param4() {
  if (param4_ != &::google::protobuf::internal::kEmptyString) {
    param4_->clear();
  }
  clear_has_param4();
}
inline const ::std::string& TestCallbackParams::param4() const {
  return *param4_;
}
inline void TestCallbackParams::set_param4(const ::std::string& value) {
  set_has_param4();
  if (param4_ == &::google::protobuf::internal::kEmptyString) {
    param4_ = new ::std::string;
  }
  param4_->assign(value);
}
inline void TestCallbackParams::set_param4(const char* value) {
  set_has_param4();
  if (param4_ == &::google::protobuf::internal::kEmptyString) {
    param4_ = new ::std::string;
  }
  param4_->assign(value);
}
inline void TestCallbackParams::set_param4(const char* value, size_t size) {
  set_has_param4();
  if (param4_ == &::google::protobuf::internal::kEmptyString) {
    param4_ = new ::std::string;
  }
  param4_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* TestCallbackParams::mutable_param4() {
  set_has_param4();
  if (param4_ == &::google::protobuf::internal::kEmptyString) {
    param4_ = new ::std::string;
  }
  return param4_;
}
inline ::std::string* TestCallbackParams::release_param4() {
  clear_has_param4();
  if (param4_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = param4_;
    param4_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// -------------------------------------------------------------------

// TestCallbackReturn

// required int32 RetVal = 1;
inline bool TestCallbackReturn::has_retval() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void TestCallbackReturn::set_has_retval() {
  _has_bits_[0] |= 0x00000001u;
}
inline void TestCallbackReturn::clear_has_retval() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void TestCallbackReturn::clear_retval() {
  retval_ = 0;
  clear_has_retval();
}
inline ::google::protobuf::int32 TestCallbackReturn::retval() const {
  return retval_;
}
inline void TestCallbackReturn::set_retval(::google::protobuf::int32 value) {
  set_has_retval();
  retval_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Entity

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_entity_2eproto__INCLUDED