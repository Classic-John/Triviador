#pragma once

namespace net {
	enum class ConnectionStatus {
		PENDING_CONNECTION,
		CONNECTED,
		CONNECTION_LOST,
		DISCONNECTED
	};
}