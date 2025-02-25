#pragma once

#define DG_ENUM_CLASS_OPERATOR_NOT(enum_class, parent_type) \
~inline enum_class operator~(const enum_class& a) \
{ \
  return static_cast<enum_class>(~static_cast<parent_type>(a)); \
}

#define DG_ENUM_CLASS_OPERATOR_AND(enum_class, parent_type) \
inline enum_class operator&(const enum_class& a, const enum_class& b) \
{ \
  return static_cast<enum_class>(static_cast<parent_type>(a) & static_cast<parent_type>(b)); \
}

#define DG_ENUM_CLASS_OPERATOR_OR(enum_class, parent_type) \
inline enum_class operator|(const enum_class& a, const enum_class& b) \
{ \
  return static_cast<enum_class>(static_cast<parent_type>(a) | static_cast<parent_type>(b)); \
}

#define DG_ENUM_CLASS_OPERATOR_XOR(enum_class, parent_type) \
inline enum_class operator^(const enum_class& a, const enum_class& b) \
{ \
  return static_cast<enum_class>(static_cast<parent_type>(a) ^ static_cast<parent_type>(b)); \
}

#define DG_ENUM_CLASS_OPERATOR_LEFT_SHIFT(enum_class, parent_type) \
inline enum_class operator<<(const enum_class& a, const parent_type& amount) \
{ \
  return static_cast<enum_class>(static_cast<parent_type>(a) << amount); \
}

#define DG_ENUM_CLASS_OPERATOR_RIGHT_SHIFT(enum_class, parent_type) \
inline enum_class operator>>(const enum_class& a, const parent_type& amount) \
{ \
  return static_cast<enum_class>(static_cast<parent_type>(a) >> amount); \
}

#define DG_ALLOW_ENUM_CLASS_BITWISE_OPERATOR(enum_class, parent_type) \
  DG_ENUM_CLASS_OPERATOR_AND(enum_class, parent_type) \
  DG_ENUM_CLASS_OPERATOR_OR(enum_class, parent_type) \
  DG_ENUM_CLASS_OPERATOR_XOR(enum_class, parent_type) \
  DG_ENUM_CLASS_OPERATOR_LEFT_SHIFT(enum_class, parent_type) \
  DG_ENUM_CLASS_OPERATOR_RIGHT_SHIFT(enum_class, parent_type)

