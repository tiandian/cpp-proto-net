// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "stdafx.h"
#include "heartbeat.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace protoc {

namespace {

const ::google::protobuf::Descriptor* Heartbeat_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Heartbeat_reflection_ = NULL;
const ::google::protobuf::Descriptor* HeartbeatAck_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  HeartbeatAck_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_heartbeat_2eproto() {
  protobuf_AddDesc_heartbeat_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "heartbeat.proto");
  GOOGLE_CHECK(file != NULL);
  Heartbeat_descriptor_ = file->message_type(0);
  static const int Heartbeat_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Heartbeat, session_),
  };
  Heartbeat_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Heartbeat_descriptor_,
      Heartbeat::default_instance_,
      Heartbeat_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Heartbeat, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Heartbeat, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Heartbeat));
  HeartbeatAck_descriptor_ = file->message_type(1);
  static const int HeartbeatAck_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HeartbeatAck, server_time_),
  };
  HeartbeatAck_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      HeartbeatAck_descriptor_,
      HeartbeatAck::default_instance_,
      HeartbeatAck_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HeartbeatAck, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HeartbeatAck, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(HeartbeatAck));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_heartbeat_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Heartbeat_descriptor_, &Heartbeat::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    HeartbeatAck_descriptor_, &HeartbeatAck::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_heartbeat_2eproto() {
  delete Heartbeat::default_instance_;
  delete Heartbeat_reflection_;
  delete HeartbeatAck::default_instance_;
  delete HeartbeatAck_reflection_;
}

void protobuf_AddDesc_heartbeat_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\017heartbeat.proto\022\006protoc\"\034\n\tHeartbeat\022\017"
    "\n\007session\030\001 \002(\t\"#\n\014HeartbeatAck\022\023\n\013serve"
    "r_time\030\001 \002(\t", 92);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "heartbeat.proto", &protobuf_RegisterTypes);
  Heartbeat::default_instance_ = new Heartbeat();
  HeartbeatAck::default_instance_ = new HeartbeatAck();
  Heartbeat::default_instance_->InitAsDefaultInstance();
  HeartbeatAck::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_heartbeat_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_heartbeat_2eproto {
  StaticDescriptorInitializer_heartbeat_2eproto() {
    protobuf_AddDesc_heartbeat_2eproto();
  }
} static_descriptor_initializer_heartbeat_2eproto_;


// ===================================================================

#ifndef _MSC_VER
const int Heartbeat::kSessionFieldNumber;
#endif  // !_MSC_VER

Heartbeat::Heartbeat()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Heartbeat::InitAsDefaultInstance() {
}

Heartbeat::Heartbeat(const Heartbeat& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Heartbeat::SharedCtor() {
  _cached_size_ = 0;
  session_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Heartbeat::~Heartbeat() {
  SharedDtor();
}

void Heartbeat::SharedDtor() {
  if (session_ != &::google::protobuf::internal::kEmptyString) {
    delete session_;
  }
  if (this != default_instance_) {
  }
}

void Heartbeat::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Heartbeat::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Heartbeat_descriptor_;
}

const Heartbeat& Heartbeat::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_heartbeat_2eproto();  return *default_instance_;
}

Heartbeat* Heartbeat::default_instance_ = NULL;

Heartbeat* Heartbeat::New() const {
  return new Heartbeat;
}

void Heartbeat::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_session()) {
      if (session_ != &::google::protobuf::internal::kEmptyString) {
        session_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Heartbeat::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string session = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_session()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->session().data(), this->session().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Heartbeat::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string session = 1;
  if (has_session()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->session().data(), this->session().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->session(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Heartbeat::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string session = 1;
  if (has_session()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->session().data(), this->session().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->session(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Heartbeat::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string session = 1;
    if (has_session()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->session());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Heartbeat::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Heartbeat* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Heartbeat*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Heartbeat::MergeFrom(const Heartbeat& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_session()) {
      set_session(from.session());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Heartbeat::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Heartbeat::CopyFrom(const Heartbeat& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Heartbeat::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  return true;
}

void Heartbeat::Swap(Heartbeat* other) {
  if (other != this) {
    std::swap(session_, other->session_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Heartbeat::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Heartbeat_descriptor_;
  metadata.reflection = Heartbeat_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int HeartbeatAck::kServerTimeFieldNumber;
#endif  // !_MSC_VER

HeartbeatAck::HeartbeatAck()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void HeartbeatAck::InitAsDefaultInstance() {
}

HeartbeatAck::HeartbeatAck(const HeartbeatAck& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void HeartbeatAck::SharedCtor() {
  _cached_size_ = 0;
  server_time_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

HeartbeatAck::~HeartbeatAck() {
  SharedDtor();
}

void HeartbeatAck::SharedDtor() {
  if (server_time_ != &::google::protobuf::internal::kEmptyString) {
    delete server_time_;
  }
  if (this != default_instance_) {
  }
}

void HeartbeatAck::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* HeartbeatAck::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return HeartbeatAck_descriptor_;
}

const HeartbeatAck& HeartbeatAck::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_heartbeat_2eproto();  return *default_instance_;
}

HeartbeatAck* HeartbeatAck::default_instance_ = NULL;

HeartbeatAck* HeartbeatAck::New() const {
  return new HeartbeatAck;
}

void HeartbeatAck::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_server_time()) {
      if (server_time_ != &::google::protobuf::internal::kEmptyString) {
        server_time_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool HeartbeatAck::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string server_time = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_server_time()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->server_time().data(), this->server_time().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void HeartbeatAck::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string server_time = 1;
  if (has_server_time()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->server_time().data(), this->server_time().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->server_time(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* HeartbeatAck::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string server_time = 1;
  if (has_server_time()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->server_time().data(), this->server_time().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->server_time(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int HeartbeatAck::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string server_time = 1;
    if (has_server_time()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->server_time());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void HeartbeatAck::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const HeartbeatAck* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const HeartbeatAck*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void HeartbeatAck::MergeFrom(const HeartbeatAck& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_server_time()) {
      set_server_time(from.server_time());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void HeartbeatAck::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void HeartbeatAck::CopyFrom(const HeartbeatAck& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool HeartbeatAck::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  return true;
}

void HeartbeatAck::Swap(HeartbeatAck* other) {
  if (other != this) {
    std::swap(server_time_, other->server_time_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata HeartbeatAck::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = HeartbeatAck_descriptor_;
  metadata.reflection = HeartbeatAck_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace protoc

// @@protoc_insertion_point(global_scope)