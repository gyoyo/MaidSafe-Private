/***************************************************************************************************
 *  Copyright 2012 maidsafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use. The use of this code is governed by the licence file licence.txt found in the root of     *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit written *
 *  permission of the board of directors of MaidSafe.net.                                          *
 **************************************************************************************************/

#ifndef MAIDSAFE_DATA_TYPES_DATA_NAME_VARIANT_H_
#define MAIDSAFE_DATA_TYPES_DATA_NAME_VARIANT_H_

#include <type_traits>
#include <utility>

#include "boost/variant/static_visitor.hpp"
#include "boost/variant/variant.hpp"

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/tagged_value.h"
#include "maidsafe/common/types.h"
#include "maidsafe/passport/types.h"

#include "maidsafe/data_types/data_type_values.h"
#include "maidsafe/data_types/immutable_data.h"
#include "maidsafe/data_types/macros.h"
#include "maidsafe/data_types/owner_directory.h"
#include "maidsafe/data_types/group_directory.h"
#include "maidsafe/data_types/world_directory.h"

namespace maidsafe {

template<DataTagValue tag_value, typename Enable = void>
struct is_maidsafe_data {
  static const bool value = false;
};

DEFINE_VARIANT

struct GetTagValueVisitor : public boost::static_visitor<DataTagValue> {
  template<typename T, typename Tag>
  result_type operator()(const TaggedValue<T, Tag>&) const {
    return TaggedValue<T, Tag>::tag_type::kEnumValue;
  }
};

struct GetIdentityVisitor : public boost::static_visitor<Identity> {
  template<typename T, typename Tag>
  result_type operator()(const TaggedValue<T, Tag>& t) const {
    return t.data;
  }
};

struct GetTagValueAndIdentityVisitor
    : public boost::static_visitor<std::pair<DataTagValue, Identity>> {
  template<typename T, typename Tag>
  result_type operator()(const TaggedValue<T, Tag>& t) const {
    return std::make_pair(TaggedValue<T, Tag>::tag_type::kEnumValue, t.data);
  }
};

template<DataTagValue tag_value>
typename std::enable_if<
    is_maidsafe_data<tag_value>::value,
    typename is_maidsafe_data<tag_value>::name_type>::type GetName(const Identity& name) {
  return typename is_maidsafe_data<tag_value>::name_type(name);
}

}  // namespace maidsafe

#endif  // MAIDSAFE_DATA_TYPES_DATA_NAME_VARIANT_H_
