#pragma once

#include <memory>

#include "BaseAction.h"
#include "WorldState.h"

namespace GOAP
{
	struct Node final
	{
		static int last_id;		// Static to assign incrementing, unique IDs to nodes

		WorldState ws;			// State of the world at this node
		int id;					// The unique ID of this node
		int parent_id;			// The ID of this node's predecessor
		int g;					// The cost from 'start' to 'here', used by A*
		int h;					// The heuristic, estimated remaining cost from 'here' to 'goal', used by A*
		BaseAction* action;	    // The action that got us here

		/// <summary>
		/// Constructs a node
		/// </summary>
		/// <param name="_ws">State of the world at this node</param>
		/// <param name="_g">The cost from 'start' to 'here', used by A*</param>
		/// <param name="_h">The heuristic, estimated remaining cost from 'here' to 'goal', used by A*</param>
		/// <param name="_parent_id">The id of the node we followed to get to this action</param>
		/// <param name="_action">The action that got us here</param>
		Node(const WorldState& _ws, const int _g, const int _h, const int _parent_id, BaseAction* _action);

		/// <summary>
		/// The full cost (g + h) of the A* path
		/// </summary>
		/// <returns>The full cost of the path through this node</returns>
		int f() const
		{
			return g + h;
		}

		friend std::ostream& operator<<(std::ostream& out, const Node& n);
	};

	bool operator<(const Node& lhs, const Node& rhs);

	inline std::ostream& operator<<(std::ostream& out, const Node& n)
	{
		out << "Node { id:" << n.id << " parent:" << n.parent_id << " F:" << n.f() << " G:" << n.g << " H:" << n.h;
		out << ", " << n.ws << "}";
		return out;
	}
}