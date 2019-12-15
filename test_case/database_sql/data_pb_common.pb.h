// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: data_pb_common.proto

#ifndef PROTOBUF_data_5fpb_5fcommon_2eproto__INCLUDED
#define PROTOBUF_data_5fpb_5fcommon_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace db_protos {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_data_5fpb_5fcommon_2eproto();
void protobuf_AssignDesc_data_5fpb_5fcommon_2eproto();
void protobuf_ShutdownFile_data_5fpb_5fcommon_2eproto();

class pb_data_module;
class pb_table_field;
class pb_table_item;

// ===================================================================

class pb_table_field : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:db_protos.pb_table_field) */ {
 public:
  pb_table_field();
  virtual ~pb_table_field();

  pb_table_field(const pb_table_field& from);

  inline pb_table_field& operator=(const pb_table_field& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const pb_table_field& default_instance();

  void Swap(pb_table_field* other);

  // implements Message ----------------------------------------------

  inline pb_table_field* New() const { return New(NULL); }

  pb_table_field* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const pb_table_field& from);
  void MergeFrom(const pb_table_field& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(pb_table_field* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional int32 val_type = 1;
  void clear_val_type();
  static const int kValTypeFieldNumber = 1;
  ::google::protobuf::int32 val_type() const;
  void set_val_type(::google::protobuf::int32 value);

  // optional int32 field_index = 2;
  void clear_field_index();
  static const int kFieldIndexFieldNumber = 2;
  ::google::protobuf::int32 field_index() const;
  void set_field_index(::google::protobuf::int32 value);

  // optional float val_f = 3;
  void clear_val_f();
  static const int kValFFieldNumber = 3;
  float val_f() const;
  void set_val_f(float value);

  // optional int32 val_int = 4;
  void clear_val_int();
  static const int kValIntFieldNumber = 4;
  ::google::protobuf::int32 val_int() const;
  void set_val_int(::google::protobuf::int32 value);

  // optional int64 val_bigint = 5;
  void clear_val_bigint();
  static const int kValBigintFieldNumber = 5;
  ::google::protobuf::int64 val_bigint() const;
  void set_val_bigint(::google::protobuf::int64 value);

  // optional string val_str = 6;
  void clear_val_str();
  static const int kValStrFieldNumber = 6;
  const ::std::string& val_str() const;
  void set_val_str(const ::std::string& value);
  void set_val_str(const char* value);
  void set_val_str(const char* value, size_t size);
  ::std::string* mutable_val_str();
  ::std::string* release_val_str();
  void set_allocated_val_str(::std::string* val_str);

  // optional bool val_bool = 7;
  void clear_val_bool();
  static const int kValBoolFieldNumber = 7;
  bool val_bool() const;
  void set_val_bool(bool value);

  // @@protoc_insertion_point(class_scope:db_protos.pb_table_field)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::int32 val_type_;
  ::google::protobuf::int32 field_index_;
  float val_f_;
  ::google::protobuf::int32 val_int_;
  ::google::protobuf::int64 val_bigint_;
  ::google::protobuf::internal::ArenaStringPtr val_str_;
  bool val_bool_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_data_5fpb_5fcommon_2eproto();
  friend void protobuf_AssignDesc_data_5fpb_5fcommon_2eproto();
  friend void protobuf_ShutdownFile_data_5fpb_5fcommon_2eproto();

  void InitAsDefaultInstance();
  static pb_table_field* default_instance_;
};
// -------------------------------------------------------------------

class pb_table_item : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:db_protos.pb_table_item) */ {
 public:
  pb_table_item();
  virtual ~pb_table_item();

  pb_table_item(const pb_table_item& from);

  inline pb_table_item& operator=(const pb_table_item& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const pb_table_item& default_instance();

  void Swap(pb_table_item* other);

  // implements Message ----------------------------------------------

  inline pb_table_item* New() const { return New(NULL); }

  pb_table_item* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const pb_table_item& from);
  void MergeFrom(const pb_table_item& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(pb_table_item* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .db_protos.pb_table_field field_list = 1;
  int field_list_size() const;
  void clear_field_list();
  static const int kFieldListFieldNumber = 1;
  const ::db_protos::pb_table_field& field_list(int index) const;
  ::db_protos::pb_table_field* mutable_field_list(int index);
  ::db_protos::pb_table_field* add_field_list();
  ::google::protobuf::RepeatedPtrField< ::db_protos::pb_table_field >*
      mutable_field_list();
  const ::google::protobuf::RepeatedPtrField< ::db_protos::pb_table_field >&
      field_list() const;

  // optional int32 db_status = 2;
  void clear_db_status();
  static const int kDbStatusFieldNumber = 2;
  ::google::protobuf::int32 db_status() const;
  void set_db_status(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:db_protos.pb_table_item)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::RepeatedPtrField< ::db_protos::pb_table_field > field_list_;
  ::google::protobuf::int32 db_status_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_data_5fpb_5fcommon_2eproto();
  friend void protobuf_AssignDesc_data_5fpb_5fcommon_2eproto();
  friend void protobuf_ShutdownFile_data_5fpb_5fcommon_2eproto();

  void InitAsDefaultInstance();
  static pb_table_item* default_instance_;
};
// -------------------------------------------------------------------

class pb_data_module : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:db_protos.pb_data_module) */ {
 public:
  pb_data_module();
  virtual ~pb_data_module();

  pb_data_module(const pb_data_module& from);

  inline pb_data_module& operator=(const pb_data_module& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const pb_data_module& default_instance();

  void Swap(pb_data_module* other);

  // implements Message ----------------------------------------------

  inline pb_data_module* New() const { return New(NULL); }

  pb_data_module* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const pb_data_module& from);
  void MergeFrom(const pb_data_module& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(pb_data_module* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional int32 table_id = 1;
  void clear_table_id();
  static const int kTableIdFieldNumber = 1;
  ::google::protobuf::int32 table_id() const;
  void set_table_id(::google::protobuf::int32 value);

  // repeated .db_protos.pb_table_item data_items = 2;
  int data_items_size() const;
  void clear_data_items();
  static const int kDataItemsFieldNumber = 2;
  const ::db_protos::pb_table_item& data_items(int index) const;
  ::db_protos::pb_table_item* mutable_data_items(int index);
  ::db_protos::pb_table_item* add_data_items();
  ::google::protobuf::RepeatedPtrField< ::db_protos::pb_table_item >*
      mutable_data_items();
  const ::google::protobuf::RepeatedPtrField< ::db_protos::pb_table_item >&
      data_items() const;

  // @@protoc_insertion_point(class_scope:db_protos.pb_data_module)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::RepeatedPtrField< ::db_protos::pb_table_item > data_items_;
  ::google::protobuf::int32 table_id_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_data_5fpb_5fcommon_2eproto();
  friend void protobuf_AssignDesc_data_5fpb_5fcommon_2eproto();
  friend void protobuf_ShutdownFile_data_5fpb_5fcommon_2eproto();

  void InitAsDefaultInstance();
  static pb_data_module* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// pb_table_field

// optional int32 val_type = 1;
inline void pb_table_field::clear_val_type() {
  val_type_ = 0;
}
inline ::google::protobuf::int32 pb_table_field::val_type() const {
  // @@protoc_insertion_point(field_get:db_protos.pb_table_field.val_type)
  return val_type_;
}
inline void pb_table_field::set_val_type(::google::protobuf::int32 value) {
  
  val_type_ = value;
  // @@protoc_insertion_point(field_set:db_protos.pb_table_field.val_type)
}

// optional int32 field_index = 2;
inline void pb_table_field::clear_field_index() {
  field_index_ = 0;
}
inline ::google::protobuf::int32 pb_table_field::field_index() const {
  // @@protoc_insertion_point(field_get:db_protos.pb_table_field.field_index)
  return field_index_;
}
inline void pb_table_field::set_field_index(::google::protobuf::int32 value) {
  
  field_index_ = value;
  // @@protoc_insertion_point(field_set:db_protos.pb_table_field.field_index)
}

// optional float val_f = 3;
inline void pb_table_field::clear_val_f() {
  val_f_ = 0;
}
inline float pb_table_field::val_f() const {
  // @@protoc_insertion_point(field_get:db_protos.pb_table_field.val_f)
  return val_f_;
}
inline void pb_table_field::set_val_f(float value) {
  
  val_f_ = value;
  // @@protoc_insertion_point(field_set:db_protos.pb_table_field.val_f)
}

// optional int32 val_int = 4;
inline void pb_table_field::clear_val_int() {
  val_int_ = 0;
}
inline ::google::protobuf::int32 pb_table_field::val_int() const {
  // @@protoc_insertion_point(field_get:db_protos.pb_table_field.val_int)
  return val_int_;
}
inline void pb_table_field::set_val_int(::google::protobuf::int32 value) {
  
  val_int_ = value;
  // @@protoc_insertion_point(field_set:db_protos.pb_table_field.val_int)
}

// optional int64 val_bigint = 5;
inline void pb_table_field::clear_val_bigint() {
  val_bigint_ = GOOGLE_LONGLONG(0);
}
inline ::google::protobuf::int64 pb_table_field::val_bigint() const {
  // @@protoc_insertion_point(field_get:db_protos.pb_table_field.val_bigint)
  return val_bigint_;
}
inline void pb_table_field::set_val_bigint(::google::protobuf::int64 value) {
  
  val_bigint_ = value;
  // @@protoc_insertion_point(field_set:db_protos.pb_table_field.val_bigint)
}

// optional string val_str = 6;
inline void pb_table_field::clear_val_str() {
  val_str_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& pb_table_field::val_str() const {
  // @@protoc_insertion_point(field_get:db_protos.pb_table_field.val_str)
  return val_str_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void pb_table_field::set_val_str(const ::std::string& value) {
  
  val_str_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:db_protos.pb_table_field.val_str)
}
inline void pb_table_field::set_val_str(const char* value) {
  
  val_str_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:db_protos.pb_table_field.val_str)
}
inline void pb_table_field::set_val_str(const char* value, size_t size) {
  
  val_str_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:db_protos.pb_table_field.val_str)
}
inline ::std::string* pb_table_field::mutable_val_str() {
  
  // @@protoc_insertion_point(field_mutable:db_protos.pb_table_field.val_str)
  return val_str_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* pb_table_field::release_val_str() {
  // @@protoc_insertion_point(field_release:db_protos.pb_table_field.val_str)
  
  return val_str_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void pb_table_field::set_allocated_val_str(::std::string* val_str) {
  if (val_str != NULL) {
    
  } else {
    
  }
  val_str_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), val_str);
  // @@protoc_insertion_point(field_set_allocated:db_protos.pb_table_field.val_str)
}

// optional bool val_bool = 7;
inline void pb_table_field::clear_val_bool() {
  val_bool_ = false;
}
inline bool pb_table_field::val_bool() const {
  // @@protoc_insertion_point(field_get:db_protos.pb_table_field.val_bool)
  return val_bool_;
}
inline void pb_table_field::set_val_bool(bool value) {
  
  val_bool_ = value;
  // @@protoc_insertion_point(field_set:db_protos.pb_table_field.val_bool)
}

// -------------------------------------------------------------------

// pb_table_item

// repeated .db_protos.pb_table_field field_list = 1;
inline int pb_table_item::field_list_size() const {
  return field_list_.size();
}
inline void pb_table_item::clear_field_list() {
  field_list_.Clear();
}
inline const ::db_protos::pb_table_field& pb_table_item::field_list(int index) const {
  // @@protoc_insertion_point(field_get:db_protos.pb_table_item.field_list)
  return field_list_.Get(index);
}
inline ::db_protos::pb_table_field* pb_table_item::mutable_field_list(int index) {
  // @@protoc_insertion_point(field_mutable:db_protos.pb_table_item.field_list)
  return field_list_.Mutable(index);
}
inline ::db_protos::pb_table_field* pb_table_item::add_field_list() {
  // @@protoc_insertion_point(field_add:db_protos.pb_table_item.field_list)
  return field_list_.Add();
}
inline ::google::protobuf::RepeatedPtrField< ::db_protos::pb_table_field >*
pb_table_item::mutable_field_list() {
  // @@protoc_insertion_point(field_mutable_list:db_protos.pb_table_item.field_list)
  return &field_list_;
}
inline const ::google::protobuf::RepeatedPtrField< ::db_protos::pb_table_field >&
pb_table_item::field_list() const {
  // @@protoc_insertion_point(field_list:db_protos.pb_table_item.field_list)
  return field_list_;
}

// optional int32 db_status = 2;
inline void pb_table_item::clear_db_status() {
  db_status_ = 0;
}
inline ::google::protobuf::int32 pb_table_item::db_status() const {
  // @@protoc_insertion_point(field_get:db_protos.pb_table_item.db_status)
  return db_status_;
}
inline void pb_table_item::set_db_status(::google::protobuf::int32 value) {
  
  db_status_ = value;
  // @@protoc_insertion_point(field_set:db_protos.pb_table_item.db_status)
}

// -------------------------------------------------------------------

// pb_data_module

// optional int32 table_id = 1;
inline void pb_data_module::clear_table_id() {
  table_id_ = 0;
}
inline ::google::protobuf::int32 pb_data_module::table_id() const {
  // @@protoc_insertion_point(field_get:db_protos.pb_data_module.table_id)
  return table_id_;
}
inline void pb_data_module::set_table_id(::google::protobuf::int32 value) {
  
  table_id_ = value;
  // @@protoc_insertion_point(field_set:db_protos.pb_data_module.table_id)
}

// repeated .db_protos.pb_table_item data_items = 2;
inline int pb_data_module::data_items_size() const {
  return data_items_.size();
}
inline void pb_data_module::clear_data_items() {
  data_items_.Clear();
}
inline const ::db_protos::pb_table_item& pb_data_module::data_items(int index) const {
  // @@protoc_insertion_point(field_get:db_protos.pb_data_module.data_items)
  return data_items_.Get(index);
}
inline ::db_protos::pb_table_item* pb_data_module::mutable_data_items(int index) {
  // @@protoc_insertion_point(field_mutable:db_protos.pb_data_module.data_items)
  return data_items_.Mutable(index);
}
inline ::db_protos::pb_table_item* pb_data_module::add_data_items() {
  // @@protoc_insertion_point(field_add:db_protos.pb_data_module.data_items)
  return data_items_.Add();
}
inline ::google::protobuf::RepeatedPtrField< ::db_protos::pb_table_item >*
pb_data_module::mutable_data_items() {
  // @@protoc_insertion_point(field_mutable_list:db_protos.pb_data_module.data_items)
  return &data_items_;
}
inline const ::google::protobuf::RepeatedPtrField< ::db_protos::pb_table_item >&
pb_data_module::data_items() const {
  // @@protoc_insertion_point(field_list:db_protos.pb_data_module.data_items)
  return data_items_;
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace db_protos

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_data_5fpb_5fcommon_2eproto__INCLUDED