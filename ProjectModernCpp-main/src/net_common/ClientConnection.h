#pragma once

#pragma once
#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "MessageTypes.h"

#include <cpr/cpr.h>
#include <thread>

namespace net
{
	class ClientConnection : public std::enable_shared_from_this<ClientConnection>
	{
	public:
		struct Response {
			std::string text;
			long status_code;
		};

		ClientConnection(tsqueue<std::string>& qIn)
			: m_qMessagesIn(qIn), m_session(std::make_shared<cpr::Session>())
		{}
		virtual ~ClientConnection()
		{
			stopRequester();
			changeConnectionStatus(ConnectionStatus::DISCONNECTED);
		}

	public:
		// TODO: function to return a more specific response
		bool Authenticate(std::string address, std::string path, std::string credentials, std::string pingPath)
		{
			if (IsConnected())
				return false;

			m_address = std::move(address);
			m_pingPath = std::move(pingPath);

			auto response = SendData(std::move(path), std::move(credentials));
			if (response.status_code == 0 || cpr::status::is_client_error(response.status_code))
				return false;

			net::SessionInfo sessionInfo = net::SessionInfo::Deserialize(response.text);
			id = sessionInfo.sessionToken;

			changeConnectionStatus(net::ConnectionStatus::PENDING_CONNECTION);
			return true;
		}

		bool IsConnected()
		{
			return m_connectionStatus == net::ConnectionStatus::CONNECTED;
		}

		net::ConnectionStatus GetConnectionStatus()
		{
			return m_connectionStatus;
		}

		Response SendRequest(std::string path)
		{
			m_session->SetUrl(cpr::Url{ m_address + std::move(path) });
			m_session->SetParameters(cpr::Parameters{ {"session_token", std::to_string(id)} });
			cpr::Response response = m_session->Get();
			return { std::move(response.text), response.status_code };
		}

		Response SendData(std::string path, std::string message)
		{
			m_session->SetUrl(cpr::Url{ m_address + std::move(path) });
			m_session->SetBody(cpr::Body{ std::move(message) });
			m_session->SetParameters(cpr::Parameters{ {"session_token", std::to_string(id)} });
			cpr::Response response = m_session->Post();
			return { std::move(response.text), response.status_code };
		}

		bool StartConnection()
		{
			if (m_connectionStatus != ConnectionStatus::PENDING_CONNECTION)
				return false;
			changeConnectionStatus(ConnectionStatus::CONNECTED);
			ResetRequester();
			return true;
		}

		void Disconnect(std::string disconnectNotificationPath, std::string message)
		{
			if (!IsConnected())
				return;
			stopRequester();
			changeConnectionStatus(ConnectionStatus::DISCONNECTED);
			SendData(disconnectNotificationPath, message);
		}

		void _SetAddress(std::string address)
		{
			m_address = std::move(address);
		}

		void SetRequester(std::string path, std::chrono::milliseconds interval, std::function<void(Response)> callback)
		{
			std::lock_guard lock(m_workerMutex);
			m_workerInterval = interval;
			m_workerPath = path;
			m_workerCallback = callback;

			if (!m_workerRunning) {
				m_workerRunning = true;
				
				m_workerThread = std::thread([&]()
					{
						while (m_workerRunning) {
							{
								std::lock_guard lock(m_workerMutex);
								Response response = SendRequest(m_workerPath);
								if (response.status_code == 0) {
									stopRequester();
									changeConnectionStatus(net::ConnectionStatus::DISCONNECTED);
									break;
								}
								else if (m_workerCallback)
									m_workerCallback(response);
							}
							std::this_thread::sleep_for(m_workerInterval);
						}
					}
				);
				m_workerThread.detach();
			}
		}

		void ResetRequester()
		{

			SetRequester(m_pingPath, std::chrono::milliseconds(1000), nullptr);
		}

		void SetConnectionStatusChangeCallback(std::function<void(ClientConnection&, net::ConnectionStatus, net::ConnectionStatus)> callback)
		{
			m_onConnectionStatusChangeCallback = callback;
		}
	protected:
		void stopRequester()
		{
			m_workerRunning = false;
		}

		void changeConnectionStatus(net::ConnectionStatus newStatus)
		{
			if (m_connectionStatus == newStatus)
				return;
			auto oldStatus = m_connectionStatus;
			m_connectionStatus = newStatus;
			if (m_onConnectionStatusChangeCallback)
				m_onConnectionStatusChangeCallback(*this, oldStatus, newStatus);
		}

		std::string m_address;

		std::string m_pingPath;

		std::shared_ptr<cpr::Session> m_session;
		net::ConnectionStatus m_connectionStatus = net::ConnectionStatus::DISCONNECTED;
		std::function<void(ClientConnection&, net::ConnectionStatus, net::ConnectionStatus)> m_onConnectionStatusChangeCallback;

		std::thread m_workerThread;
		std::mutex m_workerMutex;
		std::string m_workerPath;
		std::chrono::milliseconds m_workerInterval;
		std::function<void(Response)> m_workerCallback;
		std::atomic_bool m_workerRunning = false;

		//This queue holds all messages to be sent to the remote side 
		// of this connection
		tsqueue<std::string> m_qMessagesOut;

		// This queue holds all messages that have been received from
		// the remote side of this connection. Note it is a reference 
		// as the "owner" of this connection is expected to provide a queue
		tsqueue<std::string>& m_qMessagesIn;

		uint32_t id = 0;
	};
}
