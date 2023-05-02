#pragma once

#include "ClientConnection.h"
#include "net_common.h"
#include "net_tsqueue.h"
#include "MessageTypes.h"

#include <cpr/cpr.h>

#include <optional>

namespace net
{
	class ClientInterface
	{
	public:
		ClientInterface()
		{
		}

		virtual ~ClientInterface()
		{
			Disconnect();
		}

		// TODO: return a Response object
		bool AuthenticateAndConnect(const std::string& address, LoginInfo loginInfo)
		{
			m_connection = std::make_unique<ClientConnection>(m_qMessagesIn);

			bool success = m_connection->Authenticate(address, "/login/", loginInfo.serialize(), "/ping/");
			if (success)
				m_connection->StartConnection();

			return success;
		}

		void setOnDisconnectCallback(std::function<void()> disconnectCallback)
		{
			m_connection->SetConnectionStatusChangeCallback([=](ClientConnection& connection, net::ConnectionStatus oldStatus, net::ConnectionStatus newStatus)
				{
					if (newStatus == net::ConnectionStatus::DISCONNECTED)
					{
						if (disconnectCallback)
						{
							disconnectCallback();
						}
					}
				});
		}

		// Disconnect from server
		void Disconnect()
		{
			//If connection exists, and it connected, then:
			if (IsConnected())
			{
				// disconnect from server
				m_connection->Disconnect("/logout/", "");
			}

			//Destroy the connection object
			m_connection.release();
		}

		// Check if client is actually connected to a server
		bool IsConnected()
		{
			if (m_connection)
			{
				return m_connection->IsConnected();
			}
			return false;
		}

		bool Register(std::string address, net::RegisterInfo info)
		{
			m_connection = std::make_unique<ClientConnection>(m_qMessagesIn);
			m_connection->_SetAddress(std::move(address));
			ClientConnection::Response response = m_connection->SendData("/register/", info.serialize());
			m_connection.release();
			if (response.status_code == cpr::status::HTTP_OK)
				return true;
			return false;
		}

		bool JoinRoom(uint32_t roomCode) {
			net::JoinRoomInfo info{ roomCode };
			ClientConnection::Response response = m_connection->SendData("/room/join/", info.serialize());
			if (response.status_code == cpr::status::HTTP_OK) {
				return true;
			}
			else if (response.status_code == cpr::status::HTTP_NOT_FOUND) {
				return false;
			}
			else {
				return false;
			}
		}

		void LeaveRoom()
		{
			ClientConnection::Response response = m_connection->SendData("/room/leave/", "");
			m_connection->ResetRequester();
		}

		std::optional<net::InitRoomInfo> CreateRoom() {
			ClientConnection::Response response = m_connection->SendData("/room/create/", "");
			if (response.status_code == cpr::status::HTTP_OK) {
				auto info = InitRoomInfo::Deserialize(response.text);
				return info;
			}
			else {
				return std::nullopt;
			}
		}

		std::optional<net::PlayerHistoryInfo> RequestHistory(const std::string& username)
		{
			ClientConnection::Response response = m_connection->SendRequest(std::string("/player/") + username);
			if (response.status_code == cpr::status::HTTP_OK) {
				auto info = PlayerHistoryInfo::Deserialize(response.text);
				return info;
			}
			else {
				return std::nullopt;
			}
		}

		void RequestRoomInfo(std::function<void(std::optional<net::WaitingRoomInfo>)> callback)
		{
			m_connection->SetRequester("/room/", std::chrono::milliseconds(1000),
				[=](net::ClientConnection::Response response)
				{
					if (callback)
					{
						if (response.status_code == cpr::status::HTTP_OK)
						{
							callback(net::WaitingRoomInfo::Deserialize(response.text));
						}
						else
						{
							callback(std::nullopt);
						}
					}
				
				});
		}

		bool StartGame()
		{
			ClientConnection::Response response = m_connection->SendData("/game/start/", "");
			if (response.status_code == cpr::status::HTTP_OK)
			{
				return true;
			}
			return false;
		}

		std::optional<net::InitGameInfo> RequestGameInfo()
		{
			ClientConnection::Response response = m_connection->SendRequest("/game/init/");
			if (response.status_code == cpr::status::HTTP_OK)
			{
				m_connection->ResetRequester();
				return net::InitGameInfo::Deserialize(response.text);
			}
			return std::nullopt;
		}

		void StartRequestingClientAction(std::function<void(std::optional<net::GameActionInfo>)> callback)
		{
			m_connection->SetRequester("/game/", std::chrono::milliseconds(1000),
				[=](net::ClientConnection::Response response)
				{
					if (callback)
					{
						if (response.status_code == cpr::status::HTTP_OK)
						{
							callback(net::GameActionInfo::Deserialize(response.text));
						}
						else
						{
							callback(std::nullopt);
						}
					}

				});
		}

		std::optional<net::NumericQuestionInfo> RequestNumericalQuestion()
		{
			ClientConnection::Response response = m_connection->SendRequest("/game/question/numeric/");
			if (response.status_code == cpr::status::HTTP_OK)
				return net::NumericQuestionInfo::Deserialize(response.text);
			else
				return std::nullopt;
		}

		std::optional<net::ChoiceQuestionInfo> RequestChoiceQuestion()
		{
			ClientConnection::Response response = m_connection->SendRequest("/game/question/choice/");
			if (response.status_code == cpr::status::HTTP_OK)
				return net::ChoiceQuestionInfo::Deserialize(response.text);
			return std::nullopt;
		}

		bool PostNumericalAnswer(net::NumericAnswerInfo answerInfo)
		{
			ClientConnection::Response response = m_connection->SendData("/game/question/numeric/answer/", answerInfo.serialize());
			if (response.status_code == cpr::status::HTTP_OK)
				return true;
			return false;
		}

		bool PostChoiceAnswer(net::ChoiceAnswerInfo answerInfo)
		{
			ClientConnection::Response response = m_connection->SendData("/game/question/choice/answer/", answerInfo.serialize());
			if (response.status_code == cpr::status::HTTP_OK)
				return true;
			return false;
		}

		bool SendTerritorySelection(uint64_t index)
		{
			auto info = net::TerritorySelectionInfo{ index };
            ClientConnection::Response response = m_connection->SendData("/game/territory/select/", info.serialize());
			if (response.status_code == cpr::status::HTTP_OK)
				return true;
			return false;
		}

		std::optional<std::vector<uint64_t>> RequestSelectableTerritories()
		{
			ClientConnection::Response response = m_connection->SendRequest("/game/territory/selectable/");
			if (response.status_code == cpr::status::HTTP_OK)
			{
				auto info = net::SelectableTerritoriesInfo::Deserialize(response.text);
				return std::move(info.territories);
			}
			else
				return std::nullopt;
		}

		std::optional<net::TerritoryUpdatedInfo> RequestTerritoryUpdate()
		{
			ClientConnection::Response response = m_connection->SendRequest("/game/territory/");
			if (response.status_code == cpr::status::HTTP_OK)
				return net::TerritoryUpdatedInfo::Deserialize(response.text);
			else
				return std::nullopt;
		}

		//Retrieve queue of messsages from server
		tsqueue<std::string>& Incoming()
		{
			return m_qMessagesIn;
		}
	protected:
		// The Client has a single instance of a "connection" object, which handles data transfer
		std::unique_ptr<ClientConnection> m_connection;
	private:
		// This is the thread safe queue of incoming messages from the server
		tsqueue<std::string> m_qMessagesIn;

		uint32_t m_clientId;
	};
}

