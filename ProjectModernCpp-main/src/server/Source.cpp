#include <iostream>
#include <random>
#include <fstream>
#include <optional>

#define CROW_USE_LOCALTIMEZONE

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#include <crow/app.h>
#include <crow/logging.h>
#pragma warning(default:4267)
#pragma warning(default:4244)

#include "WaitingRoom.h"
#include "QuestionManager.h"
#include "UserManager.h"
#include "WaitingRoomManager.h"
#include "GameManager.h"

#include "model/ClientActions.h"

#include "net_common/MessageTypes.h"

#define SERVER_STARTED(address, port) CROW_LOG_WARNING << "Server started on address " << address << ':' << port

#define USER_LOGGED_IN(sessionID) CROW_LOG_WARNING << "Player logged in with SessionID [" << sessionID << ']'

#define CONNECTION_FAILED(status_code, error) \
	CROW_LOG_WARNING << "Connection failed\n\t\t\t\t" << " Code: [" << status_code << "], Message: [" << error << "]"

#define ROOM_STARTED(sessionID, room_code) CROW_LOG_WARNING << "Player with SessionID [" << sessionID << "] started Room with code [" << room_code << ']'

#define ROOM_JOINED(sessionID, room_code) CROW_LOG_WARNING << "Player with SessionID [" << sessionID << "] joined Room with code [" << room_code << ']'

#define GAME_STARTED(gameID) 0

crow::response createError(uint16_t code, const char* message)
{
	return crow::response(code, message);
}

template <typename InfoType>
crow::response createInfo(uint16_t code, InfoType info)
{
	return crow::response(code, InfoType::Serialize(info));
}

crow::response createResponse(uint16_t code)
{
	return crow::response(code);
}

crow::response unauthorized()
{
	uint16_t error_code = 401;
	const char* error_message = "User not logged in";

	CONNECTION_FAILED(error_code, error_message);
	return createError(error_code, error_message);
}

crow::response roomNotFound()
{
	uint16_t error_code = 404;
	const char* error_message = "Room not found";

	CONNECTION_FAILED(error_code, error_message);
	return createError(error_code, error_message);
}

crow::response alreadyLoggedIn()
{
	uint16_t error_code = 409;
	const char* error_message = "User already logged in";

	CONNECTION_FAILED(error_code, error_message);
	return createError(error_code, error_message);
}

crow::response alreadyRegistered()
{
	uint16_t error_code = 409;
	const char* error_message = "Username is already taken";

	return createError(error_code, error_message);
}

crow::response invalidCredentials()
{
	uint16_t error_code = 401;
	const char* error_message = "Invalid credentials";

	CONNECTION_FAILED(error_code, error_message);
	return createError(error_code, error_message);
}

crow::response userSessionInfo(net::SessionInfo info)
{
	USER_LOGGED_IN(info.sessionToken);
	return createInfo(200, info);
}

crow::response roomCreated(User::IdType id, net::InitRoomInfo info)
{
	ROOM_STARTED(id, info.roomCode);
	return createInfo(200, info);
}

crow::response roomJoined(User::IdType id, WaitingRoom::CodeType roomCode)
{
	ROOM_JOINED(id, roomCode);
	return createResponse(200);
}

crow::response roomInfo(net::WaitingRoomInfo info)
{
	return createInfo(200, info);
}

crow::response roomLeft()
{
	return createResponse(200);
}

crow::response gameStarted() // TODO add info to reposne
{
	return createResponse(200);
}

crow::response gameNotStartable() // TODO add info to reposne
{
	return createError(405, "Not enough players, room couldn't be started");
}

crow::response gameNotFound()
{
	return createError(404, "Game not found");
}

crow::response initGameInfo(net::InitGameInfo gameInfo)
{
	return createInfo(200, gameInfo);
}

crow::response numericalQuestion(net::NumericQuestionInfo info)
{
	return createInfo(200, info);
}

crow::response choiceQuestion(net::ChoiceQuestionInfo info)
{
	return createInfo(200, info);
}

crow::response actionInfo(net::GameActionInfo info)
{
	return createInfo(200, info);
}

crow::response territoryInfo(net::TerritoryUpdatedInfo info)
{
	return createInfo(200, info);
}

int main()
{
	WaitingRoomManager roomMng;
	GameManager gameMng;
	UserManager userMng(R"(db.sqlite)", roomMng, gameMng);

	std::string address = "0.0.0.0";
	uint16_t port = 18080;

	crow::SimpleApp app;
	//app.loglevel(crow::LogLevel::Warning);

	app.bindaddr(address);
	app.port(port);

	SERVER_STARTED(address, port);

	CROW_ROUTE(app, "/login/").methods(crow::HTTPMethod::POST)([&](const crow::request& req) 
		{
		auto userEntity = userMng.validateLogin(req);
		if (!userEntity)
			return invalidCredentials();

		if (userMng.getLoggedInUser(userEntity->userId))
			return alreadyLoggedIn();

		User::IdType userId = userMng.loginUser(std::move(*userEntity));

		return userSessionInfo(net::SessionInfo{.sessionToken = userId});
		});

	CROW_ROUTE(app, "/register/").methods(crow::HTTPMethod::POST)([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (user)
				return alreadyLoggedIn();

			auto registerInfo = net::RegisterInfo::Deserialize(req.body);
			if (userMng.isRegistered(registerInfo.username))
				return alreadyRegistered();
			
			userMng.registerUser(std::move(registerInfo));

			return createResponse(200);
		});

	// PINGING SERVER WHEN IDLE (i.e. when in MainMenu)
	CROW_ROUTE(app, "/ping/")([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			return crow::response(200);
		});

	// TODO: implement history request
	CROW_ROUTE(app, "/player/<int>/")([](int userId)
		{
			return crow::response(200);
		});

	CROW_ROUTE(app, "/room/create/").methods(crow::HTTPMethod::POST)([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			auto room = roomMng.createRoom();
			roomMng.addUserToRoom(user, room);

			net::InitRoomInfo info{ room->getRoomCode() };

			return roomCreated(user->getId(), info);
		});

	CROW_ROUTE(app, "/room/join/").methods(crow::HTTPMethod::POST)([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			auto info = net::JoinRoomInfo::Deserialize(req.body);
			WaitingRoom::CodeType roomCode = info.roomCode;

			// TODO: Factor out waitingRoom object
			auto room = roomMng.getRoom(roomCode).value_or(nullptr);
			if (!room)
				return roomNotFound();

			roomMng.addUserToRoom(user, room);

			return roomJoined(user->getId(), room->getRoomCode());
		});

	//Pinging server when in waitingRoom
	CROW_ROUTE(app, "/room/")([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			auto room = user->getRoom();
			if (!room)
				return roomNotFound();

			net::WaitingRoomInfo info{
				.roomCode = room->getRoomCode(),
				.players = {},
				.changed = user->hasUnseenChanges(),
				.gameStarted = room->m_gameStarted
			};

			user->setHasUnseenChanges(false);
			if (!room->m_gameStarted)
				for (auto player : *room)
					info.players.push_back(player->getName());
			return roomInfo(info);
		});

	CROW_ROUTE(app, "/room/leave/").methods(crow::HTTPMethod::POST)([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			roomMng.removeUserFromRoom(user);

			return roomLeft();
		});



	CROW_ROUTE(app, "/game/start/").methods(crow::HTTPMethod::POST)([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			auto room = user->getRoom();
			if (!room)
				return roomNotFound();
			if (!room->isGameStartable())
				return gameNotStartable();

			room->m_gameStarted = true;

			auto game = gameMng.createGame();
			for (auto user : *room)
			{
				gameMng.addUserToGame(user, game);
			}

			return gameStarted();
		});

	CROW_ROUTE(app, "/game/init/")([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			auto game = user->getGame();
			if (!game)
				return gameNotFound();

			game->playerJoined();
			if (game->allPlayersJoined())
				game->startGame();

			net::InitGameInfo gameInfo{.mapName = "Romania"};

			gameInfo.players = game->getPlayerNames();

			return initGameInfo(gameInfo);
		});

	CROW_ROUTE(app, "/game/")([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			auto game = user->getGame();
			if (!game)
				return gameNotFound();

			Player* player = game->getPlayer(user);

			net::GameActionInfo info{ .action = static_cast<uint8_t>(player->getAction()) };

			return actionInfo(info);
		});

	CROW_ROUTE(app, "/game/question/choice/")([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			auto game = user->getGame();
			if (!game)
				return gameNotFound();

			ChoiceQuestion question = game->getCurrentChoiceQuestion();

			Player* player = game->getPlayer(user);

			net::ChoiceQuestionInfo questionInfo{
			.question = question.getStatement(),
			.answers = question.getChoices(),
			.expireTime = game->getResponseDeadline()};

			player->setAction(ClientAction::ANSWER_CHOICE_QUESTION);

			return choiceQuestion(questionInfo);
		
		});

	CROW_ROUTE(app, "/game/question/numeric/")([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			auto game = user->getGame();
			if (!game)
				return gameNotFound();

			if (!game->isNumericQuestionOngoing())
				return createError(404, "No numeric question ongoing");

			NumericalQuestion question = game->getCurrentNumericalQuestion();
			Player* player = game->getPlayer(user);

			// player received question, they should now answer it
			player->setAction(ClientAction::ANSWER_NUMERICAL_QUESTION);

			net::NumericQuestionInfo questionInfo{
			.question = question.getStatement(),
			.expireTime = game->getResponseDeadline()};

			return numericalQuestion(questionInfo);
		});

	CROW_ROUTE(app, "/game/question/numeric/answer/").methods(crow::HTTPMethod::POST)([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			auto game = user->getGame();
			if (!game)
				return gameNotFound();

			// TODO to be entirely correct, should also check whether a numeric question is being asked currently at all
			if (!game->isNumericQuestionOngoing())
				createError(404, "No numeric question ongoing");

			auto numericAnswerInfo = net::NumericAnswerInfo::Deserialize(req.body);
			game->playerAnsweredNumericQuestion(game->getPlayer(user), numericAnswerInfo.answer);
			
			return createResponse(200);
		});

	CROW_ROUTE(app, "/game/question/choice/answer/").methods(crow::HTTPMethod::POST)([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			auto game = user->getGame();
			if (!game)
				return gameNotFound();

			// TODO to be entirely correct, should also check whether a numeric question is being asked currently at all
			if (!game->isChoiceQuestionOngoing())
				createError(404, "No numeric question ongoing");

			auto choiceAnswerInfo = net::ChoiceAnswerInfo::Deserialize(req.body);
			game->playerAnsweredChoiceQuestion(game->getPlayer(user), choiceAnswerInfo.index);

			return createResponse(200);
		});

	CROW_ROUTE(app, "/game/territory/select/").methods(crow::HTTPMethod::POST)([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();
	
			auto game = user->getGame();
			if (!game)
				return gameNotFound();

			auto selectedTerritoryInfo = net::TerritorySelectionInfo::Deserialize(req.body);
			std::cout << "Received Territory info: " << selectedTerritoryInfo.index << '\n';

			Player* player = game->getPlayer(user);
			game->playerSelectedTerritory(player, selectedTerritoryInfo.index);
			
			
			return createResponse(200);
		});

	CROW_ROUTE(app, "/game/territory/selectable/")([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			auto game = user->getGame();
			if (!game)
				return gameNotFound();

			net::SelectableTerritoriesInfo info{game->selectableTerritories(game->getPlayer(user))};

			return createInfo(200, info);
		});

	CROW_ROUTE(app, "/game/territory/")([&](const crow::request& req)
		{
			auto user = userMng.getLoggedInUser(req).value_or(nullptr);
			if (!user)
				return unauthorized();

			auto game = user->getGame();
			if (!game)
				return gameNotFound();

			auto updatedTerritory = game->getUpdatedTerritory();
			net::TerritoryUpdatedInfo info{
				.index = updatedTerritory->index(), 
				.score=updatedTerritory->score(), 
				.ownerIndex = game->getOwnerIndex(*updatedTerritory)};

			/*if (game->stage() == Game::GameStage::BASE_SELECTION)
			{
				game->playerReceivedBaseTerritoryUpdate(user);
			}*/

			return territoryInfo(info);
		});

	app.multithreaded().run_async().wait();
}