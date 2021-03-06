/*  Copyright 2013 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#ifndef MAIDSAFE_DATA_TYPES_GROUP_DIRECTORY_H_
#define MAIDSAFE_DATA_TYPES_GROUP_DIRECTORY_H_

#include <cstdint>

#include "maidsafe/common/types.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/tagged_value.h"

#include "maidsafe/data_types/data_type_values.h"


namespace maidsafe {

class GroupDirectory {
 public:
  typedef detail::Name<GroupDirectory> Name;
  typedef detail::Tag<DataTagValue::kGroupDirectoryValue> Tag;
  typedef TaggedValue<NonEmptyString, GroupDirectory> serialised_type;

  GroupDirectory(const GroupDirectory& other);
  GroupDirectory(GroupDirectory&& other);
  GroupDirectory& operator=(GroupDirectory other);

  GroupDirectory(Name name, NonEmptyString data);
  GroupDirectory(Name name, const serialised_type& serialised_mutable_data);
  serialised_type Serialise() const;

  Name name() const;
  NonEmptyString data() const;

  friend void swap(GroupDirectory& lhs, GroupDirectory& rhs);

 private:
  Name name_;
  NonEmptyString data_;
};

template<>
struct is_short_term_cacheable<GroupDirectory> : public std::true_type {};

}  // namespace maidsafe

#endif  // MAIDSAFE_DATA_TYPES_GROUP_DIRECTORY_H_
