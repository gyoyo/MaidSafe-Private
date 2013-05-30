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

#ifndef MAIDSAFE_DATA_TYPES_MACROS_H_
#define MAIDSAFE_DATA_TYPES_MACROS_H_

#include <string>

#include "boost/preprocessor/arithmetic/dec.hpp"
#include "boost/preprocessor/cat.hpp"
#include "boost/preprocessor/comparison/not_equal.hpp"
#include "boost/preprocessor/punctuation/comma_if.hpp"
#include "boost/preprocessor/punctuation/paren.hpp"
#include "boost/preprocessor/seq/enum.hpp"
#include "boost/preprocessor/seq/for_each.hpp"
#include "boost/preprocessor/seq/for_each_i.hpp"
#include "boost/preprocessor/seq/push_back.hpp"
#include "boost/preprocessor/seq/seq.hpp"
#include "boost/preprocessor/seq/size.hpp"
#include "boost/preprocessor/stringize.hpp"
#include "boost/preprocessor/tuple/elem.hpp"


#define ADD_PAREN_0(A, B) ((A, B)) ADD_PAREN_1
#define ADD_PAREN_1(A, B) ((A, B)) ADD_PAREN_0
#define ADD_PAREN_0_END
#define ADD_PAREN_1_END
#define WRAP_PAIRS_IN_PARENS(input) BOOST_PP_CAT(ADD_PAREN_0 input,_END)  // NOLINT

#define GET_ENUM_VALUE(value) BOOST_PP_CAT(BOOST_PP_CAT(k, value), Value)

#define GET_FIRST_WITH_COMMA(r, data, i, elem)                                                     \
    GET_ENUM_VALUE(BOOST_PP_TUPLE_ELEM(2, 0, elem))                                                \
    BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(i, data))

#define X_DEFINE_ENUM_WITH_STRING_CONVERSIONS_TOSTRING_CASE(r, data, elem)                         \
    case data::GET_ENUM_VALUE(BOOST_PP_TUPLE_ELEM(2, 0, elem)) :                                   \
      str = BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, elem));                                   \
      break;

#define DEFINE_DATA_TYPES_ENUM_VALUES_WITH_PARENS(name, type, enumerators)                         \
    enum class name : type {                                                                       \
      BOOST_PP_SEQ_FOR_EACH_I(GET_FIRST_WITH_COMMA,                                                \
                              BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(enumerators)), enumerators)           \
    };                                                                                             \
                                                                                                   \
    template<typename Elem, typename Traits>                                                       \
    std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ostream,        \
                                                 const name &n) {                                  \
      std::string str;                                                                             \
      switch (n) {                                                                                 \
        BOOST_PP_SEQ_FOR_EACH(                                                                     \
            X_DEFINE_ENUM_WITH_STRING_CONVERSIONS_TOSTRING_CASE,                                   \
            name,                                                                                  \
            enumerators)                                                                           \
        default:                                                                                   \
          str = "Invalid " BOOST_PP_STRINGIZE(name) " type";                                       \
          break;                                                                                   \
      }                                                                                            \
      for (std::string::iterator itr(str.begin()); itr != str.end(); ++itr)                        \
        ostream << ostream.widen(*itr);                                                            \
      return ostream;                                                                              \
    }

#define DEFINE_MAIDSAFE_DATA_TYPES_ENUM_VALUES(name, type)                                         \
    DEFINE_DATA_TYPES_ENUM_VALUES_WITH_PARENS(name, type, WRAP_PAIRS_IN_PARENS(MAIDSAFE_DATA_TYPES))



#define GET_SECOND_WITH_COMMA_AND_NAME_TYPE(r, data, i, elem)                                      \
    BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(2, 1, elem), ::name_type)                                     \
    BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(i, data))

#define MAP_FIRST_TO_SECOND(r, data, elem)                                                         \
    case DataTagValue::GET_ENUM_VALUE(BOOST_PP_TUPLE_ELEM(2, 0, elem)) :                           \
      return BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(2, 1, elem), ::name_type)                            \
             BOOST_PP_LPAREN() name BOOST_PP_RPAREN();                                             \

#define DEFINE_IS_MAIDSAFE_DATA_STRUCT(r, data, elem)                                              \
    template<DataTagValue tag_value>                                                               \
    struct is_maidsafe_data< tag_value,                                                            \
        typename std::enable_if<                                                                   \
            std::is_same<                                                                          \
                std::integral_constant< DataTagValue, tag_value >,                                 \
                std::integral_constant< DataTagValue, DataTagValue::                               \
                    GET_ENUM_VALUE(BOOST_PP_TUPLE_ELEM(2, 0, elem))>>::value>::type> {             \
      static const bool value = true;                                                              \
      typedef BOOST_PP_TUPLE_ELEM(2, 1, elem) data_type;                                           \
      typedef data_type::name_type name_type;                                                      \
    };

#define DEFINE_VARIANT_WITH_PARENS(enumerators)                                                    \
    typedef boost::variant<                                                                        \
        BOOST_PP_SEQ_FOR_EACH_I(                                                                   \
        GET_SECOND_WITH_COMMA_AND_NAME_TYPE,                                                       \
        BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(enumerators)), enumerators) > DataNameVariant;              \
                                                                                                   \
    inline DataNameVariant GetDataNameVariant(DataTagValue type, const Identity& name) {           \
      switch (type) {                                                                              \
        BOOST_PP_SEQ_FOR_EACH(MAP_FIRST_TO_SECOND, , enumerators)                                  \
        default: {                                                                                 \
          LOG(kError) << "Unhandled data type";                                                    \
          ThrowError(CommonErrors::invalid_parameter);                                             \
          return DataNameVariant();                                                                \
        }                                                                                          \
      }                                                                                            \
    }                                                                                              \
                                                                                                   \
    BOOST_PP_SEQ_FOR_EACH(DEFINE_IS_MAIDSAFE_DATA_STRUCT, , enumerators)


#define DEFINE_VARIANT DEFINE_VARIANT_WITH_PARENS(WRAP_PAIRS_IN_PARENS(MAIDSAFE_DATA_TYPES))

#endif  // MAIDSAFE_DATA_TYPES_MACROS_H_
