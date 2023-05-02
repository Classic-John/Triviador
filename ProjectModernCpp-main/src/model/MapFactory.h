#pragma once

#include "Map.h"

#include <istream>
#include <memory>

namespace model {
	class MapFactory
	{
	public:
		MapFactory(std::istream& in);

		model::Map createMap();

	private:
		std::unique_ptr<model::Map> m_mapTemplate;
	};
}
