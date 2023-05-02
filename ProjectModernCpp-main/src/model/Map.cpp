#include "Map.h"

#include <assert.h>

namespace model {

	uint32_t Territory::score() const noexcept { return m_score; }

	Player* Territory::owner() noexcept { return m_owner; }
	const Player* Territory::owner() const noexcept { return m_owner; }

	size_t Territory::index() const noexcept { return m_index; }

	Map::Map()
	{}

	const Map::OwnedTerritoriesContainerType::mapped_type Map::EMPTY_OWNED_TERRITORIES;

	void Map::setOwnerOf(Territory& territory, Player* owner)
	{
		if (territory.owner() == owner)
			return;

		// Remove territory from old owner
		auto oldOwnerCacheIter = m_ownedTerritoriesCache.find(territory.owner());
		auto& territoriesOfOldOwner = oldOwnerCacheIter->second;
		territoriesOfOldOwner.erase(territory.index());
		if (territoriesOfOldOwner.empty())
			m_ownedTerritoriesCache.erase(oldOwnerCacheIter);

		// Add territory to new owner
		m_ownedTerritoriesCache[owner].insert(territory.index());
		territory.m_owner = owner;
	}

	void Map::setScoreOf(Territory& territory, uint32_t score)
	{
		territory.m_score = score;
	}

	Map::const_iterator Map::begin() const noexcept { return m_territories.begin(); }
	Map::const_iterator Map::end() const noexcept { return m_territories.end(); }

	Map::const_iterator_adjacent_territories Map::begin(const size_type index) const noexcept
	{
		assert(index < size());
		return m_adjList.begin() + m_adjIterators[index];
	}
	Map::const_iterator_adjacent_territories Map::end(const size_type index) const noexcept
	{
		assert(index < size());
		assert(index + 1 < m_adjIterators.size());
		return m_adjList.begin() + m_adjIterators[index + 1];
	}

	Map::const_iterator_owned_territories Map::begin(const Player* player) const noexcept
	{
		if (auto iter = m_ownedTerritoriesCache.begin(); iter != m_ownedTerritoriesCache.end())
			return iter->second.begin();
		else
			return const_iterator_owned_territories{};
	}
	Map::const_iterator_owned_territories Map::end(const Player* player) const noexcept
	{
		if (auto iter = m_ownedTerritoriesCache.begin(); iter != m_ownedTerritoriesCache.end())
			return iter->second.end();
		else
			return const_iterator_owned_territories{};
	}
}