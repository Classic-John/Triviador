#include "MapFactory.h"

namespace model {
	MapFactory::MapFactory(std::istream& in)
		: m_mapTemplate(std::make_unique<model::Map>())
	{
		// TODO validate read input
		// Read number of territories
		unsigned int numTerritories;
		in >> numTerritories;
		m_mapTemplate->m_territories.reserve(numTerritories);
		// Create that many territories with only the index assigned
		for (unsigned int idx = 0; idx < numTerritories; ++idx) {
			m_mapTemplate->m_territories.emplace_back(nullptr, 0, idx);
		}
		// Read the adjacency list
		unsigned int numAdjacentTerritories;
		unsigned int adjacentIndex;
		for (unsigned int idx = 0; idx < numTerritories; ++idx) {
			in >> numAdjacentTerritories;
			for (unsigned int i = 0; i < numAdjacentTerritories; ++i)
			{
				in >> adjacentIndex;
				m_mapTemplate->m_adjList.push_back(adjacentIndex);
			}
			m_mapTemplate->m_adjIterators.push_back(m_mapTemplate->m_adjList.size() - numAdjacentTerritories);
		}
		m_mapTemplate->m_adjIterators.push_back(m_mapTemplate->m_adjList.size());
		// Assign every territory to no owner in the cache
		auto& noOwnerCache = m_mapTemplate->m_ownedTerritoriesCache[nullptr];
		for (unsigned int idx = 0; idx < numTerritories; ++idx)
			noOwnerCache.insert(idx);
	}

	model::Map MapFactory::createMap()
	{
		return *m_mapTemplate;
	}
}
