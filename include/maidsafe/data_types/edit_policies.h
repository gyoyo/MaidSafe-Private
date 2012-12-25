/*
* ============================================================================
*
* Copyright [2011] maidsafe.net limited
*
* The following source code is property of maidsafe.net limited and is not
* meant for external use.  The use of this code is governed by the license
* file licence.txt found in the root of this directory and also on
* www.maidsafe.net.
*
* You are not free to copy, amend or otherwise use this source code without
* the explicit written permission of the board of directors of maidsafe.net.
*
* ============================================================================
*/

#ifndef MAIDSAFE_PRIVATE_DATA_MANAGER_EDIT_POLICIES_H_
#define MAIDSAFE_PRIVATE_DATA_MANAGER_EDIT_POLICIES_H_

#include "maidsafe/common/types.h"
#include "maidsafe/private/utils/fob.h"

template <typename T>
class EditIfOwner {
 public:
  static bool Edit();
 protected:
  ~EditIfOwner() {}
};

template <typename T>
class NoEdit {};


#endif  // MAIDSAFE_PRIVATE_DATA_MANAGER_EDIT_POLICIES_H_
