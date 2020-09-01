#pragma once
#include <string>
namespace libparse2 {

enum StrTokenType {
    TYPE_PRE = -1000,
    TYPE_ALGO_OPERATOR = 1,
    TYPE_VARIBLE = 2,
    TYPE_CMP_OPERATOR = 3,
    TYPE_CONST = 4,
    TYPE_COMMA = 5,
    TYPE_OTHERS = 6,
    TYPE_STRING = 7,
    TYPE_LEFT_PARENTHESES = 8,
    TYPE_RIGHT_PARENTHESES = 9,
    TYPE_BRANCH = 10,
    TYPE_NUMBER = 11,
    TYPE_OTERNARY = 12,
};

enum OperateType {
    kADD = 1,
    kSUB = 2,
    kMUL = 3,
    kDIV = 4,
    kIN = 5,
    kGREATE = 6,
    kGREATEOREQUAL = 7,
    kLOWER = 8,
    kLOWEROREQUAL = 9,
    kEQUAL = 10,
    kNOTEQUAL = 11,
    kNOT = 12,
    kAND = 13,
    kOR = 14,
    kLEFTPARENTHESIS = 15,   //(
    kRIGHTPARENTHESIS = 16,  //)
    kMOD = 17,
    kFUNC1 = 18,    // 1参数，一个返回值
    kFUNC2 = 19,    // 2参数，一个返回值
    kFUNC3 = 20,    // 3参数，一个返回值
    kFUNC4 = 21,    // 3参数，一个返回值
    kFUNC5 = 22,    // 3参数，一个返回值
    kVEC = 23,      // 一个数组，暂时只支持常量
    kSET = 24,      // 一个集合，暂时只支持常量
    kPOW = 25,      // 2 ^ 3 = 8
    kASIGN = 26,    // 赋值
    kBRANCH = 27,   // 逗号
    kTERNARY = 28,  // 条件运算符
};

enum ValueType {
    vNull = 0,
    vInt = 1,
    vLong = 2,
    vFloat = 3,
    vDouble = 4,
    vString = 5,
    vBool = 6,
    vUserType = 7,
    vVecBool = 8,
    vVecInt = 9,
    vVecLong = 10,
    vVecFloat = 11,
    vVecDouble = 12,
    vVecString = 13,
    vMapBool = 14,
    vMapInt = 15,
    vMapLong = 16,
    vMapFloat = 17,
    vMapDouble = 18,
    vMapString = 19,
    vSetBool = 20,
    vSetInt = 22,
    vSetLong = 22,
    vSetFloat = 23,
    vSetDouble = 24,
    vSetString = 25,
    vInts = 26,
    vLongs = 27,
    vFloats = 28,
    vDoubles = 29,
    vBools = 30,
    vPointer = 31,
    vPointers = 32,
    vStrings = 33,
    vUserTypes = 34,
};

enum TokenType {
    tOperator = 1,
    tConst = 2,
    tVarible = 3,
    tLeftParenthesis = 4,
    tInnerFunction = 100000,       // 占用10万个空位
    tFunctionBeginIndex = 200000,  //占用10万个空位
};

enum NodeLevel { nLeaf = 1, nAllChildIsLeaf = 2, nEtherChildIsLeaf = 3, nNoneChildIsLeaf = 4 };

enum MemType { mm_Const = 1, mm_Var = 2, mm_Alloc = 3 };

};  // namespace libparse2
