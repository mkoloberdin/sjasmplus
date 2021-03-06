#ifndef SJASMPLUS_PARSER_DEFINES_H
#define SJASMPLUS_PARSER_DEFINES_H

#include <string>

#include "message.h"
#include "macro.h"

using namespace std::string_literals;

namespace parser {

struct DefineSp : RequiredNothing1L {};

struct DefineArg : until<at<TrailingNothing> > {};

struct Define : seq<if_must<TAO_PEGTL_ISTRING("DEFINE"), DefineSp, Identifier> ,
        opt<RequiredNothing1L, DefineArg> > {};

struct DefArrayArgList : MacroArgList {};

struct DefArraySp1 : RequiredNothing1L {};

struct DefArraySp2 : RequiredNothing1L {};

struct DefArray : if_must<TAO_PEGTL_ISTRING("DEFARRAY"), DefArraySp1,
        Identifier, DefArraySp2, DefArrayArgList> {};

template<> const std::string Ctrl<DefineSp>::ErrMsg;
template<> const std::string Ctrl<DefArrayArgList>::ErrMsg;
template<> const std::string Ctrl<DefArraySp1>::ErrMsg;
template<> const std::string Ctrl<DefArraySp2>::ErrMsg;

template<typename State>
struct ActionsT<State, Define> {
    template<typename Input>
    static void apply(const Input &In, State &S) {
        if (S.Asm.setDefine(S.Id, S.StringVec.empty() ? ""s : S.StringVec[0])) {
            State::M::warn(In.position(), "`" + S.Id +  "` redefined"s);
        }
    }
};

template<typename State>
struct ActionsT<State, DefineArg> {
    template<typename Input>
    static void apply(const Input &In, State &S) {
        S.StringVec = {1, In.string()};
    }
};


template<typename State>
struct ActionsT<State, DefArray> {
    template<typename Input>
    static void apply(const Input &In, State &S) {
        if (S.Asm.setDefArray(S.Id, S.StringVec)) {
            State::M::warn(In.position(), "`" + S.Id +  "` redefined"s);
        }
    }
};

} // namespace parser

#endif //SJASMPLUS_PARSER_DEFINES_H
