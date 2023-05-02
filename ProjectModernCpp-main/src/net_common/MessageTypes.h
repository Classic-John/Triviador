#pragma once

#include <string>
#include <type_traits>
#include <array>

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#include <crow/json.h>
#pragma warning(default:4267)
#pragma warning(default:4244)

template <typename DataType>
static void createMapping(crow::json::wvalue& json, const char* key, const DataType& data)
{
	json[key] = data;
}

template <typename ElemType>
static void createMapping(crow::json::wvalue& json, const char* key, const std::vector<ElemType>& data)
{
	json[key] = crow::json::wvalue::list(data.begin(), data.end());
}

template <typename ElemType, std::size_t N>
static void createMapping(crow::json::wvalue& json, const char* key, const std::array<ElemType, N>& data)
{
	json[key] = crow::json::wvalue::list(data.begin(), data.end());
}

template <typename DataType, std::enable_if_t<std::is_integral_v<DataType>, bool> = 0>
static void dumpMapping(crow::json::rvalue& json, const char* key, DataType& data)
{
	data = static_cast<DataType>(json[key].i());
}

static void dumpMapping(crow::json::rvalue& json, const char* key, bool& data)
{
	data = json[key].b();
}

template <typename DataType, std::enable_if_t<std::is_floating_point_v<DataType>, bool> = 0>
static void dumpMapping(crow::json::rvalue& json, const char* key, DataType& data)
{
	data = static_cast<DataType>(json[key].d());
}

static void dumpMapping(crow::json::rvalue& json, const char* key, std::string& data)
{
	data = json[key].s();
}

static void dumpMapping(crow::json::rvalue& json, const char* key, std::vector<std::string>& data)
{
	for (const auto& elem : json[key])
		data.push_back(elem.s());
}

template <typename ElemType, std::enable_if_t<std::is_integral_v<ElemType>, bool> = 0>
static void dumpMapping(crow::json::rvalue& json, const char* key, std::vector<ElemType>& data)
{
	for (const auto& elem : json[key])
		data.push_back(static_cast<ElemType>(elem.i()));
}

template <typename ElemType, std::size_t N, std::enable_if_t<std::is_integral_v<ElemType>, bool> = 0>
static void dumpMapping(crow::json::rvalue& json, const char* key, std::array<ElemType, N>& data)
{
	for (size_t i = 0; const auto & elem : json[key])
		data[i++] = static_cast<ElemType>(elem.i());
}

template <std::size_t N>
static void dumpMapping(crow::json::rvalue& json, const char* key, std::array<std::string, N>& data)
{
	for (size_t i = 0; const auto & elem : json[key])
		data[i++] = elem.s();
}

// Helper FOR_EACH macro for expanding a variable number of macro arguments
// Requires C++20 and /Zc:preprocessor command-line argument for enabling __VA_OPT__
// Adapted from https://www.scs.stanford.edu/~dm/blog/va-opt.html

#define PARENS ()

#define EXPAND(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

#define FOR_EACH(macro, ...)										\
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER(macro, a1, a2, ...)                         \
  macro(a1, a2)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (macro, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER


#define _DECL_FIELD(type, name) type name;
#define _SERIALIZE_FIELD(type, name) createMapping(json, #name, name);
#define _DESERIALIZE_FIELD(type, name) dumpMapping(json, #name, name);

// Create a serializable struct with a variable number of fields of any type
// MsgName = name of struct
// ... = type, name, ... = type and name of a struct field,
// any number of such pairs
#define DECL_MESSAGE_TYPE(MsgName, ...)							    \
struct MsgName														\
{																	\
	EXPAND1(FOR_EACH(_DECL_FIELD, __VA_ARGS__))						\
																	\
	std::string serialize() const {									\
		crow::json::wvalue json;									\
		EXPAND1(FOR_EACH(_SERIALIZE_FIELD, __VA_ARGS__));			\
		return json.dump();											\
	}																\
																	\
	void deserialize(const std::string& msg) {						\
		crow::json::rvalue json = crow::json::load(msg);			\
		EXPAND1(FOR_EACH(_DESERIALIZE_FIELD, __VA_ARGS__));			\
	}																\
																	\
	static std::string Serialize(const MsgName& obj) {				\
		return obj.serialize();										\
	}																\
																	\
	static MsgName Deserialize(const std::string& msg) {			\
		MsgName obj;												\
		obj.deserialize(msg);										\
		return obj;													\
	}																\
}																	\

using FourArray = std::array<std::string, 4>;

namespace net
{
	DECL_MESSAGE_TYPE(LoginInfo, std::string, username, std::string, password);
	DECL_MESSAGE_TYPE(RegisterInfo, std::string, username, std::string, email, std::string, password);
	DECL_MESSAGE_TYPE(SessionInfo, uint32_t, sessionToken);
	DECL_MESSAGE_TYPE(JoinRoomInfo, uint32_t, roomCode);
	DECL_MESSAGE_TYPE(InitRoomInfo, uint32_t, roomCode);
	DECL_MESSAGE_TYPE(PlayerHistoryInfo, std::string, username, int32_t, numWins);
	DECL_MESSAGE_TYPE(WaitingRoomInfo, uint32_t, roomCode, std::vector<std::string>, players, bool, changed, bool, gameStarted);
	DECL_MESSAGE_TYPE(InitGameInfo, std::string, mapName, std::vector<std::string>, players);
	DECL_MESSAGE_TYPE(GameActionInfo, uint8_t, action);
	DECL_MESSAGE_TYPE(ChoiceQuestionInfo, std::string, question, FourArray, answers, uint64_t, expireTime);
	DECL_MESSAGE_TYPE(NumericQuestionInfo, std::string, question, uint64_t, expireTime);

	DECL_MESSAGE_TYPE(NumericAnswerInfo, uint32_t, answer);
	DECL_MESSAGE_TYPE(ChoiceAnswerInfo, uint64_t, index);

	DECL_MESSAGE_TYPE(TerritorySelectionInfo, uint64_t, index);
	DECL_MESSAGE_TYPE(SelectableTerritoriesInfo, std::vector<uint64_t>, territories);
	DECL_MESSAGE_TYPE(TerritoryUpdatedInfo, uint64_t, index, uint32_t, score, uint64_t, ownerIndex);
}