#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace soup
{
	// QueryInterface needs the following functions:
	//   static uint64_t getUniqueId(const Node& node)
	//   static float getDistance(const Node& a, const Node& b)
	//   static float getHeuristicScore(const Node& node, const Node& end)
	//   static std::vector<Node> getNeighbours(const Node& node)
	// Node can be whatever, including a pointer -- in which case the QueryInterface may take `Node` instead of `const Node&`.
	template <typename QueryInterface, typename Node, typename NodeId = uint64_t, typename Score = float>
	struct Pathfind
	{
		// The bool indicates if a route was successfully found. If false, a route may still be present, but it will not visit `end`.
		// Note that in the resulting route, `start` is `.back()` and not `.at(0)`.
		static std::pair<bool, std::vector<Node>> route(Node start, Node end)
		{
			RouteState rs(std::move(start), std::move(end));
			while (!rs.isFinished())
			{
				rs.partialTick();
			}
			if (!rs.result.first) // No route found?
			{
				rs.populateRoughPath();
			}
			return rs.result;
		}

		class RouteState
		{
		public:
			Node end;
			const NodeId end_uid;
			std::vector<Node> open_set;
			std::unordered_map<NodeId, Node> came_from;
			std::unordered_map<NodeId, float> g_score;
			std::unordered_map<NodeId, float> f_score;

			std::pair<bool, std::vector<Node>> result{ false, {} };

			RouteState(Node _start, Node _end)
				: end(std::move(_end)), end_uid(QueryInterface::getUniqueId(end)), open_set{ std::move(_start) }
			{
				const Node& start = open_set.at(0);
				const auto uid = QueryInterface::getUniqueId(start);
				g_score.emplace(uid, Score{});
				f_score.emplace(uid, QueryInterface::getHeuristicScore(start, end));
			}

			[[nodiscard]] bool isFinished() const noexcept
			{
				return open_set.empty();
			}

			void tick()
			{
				partialTick();
				if (isFinished()) // No route found?
				{
					populateRoughPath();
				}
			}

			void partialTick()
			{
				Node n = getNodeToLookAt();
				const auto uid = QueryInterface::getUniqueId(n);
				if (uid == end_uid)
				{
					open_set.clear();
					result = { true, getPath(std::move(n)) };
					return;
				}
				for (auto& neighbour : QueryInterface::getNeighbours(n))
				{
					auto tentative_g_score = getGScore(uid) + QueryInterface::getDistance(n, neighbour);
					const auto neighbour_uid = QueryInterface::getUniqueId(neighbour);
					if (tentative_g_score < getGScore(neighbour_uid))
					{
						// Best path to this neighbour yet!
						setCameFrom(neighbour_uid, n);
						setGScore(neighbour_uid, tentative_g_score);
						setFScore(neighbour_uid, tentative_g_score + QueryInterface::getHeuristicScore(neighbour, end));
						if (!isInOpenSet(neighbour_uid))
						{
							open_set.emplace_back(std::move(neighbour));
						}
					}
				}
			}

			void populateRoughPath()
			{
				if (!came_from.empty())
				{
					typename std::unordered_map<NodeId, Node>::iterator best_node;
					Score best_score = std::numeric_limits<Score>::infinity();
					for (auto i = came_from.begin(); i != came_from.end(); ++i)
					{
						const auto score = QueryInterface::getHeuristicScore(i->second, end);
						if (score < best_score)
						{
							best_node = i;
							best_score = score;
						}
					}
					result.second = getPath(std::move(best_node->second));
				}
			}

			[[nodiscard]] bool isInOpenSet(NodeId uid) const
			{
				for (const auto& n : open_set)
				{
					if (QueryInterface::getUniqueId(n) == uid)
					{
						return true;
					}
				}
				return false;
			}

			[[nodiscard]] float getGScore(NodeId uid) const
			{
				if (auto e = g_score.find(uid); e != g_score.end())
				{
					return e->second;
				}
				return std::numeric_limits<Score>::infinity();
			}

			void setCameFrom(NodeId uid, Node n)
			{
				if (auto e = came_from.find(uid); e != came_from.end())
				{
					e->second = std::move(n);
				}
				else
				{
					came_from.emplace(std::move(uid), std::move(n));
				}
			}

			void setGScore(NodeId uid, Score score)
			{
				if (auto e = g_score.find(uid); e != g_score.end())
				{
					e->second = std::move(score);
				}
				else
				{
					g_score.emplace(std::move(uid), std::move(score));
				}
			}

			void setFScore(NodeId uid, Score score)
			{
				if (auto e = f_score.find(uid); e != f_score.end())
				{
					e->second = std::move(score);
				}
				else
				{
					f_score.emplace(std::move(uid), std::move(score));
				}
			}

		protected:
			[[nodiscard]] Node getNodeToLookAt()
			{
				typename std::vector<Node>::iterator best_node;
				Score best_score = std::numeric_limits<Score>::infinity();
				for (auto i = open_set.begin(); i != open_set.end(); ++i)
				{
					const auto score = f_score.at(QueryInterface::getUniqueId(*i));
					if (score < best_score)
					{
						best_node = i;
						best_score = score;
					}
				}
				Node n = std::move(*best_node);
				open_set.erase(best_node);
				return n;
			}

			[[nodiscard]] std::vector<Node> getPath(Node n)
			{
				auto uid = QueryInterface::getUniqueId(n);
				std::vector<Node> path{ std::move(n) };
				while (true)
				{
					auto e = came_from.find(uid);
					if (e == came_from.end())
					{
						break;
					}
					uid = QueryInterface::getUniqueId(e->second);
					path.emplace_back(std::move(e->second));
				}
				return path;
			}
		};
	};
}

/* Example of pathfinding on a 2D grid:

#include <Canvas.hpp>
#include <console.hpp>
#include <Pathfind.hpp>
#include <Vector2.hpp>

static constexpr bool _ = false;
static constexpr bool O = true;
static constexpr uint32_t GRID_WIDTH = 10;
static constexpr uint32_t GRID_HEIGHT = 10;
static const bool grid[GRID_WIDTH * GRID_HEIGHT] = {
	_,_,_,_,_,_,_,_,_,_,
	_,_,_,_,_,_,_,_,_,_,
	_,_,_,_,_,_,_,_,_,_,
	_,_,_,_,_,_,_,_,_,_,
	_,_,_,_,_,_,O,_,_,_,
	_,_,_,_,_,_,O,_,_,_,
	_,_,_,_,_,_,O,_,_,_,
	_,_,_,_,_,_,O,_,_,_,
	_,_,_,_,_,_,O,_,_,_,
	_,_,_,_,_,_,_,_,_,_,
};

[[nodiscard]] static bool isBlockedOff(int32_t x, int32_t y) noexcept
{
	return x == -1 || x == GRID_WIDTH || y == -1 || y == GRID_HEIGHT || grid[x + y * GRID_HEIGHT];
}

struct GridNode
{
	int32_t x, y;

	uint64_t getUniqueId() const noexcept
	{
		return static_cast<uint64_t>(x) << 32 | y;
	}

	std::vector<GridNode> getNeighbours() const noexcept
	{
		std::vector<GridNode> neighbours;

		if (!isBlockedOff(x - 1, y - 1)) neighbours.emplace_back(x - 1, y - 1); // Top left
		if (!isBlockedOff(x, y - 1)) neighbours.emplace_back(x, y - 1); // Top centre
		if (!isBlockedOff(x + 1, y - 1)) neighbours.emplace_back(x + 1, y - 1); // Top right
		if (!isBlockedOff(x + 1, y)) neighbours.emplace_back(x + 1, y); // Centre right
		if (!isBlockedOff(x - 1, y)) neighbours.emplace_back(x - 1, y); // Centre left
		if (!isBlockedOff(x - 1, y + 1)) neighbours.emplace_back(x - 1, y + 1); // Bottom left
		if (!isBlockedOff(x, y + 1)) neighbours.emplace_back(x, y + 1); // Bottom centre
		if (!isBlockedOff(x + 1, y + 1)) neighbours.emplace_back(x + 1, y + 1); // Bottom right

		return neighbours;
	}
};

struct GridPathfind : public soup::Pathfind<GridPathfind, GridNode>
{
	using Node = GridNode;

	static uint64_t getUniqueId(const Node& node)
	{
		return node.getUniqueId();
	}

	static float getDistance(const Node& a, const Node& b)
	{
		return soup::Vector2(a.x, a.y).distance(soup::Vector2(b.x, b.y));
	}

	static float getHeuristicScore(const Node& node, const Node& end)
	{
		return getDistance(node, end);
	}

	static std::vector<Node> getNeighbours(const Node& node)
	{
		return node.getNeighbours();
	}
};

int main()
{
	auto r = GridPathfind::route(GridNode{ 0, 0 }, GridNode{ 9, 9 });
	soup::Canvas c(GRID_WIDTH, GRID_HEIGHT);
	for (int x = 0; x != GRID_WIDTH; ++x)
	{
		for (int y = 0; y != GRID_WIDTH; ++y)
		{
			if (isBlockedOff(x, y))
			{
				c.set(x, y, soup::Rgb::RED);
			}
		}
	}
	for (const auto& n : r.second)
	{
		c.set(n.x, n.y, soup::Rgb::WHITE);
	}
	soup::console.init(false);
	soup::console << c.toStringDownsampledDoublewidth(true);
}
*/
