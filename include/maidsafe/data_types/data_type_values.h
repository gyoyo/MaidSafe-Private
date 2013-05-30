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

#ifndef MAIDSAFE_DATA_TYPES_DATA_TYPE_VALUES_H_
#define MAIDSAFE_DATA_TYPES_DATA_TYPE_VALUES_H_

#include <cstdint>
#include <ostream>
#include <string>

#include "maidsafe/data_types/macros.h"


namespace maidsafe {

#define MAIDSAFE_DATA_TYPES                                                                        \
    (Anmid, passport::PublicAnmid)                                                                 \
    (Ansmid, passport::PublicAnsmid)                                                               \
    (Antmid, passport::PublicAntmid)                                                               \
    (Anmaid, passport::PublicAnmaid)                                                               \
    (Maid, passport::PublicMaid)                                                                   \
    (Pmid, passport::PublicPmid)                                                                   \
    (Mid, passport::Mid)                                                                           \
    (Smid, passport::Smid)                                                                         \
    (Tmid, passport::Tmid)                                                                         \
    (Anmpid, passport::PublicAnmpid)                                                               \
    (Mpid, passport::PublicMpid)                                                                   \
    (ImmutableData, ImmutableData)                                                                 \
    (OwnerDirectory, OwnerDirectory)                                                               \
    (GroupDirectory, GroupDirectory)                                                               \
    (WorldDirectory, WorldDirectory)

DEFINE_MAIDSAFE_DATA_TYPES_ENUM_VALUES(DataTagValue, int32_t)


}  // namespace maidsafe

#endif  // MAIDSAFE_DATA_TYPES_DATA_TYPE_VALUES_H_
