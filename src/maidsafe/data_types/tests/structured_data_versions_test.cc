/***************************************************************************************************
 *  Copyright 2013 maidsafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use. The use of this code is governed by the licence file licence.txt found in the root of     *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit written *
 *  permission of the board of directors of MaidSafe.net.                                          *
 **************************************************************************************************/


#include <algorithm>
#include <random>
#include <string>
#include <vector>

#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/data_types/structured_data_versions.h"


namespace maidsafe {

namespace test {

namespace {

typedef StructuredDataVersions::VersionName VersionName;

ImmutableData::name_type RandomId() {
  return ImmutableData::name_type(Identity(RandomAlphaNumericString(64)));
}

std::vector<VersionName> AddBranch(StructuredDataVersions& versions,
                                   VersionName old_version,
                                   uint32_t start_index,
                                   uint32_t count) {
  std::vector<VersionName> branch(1, old_version);
  for (uint32_t i(0); i != count; ++i) {
    VersionName new_version(start_index + i, RandomId());
    versions.Put(old_version, new_version);
    branch.push_back(new_version);
    old_version = new_version;
  }
  return branch;
}

std::string DisplayVersion(const VersionName& version, bool to_hex) {
  return std::to_string(version.index) + "-" +
      (version.id->IsInitialised() ?
         (to_hex ? EncodeToHex(version.id.data) : version.id->string()).substr(0, 3) :
         ("Uninitialised"));
}

std::string DisplayVersions(const std::vector<VersionName>& versions, bool to_hex) {
  std::string result;
  for (const auto& version : versions)
    result += DisplayVersion(version, to_hex) + "  ";
  return result;
}

void ConstructAsDiagram(StructuredDataVersions& versions) {
/*   7-yyy       0-aaa
       |           |
       |           |
     8-zzz       1-bbb
              /    |   \
            /      |     \
         2-ccc   2-ddd   2-eee
         /         |          \
       /           |            \
    3-fff        3-ggg           3-hhh
      |           /  \             /  \
      |         /      \         /      \
    4-iii    4-jjj    4-kkk   4-lll    4-mmm
                        |
                        |
                      5-nnn                        */
  VersionName v0_aaa(0, ImmutableData::name_type(Identity(std::string(64, 'a'))));
  VersionName v1_bbb(1, ImmutableData::name_type(Identity(std::string(64, 'b'))));
  VersionName v2_ccc(2, ImmutableData::name_type(Identity(std::string(64, 'c'))));
  VersionName v2_ddd(2, ImmutableData::name_type(Identity(std::string(64, 'd'))));
  VersionName v2_eee(2, ImmutableData::name_type(Identity(std::string(64, 'e'))));
  VersionName v3_fff(3, ImmutableData::name_type(Identity(std::string(64, 'f'))));
  VersionName v3_ggg(3, ImmutableData::name_type(Identity(std::string(64, 'g'))));
  VersionName v3_hhh(3, ImmutableData::name_type(Identity(std::string(64, 'h'))));
  VersionName v4_iii(4, ImmutableData::name_type(Identity(std::string(64, 'i'))));
  VersionName v4_jjj(4, ImmutableData::name_type(Identity(std::string(64, 'j'))));
  VersionName v4_kkk(4, ImmutableData::name_type(Identity(std::string(64, 'k'))));
  VersionName v4_lll(4, ImmutableData::name_type(Identity(std::string(64, 'l'))));
  VersionName v4_mmm(4, ImmutableData::name_type(Identity(std::string(64, 'm'))));
  VersionName v5_nnn(5, ImmutableData::name_type(Identity(std::string(64, 'n'))));
  VersionName absent(6, ImmutableData::name_type(Identity(std::string(64, 'x'))));
  VersionName v7_yyy(7, ImmutableData::name_type(Identity(std::string(64, 'y'))));
  VersionName v8_zzz(8, ImmutableData::name_type(Identity(std::string(64, 'z'))));
  std::vector<std::pair<VersionName, VersionName>> puts;
  puts.push_back(std::make_pair(VersionName(), v0_aaa));
  puts.push_back(std::make_pair(v0_aaa, v1_bbb));
  puts.push_back(std::make_pair(v1_bbb, v2_ccc));
  puts.push_back(std::make_pair(v2_ccc, v3_fff));
  puts.push_back(std::make_pair(v3_fff, v4_iii));
  puts.push_back(std::make_pair(v1_bbb, v2_ddd));
  puts.push_back(std::make_pair(v2_ddd, v3_ggg));
  puts.push_back(std::make_pair(v3_ggg, v4_jjj));
  puts.push_back(std::make_pair(v3_ggg, v4_kkk));
  puts.push_back(std::make_pair(v4_kkk, v5_nnn));
  puts.push_back(std::make_pair(v1_bbb, v2_eee));
  puts.push_back(std::make_pair(v2_eee, v3_hhh));
  puts.push_back(std::make_pair(v3_hhh, v4_lll));
  puts.push_back(std::make_pair(v3_hhh, v4_mmm));
  puts.push_back(std::make_pair(absent, v7_yyy));
  puts.push_back(std::make_pair(v7_yyy, v8_zzz));

  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::shuffle(std::begin(puts), std::end(puts), generator);
  for (const auto& put : puts) {
    LOG(kInfo) << "Putting - old: " << DisplayVersion(put.first, false)
               << "  new: " << DisplayVersion(put.second, false);
    versions.Put(put.first, put.second);
  }
}

testing::AssertionResult Equivalent(const StructuredDataVersions& lhs,
                                    const StructuredDataVersions& rhs) {
  if (lhs.max_versions() != rhs.max_versions())
    return testing::AssertionFailure() << "lhs.max_versions (" << lhs.max_versions()
        << ") != rhs.max_versions(" << rhs.max_versions() << ")";

  if (lhs.max_branches() != rhs.max_branches())
    return testing::AssertionFailure() << "lhs.max_branches (" << lhs.max_branches()
        << ") != rhs.max_branches(" << rhs.max_branches() << ")";

  auto lhs_tots(lhs.Get());
  auto rhs_tots(rhs.Get());
  std::sort(std::begin(lhs_tots), std::end(lhs_tots));
  std::sort(std::begin(rhs_tots), std::end(rhs_tots));
  if (lhs_tots != rhs_tots) {
    return testing::AssertionFailure() << "lhs.tips_of_trees != rhs.tips_of_trees:\n"
        << "lhs: " << DisplayVersions(lhs_tots, false) << "\n rhs: "
        << DisplayVersions(rhs_tots, false) << '\n';
  }

  std::vector<std::vector<VersionName>> lhs_branches, rhs_branches;
  for (const auto& lhs_tot : lhs_tots)
    lhs_branches.push_back(lhs.GetBranch(lhs_tot));
  for (const auto& rhs_tot : rhs_tots)
    rhs_branches.push_back(rhs.GetBranch(rhs_tot));
  if (lhs_branches != rhs_branches) {
    std::string output("lhs.branches != rhs.branches:\n");
    for (size_t i(0); i != lhs_branches.size(); ++i) {
      output += "lhs " + std::to_string(i) + ": " + DisplayVersions(lhs_branches[i], false) +
          "\n rhs " + std::to_string(i) + ": " + DisplayVersions(rhs_branches[i], false) + '\n';
    }
    return testing::AssertionFailure() << output;
  }
  return testing::AssertionSuccess();
}

}  // unnamed namespace


TEST(StructuredDataVersionsTest, BEH_Put) {
  StructuredDataVersions versions(100, 10);
  VersionName old_version, new_version;
  for (uint32_t i(0); i != 100; ++i) {
    new_version = VersionName(i, RandomId());
    versions.Put(old_version, new_version);
    if (i % 20 == 0 && i != 0) {
      for (uint32_t j(0); j != (i / 20); ++j)
        AddBranch(versions, old_version, i, 20);
    }
    old_version = new_version;
  }
}

TEST(StructuredDataVersionsTest, BEH_PutOrphans) {
  StructuredDataVersions versions(1000, 100);
  VersionName old_version, new_version;
  std::vector<std::pair<VersionName, VersionName>> missing_names;
  for (uint32_t i(0); i != 100; ++i) {
    new_version = VersionName(i, RandomId());
    if (i % 20 == 0 && i != 0 && i != 20) {
      for (uint32_t j(0); j != (i / 20); ++j) {
        auto branch(AddBranch(versions, new_version, i, 20));
        AddBranch(versions, branch[7], i + 7, 20);
        AddBranch(versions, branch[14], i + 14, 20);
      }
      missing_names.push_back(std::make_pair(old_version, new_version));
    } else {
      versions.Put(old_version, new_version);
    }
    old_version = new_version;
  }

  for (const auto& missing_name : missing_names)
    versions.Put(missing_name.first, missing_name.second);
}

TEST(StructuredDataVersionsTest, BEH_Serialise) {
  StructuredDataVersions versions1(100, 20), versions2(100, 20);
  ConstructAsDiagram(versions1);
  ConstructAsDiagram(versions2);

  EXPECT_TRUE(Equivalent(versions1, versions2));

  auto serialised1(versions1.Serialise());
  auto serialised2(versions2.Serialise());

  EXPECT_EQ(serialised1, serialised2);

  StructuredDataVersions parsed1(serialised1);
  StructuredDataVersions parsed2(serialised2);

  EXPECT_TRUE(Equivalent(versions1, parsed1));
  EXPECT_TRUE(Equivalent(versions2, parsed2));
  EXPECT_TRUE(Equivalent(parsed1, parsed2));

  auto reserialised1(parsed1.Serialise());
  auto reserialised2(parsed2.Serialise());
  EXPECT_EQ(serialised1, reserialised1);
  EXPECT_EQ(serialised2, reserialised2);
  EXPECT_EQ(reserialised1, reserialised2);
}

TEST(StructuredDataVersionsTest, BEH_ApplySerialised) {
  StructuredDataVersions versions1(100, 20);
  ConstructAsDiagram(versions1);
  auto serialised1(versions1.Serialise());
  // Check applying all included versions doesn't modify the SDV.
  versions1.ApplySerialised(serialised1);
  auto temp_serialised(versions1.Serialise());
  EXPECT_EQ(serialised1, temp_serialised);

  // Construct SDV with only "absent" version from diagram included.
  VersionName v5_nnn(5, ImmutableData::name_type(Identity(std::string(64, 'n'))));
  VersionName absent(6, ImmutableData::name_type(Identity(std::string(64, 'x'))));
  StructuredDataVersions versions2(100, 20);
  versions2.Put(v5_nnn, absent);
  auto serialised2(versions2.Serialise());

  // Apply each serialised SDV to the other and check they produce the same resultant SDV.
  versions1.ApplySerialised(serialised2);
  versions2.ApplySerialised(serialised1);
  EXPECT_TRUE(Equivalent(versions1, versions2));
}

}  // namespace test

}  // namespace maidsafe
