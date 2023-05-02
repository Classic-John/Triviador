#pragma once

#include <vector>
#include <stdint.h>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <ranges>
#include <set>

class Player;

namespace model {

	class Territory {
		friend class Map;
		friend class MapFactory;

		Territory& operator=(const Territory&) = delete;
		Territory& operator=(Territory&&) = delete;

	public:
		Territory(Territory&&) = default; // FIXME copy should not change m_index and m_map (or shouldn't be possible at all, but containers need it)
		Territory(const Territory&) = default; // FIXME copy should not change m_index and m_map (or shouldn't be possible at all, but containers need it)

		uint32_t score() const noexcept;

		Player* owner() noexcept;
		const Player* owner() const noexcept;

		size_t index() const noexcept;
	public:
		Territory(Player* owner, int score, size_t index)
			: m_owner(owner), m_score(score), m_index(index)
		{}
	public:
		Player* m_owner;
		uint32_t m_score;
		size_t m_index;
	};

	class Map {
		friend class MapFactory;
		friend Territory;
	private:
		using TerritoriesContainerType = std::vector<Territory>;
		using AdjacencyListContainerType = std::vector<size_t>;
		using AdjacencyListIteratorsContainerType = std::vector<AdjacencyListContainerType::value_type>;
		using OwnedTerritoriesContainerType = std::unordered_map<const Player*, std::unordered_set<size_t>>;
	public:
		using value_type = Territory;
		using reference = value_type&;
		using const_reference = const value_type&;
		using size_type = TerritoriesContainerType::size_type;
		using difference_type = TerritoriesContainerType::difference_type;
		using iterator = TerritoriesContainerType::iterator;
		using const_iterator = TerritoriesContainerType::const_iterator;
		using const_iterator_adjacent_territories = AdjacencyListContainerType::const_iterator;
		using const_iterator_owned_territories = OwnedTerritoriesContainerType::mapped_type::const_iterator;
		// FIXME functions marked inline in Map.cpp have to be moved here to avoid linker errors
		reference operator[](const size_type index) noexcept { return m_territories[index]; }
		const_reference operator[](const size_type index) const noexcept { return const_cast<Map*>(this)->operator[](index); }
		iterator iter(const size_type index) noexcept { return begin() + index; }
		size_type size() const noexcept { return m_territories.size(); };
		iterator begin() noexcept { return m_territories.begin(); }
		iterator end() noexcept { return m_territories.end(); }
		const_iterator begin() const noexcept;
		const_iterator end() const noexcept;
		const_iterator_adjacent_territories begin(const size_type index) const noexcept;
		const_iterator_adjacent_territories end(const size_type index) const noexcept;
		const_iterator_owned_territories begin(const Player* const player) const noexcept;
		const_iterator_owned_territories end(const Player* const player) const noexcept;

		auto adjacentTerritories(const Territory& territory) const noexcept
		{
			return std::ranges::subrange(begin(territory.index()), end(territory.index())) |
				std::views::transform([&](const auto& index) -> const Territory& { return (*this)[index]; });
		}

		auto ownedTerritories(const Player* player) const noexcept
		{
			if (auto iter = m_ownedTerritoriesCache.find(player); iter != m_ownedTerritoriesCache.end())
				return std::ranges::ref_view(iter->second);
			else
				return std::ranges::ref_view(EMPTY_OWNED_TERRITORIES);
		}

		auto selectableBaseTerritories(const Player* player) const noexcept
		{
			auto unoccupiedTerritoriesView = ownedTerritories(nullptr);
			std::vector<size_t> unoccupiedTerritories(unoccupiedTerritoriesView.begin(), unoccupiedTerritoriesView.end());
			return unoccupiedTerritories;
		}

		auto selectableUnoccupiedTerritories(const Player* player) const noexcept
		{
			auto playerTerritoriesView = ownedTerritories(player);
			std::set<size_t> selectableTerritories;
			for (auto territoryIdx : playerTerritoriesView)
			{
				auto& territory = (*this)[territoryIdx];
				auto adjacentTerritories = this->adjacentTerritories(territory);
				for (auto& adjacentTerritory : adjacentTerritories)
				{
					if (!adjacentTerritory.owner())
						selectableTerritories.insert(adjacentTerritory.index());
				}
			}

			if (selectableTerritories.empty())
			{
				auto unoccupiedTerritoriesView = ownedTerritories(nullptr);
				selectableTerritories.insert(unoccupiedTerritoriesView.begin(), unoccupiedTerritoriesView.end());
			}

			return selectableTerritories;
		}

		auto attackableTerritories(const Player* player) const noexcept
		{
			auto playerTerritoriesView = ownedTerritories(player);
			std::set<size_t> neighbouringTerritories;
			for (auto territoryIdx : playerTerritoriesView)
			{
				auto& territory = (*this)[territoryIdx];
				auto adjacentTerritories = this->adjacentTerritories(territory);
				for (auto& adjacentTerritory : adjacentTerritories)
					if (adjacentTerritory.owner() != player)
						neighbouringTerritories.insert(adjacentTerritory.index());
			}

			return neighbouringTerritories;
		}

		void setOwnerOf(Territory& territory, Player* owner);
		void setScoreOf(Territory& territory, uint32_t score);
	public:
		Map();
	private:
		TerritoriesContainerType m_territories;
		AdjacencyListContainerType m_adjList;
		AdjacencyListIteratorsContainerType m_adjIterators;
		OwnedTerritoriesContainerType m_ownedTerritoriesCache;
		static const OwnedTerritoriesContainerType::mapped_type EMPTY_OWNED_TERRITORIES;
	};
}