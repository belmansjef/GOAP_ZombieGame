<a name="readme-top"></a>
<!-- PROJECT SHIELDS -->
[![MIT License][license-shield]][license-url]

<!-- PROJECT LOGO -->
<br />
<div align="center">
<h2 align="center">GOAP Survival Agent</h2>

  <p align="center">
    Making an AI agent survive a virtual wasteland using Goal-Oriented Action Planning.
    <br />
    <a href="https://github.com/belmansjef/GOAP_ZombieGame"><strong>View demo »</strong></a>
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-this-project">About This Project</a>
    </li>
    <li>
      <a href="#what-is-goap">What Is GOAP?</a>
      <ul>
  		<li><a href="#aad">Advantages And Disadvantages</a></li>
		<li><a href="#actions">Actions</a></li>
        <li><a href="#states">WorldState</a></li>
        <li><a href="#planner">Planner</a></li>
      </ul>
    </li>
    <li><a href="#conclusion">Conclusion</a></li>
    <li><a href="#license">License</a></li>
	<li><a href="#sources">Sources</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>

<!-- ABOUT THIS PROJECT -->
## About This Project

This project implements <a href="https://alumni.media.mit.edu/~jorkin/goap.html">Jeff Orkin's Goal-Oriented Action Planning</a> to help an AI agent survive a virtual 2D wasteland plagued with zombies coming for you (well, your brains to be more precise, if classic comics are to be believed).

Writen in C++, using the in-house engine developed by a few talented <a href="https://www.digitalartsandentertainment.be/">DAE</a> profs.

### Built With

[![cpp][cpp]][cpp-url]
<!-- What Is GOAP -->
## What Is GOAP?

If you were to ask a few hundred game developers to write down the most common way to implement AI in games, <a href="https://en.wikipedia.org/wiki/Finite-state_machine">Finite State Machines</a> (FSM) and <a href="https://en.wikipedia.org/wiki/Behavior_tree">Behaviour Trees</a> (BT) would surely top the charts. Almost every gameplay programmer has used, or at least heard of, Finite State Machines.

Goal-Oriented Action Planing (GOAP, rhymes with dope) is a planning architecture based on <a href="http://web.mit.edu/caelan/www/publications/rss_workshop_2017.pdf">MIT's STRIPS planning</a>. Developed by <a href="http://alumni.media.mit.edu/~jorkin/">Jeff Orkin</a> while he was working at <a href="https://www.lith.com/">Monolith Productions</a>. GOAP was first implemented in <a href="https://en.wikipedia.org/wiki/F.E.A.R.">F.E.A.R.</a>, which back in 2005 was praised for it's intelligent AI and squad behaviour in which agents could seemingly verbally communicate with eachother (although the latter was all <a href="https://alumni.media.mit.edu/~jorkin/gdc2006_orkin_jeff_fear.pdf#page=16">smoke and mirrors</a>).

Compared to a traditional FSM where you would need to define every single transition between every single state, which gets tedious and cumbersome real fast as the number of states grow, GOAP's FSM only ever uses three states.

Following states describe a GOAP's FSM:
* An Idle state, this is where GOAP uses the provided actions, current world state and goal state to formulate a plan.
* A MoveTo state, this moves the agent towards the object it needs to interact with, like moving towards an item to pick up or a switch that needs to be flipped.
* An Execution state, this is when the agent executes the actual action, like picking up the item or flipping the switch.

But how could a FSM with only three states navigate an agent through a wasteland whilst fighting of zombies, making sure to avoid starvation, I hear you ask? This is where the planning comes in to play. By giving the <a href="actions">planner</a> a list of <a href="actions">actions</a>, the current <a href="states">state</a> of the world and a desired goal state, it will compute a sequence of actions that will achieve said desired goal state.
<br><br>
GOAP saved this project from a FSM spiderweb like the one below:
<br>
<img src="https://github.com/belmansjef/GOAP_ZombieGame/blob/main/README/FSM.png" alt="A spiderweb FSM graph with states going all over the place">
<br>
And it nicely decoupled everything to be easily maintainable:
<br>
<img src="https://github.com/belmansjef/GOAP_ZombieGame/blob/main/README/GOAP.png" alt="A nice and clean list of GOAP actions">
<br>

So you get the point, GOAP is a real life saver when dealing with AI that requires more than 4 states.

<a name="aad"></a>
### Advantages And Disadvantages
Before diving deeper into GOAP, we must first have it about who GOAP is for and who it's not.
One of the major advantages of GOAP is it's ease of use, expandability and maintainability.
Since every action lives on it's own and no transitions need to be defined between them, it's real easy to add more actions even late down the pipeline of development.
GOAP makes the agent feel more "organic" and less predictable. The last might also be one of the disadvantages, depending on the type of application you're developing. In a fast-paced FPS you probably want the enemies to be predictable to ensure a nice, fast flow. For an agent that will only ever have a few states, GOAP is a bit *overkill*. In that case, I'd be faster to write a little FSM.

#### Advantages:
* Ease Of Use
* Expendable
* Maintainable
* More organic looking agents

#### Disdvantages:
* Unpredictable
* Overkill for a small agent

<a name="actions"></a>
### Actions

An action is a little snippet, a step in a plan to achieve a goal. A few examples of actions in this project are picking up a medkit, moving towards the closest unexplored grid cell, destroying garbage laying on the ground, etc. All actions are encapsulated and **no two actions are ever aware of eachother**. They all have their own variables and methods. Each action also has *effects* and *preconditions* which the planner uses to formulate a valid plan to achieve the given goal.
<br>
<br>
An action in this project has the following constructor, defining the actions *preconditions* and *effects*:
```cpp
GOAP::Action_GrabMedkit::Action_GrabMedkit()
	: BaseAction("Grab Medkit", 5)
{
	SetPrecondition("medkit_aquired", true);
	SetPrecondition("medkit_in_inventory", false);
	SetEffect("medkit_in_inventory", true);
}
```
All actions are derived from the BaseAction class. The BaseAction constructor gets called in the derived action's constructor to set its name (for debugging purposes) and a preset cost value.

To give another example, if the agent needs to eliminate a zombie, they must make sure that all *preconditions* to eliminate the zombie are met. These *preconditions* could be that they need to have a weapon equiped and are within shooting range or they have their fists readied and are within melee range. The *effect* of succesfully executing the action could be that the zombie threat is eliminated. But at the moment, they don't have a weapon equiped. They could go look for one in a nearby house and eliminate the zombie with the retrieved weapon. Searching the house has the *effect* of having a weapon equiped. They might just use their fists and move into melee range to eliminate the zombie. How would the planner know which sequence of actions to take when their are multiple different actions to achieve the same goal? This is determined by an action's cost, the planner uses this to find the lowest cost path. Basic actions can have a preset cost value, but better, more dynamic actions, have a procedural cost value.
<br>
<br>
`Action_GrabMedkit` described above has following procedural cost method:
```cpp
int GOAP::Action_GrabMedkit::GetCost() const
{
	return static_cast<int>(m_pTarget->Location.Distance(m_AgentInfo.Position));
}
```
It's a simple method that calculates the distance between itself and its target (an aquired medkit in this case) and returns that distance as the cost. So the further an item, the higher the cost of `Action_GrabMedkit`.
<br>
<br>
Picture this scenario: the agent is shooting at a horde of zombies and their weapon runs out of ammo. They previously aquired a weapon in a house a few kilometers back, but couldn't carry it at that time since their backpack was full. It would be of great risk to try and run all the way back to grab that weapon as zombies might swarm them. But if they succeed, they are guaranteed to have a weapon equiped. There's also an unlooted house a few hundred meters away. They might just try their luck and search it for a weapon, though no guarantees. In this case, you could give both actions of moving to a previously aquired weapon and searching a new house a procedural cost based on the distance to the agent and the risk factor of searching a new house.
<br>
<br>
Besides a procedural cost, an action can also have a procedural validity check. This is used by the <a href="planner">planner</a> before the action's *preconditions* and *effects* are checked to see if the action is able to operate on the world state. Bringing back `Action_GrabMedkit` as an example: the agent might have aquired a medkit and their inventory is not full, so the action's preconditions are met. But the action's `IsValid()` method will check if the pathfinder can find a valid path towards the medkit, if not, the action is not valid. You could, each frame, make the pathfinder try and find a path towards all aquired items and store the result in the world's state. You could then use the action's *preconditions* to check against these results, but you can already tell that this has diminishing returns on performance as the number of aquired items grows.

<a name="states"></a>
### World And Goal State

Not to be confused with a FSM's state, world and goal states are a list of (mostly boolean) parameters describing the current and desired state of the world respectively. For this project, a simplified version of the world state looks something like the following:
```cpp
{
    {"pistol_in_inventory"  : true},
    {"shotgun_in_inventory" : false},
    {"medkit_in_inventory"  : true},
    {"food_in_inventory"    : true},
    {"low_health"           : false},
    {"low_energy"           : false},
    {"in_danger"            : false}
}
```
Goal states also have a priority value and a method to check if they're valid. The same as with actions, all goals have a preset priority value whilst some might have a procedural priority value. GOAP checks the list of goals from highest to lowest priority and sets the first goal that is valid as the current goal.
To give an example, the "Eliminate Threat" goal might look something like this:
```cpp
struct Goal_EliminateThreat final : WorldState
{
public:
    Goal_EliminateThreat() : WorldState("Eliminate Threat", 160)
    {
        SetVariable("in_danger", false);
    };
    virtual bool IsValid(const WorldState& ws) const override;
};
```
The goal struct is derived from a WorldState struct, the constructor calls the base struct constructor to initialize its name (used for debugging) and priority.
Method `IsValid()` is used by GOAP to determine wether the goal should be considered. `Goal_EliminateThreat` has a higher priority than say `Goal_LootHouse`, but if the agent is not in danger, there is no need to try and satisfy `Goal_EliminateThreat` since it's already satisfied.

The `IsValid()` method for `Goal_EliminateThreat` is shown below:
```cpp
bool GOAP::Goal_EliminateThreat::IsValid(const WorldState& ws) const
{
    return ws.GetVariable("in_danger") &&
        (ws.GetVariable("pistol_in_inventory") || ws.GetVariable("shotgun_in_inventory") ||
         ws.GetVariable("pistol_aquired") || ws.GetVariable("shotgun_aquired"));
}
```
The parameter `const WorldState& ws` is the current state of the world. The method checks the world state if the agent is in any danger and if they have a weapon equiped or has *knowledge* of a weapon on the ground. In the case that they have a weapon equiped, they'll start shooting at the enemy. If they don't have a weapon equiped, they'll first move to and pickup the weapon they previously aquired and then start shooting at the enemy.
<a name="planner"></a>
### Planner

The planner is the GOAP's brain. It chews on all available actions, the current world state and the goal world state to spit out a sequence of actions. The planner uses one of the most popular pathfinding algorithms: the <a href="https://en.wikipedia.org/wiki/A*_search_algorithm">A* search algorithm</a>.
It can not only be used to find the shortest and lowest cost path to a target, it can be used on any graph or list. The algorithm uses the current world state as its "starting position" and finds a "path" towards the goal world state. Below is an example of how the planner tries and find the cheapest path towards its goal:
<br>
<br>
<img src="https://github.com/belmansjef/GOAP_ZombieGame/blob/main/README/GOAP_PlannerPath.png" alt="A graph showing every possible path towards a goal">
<br>
> The values denoted over the arrows are the cost to execute the action.

<br>

Let's dive a bit deeper under the hood of the planner!
The planner has a method `FormulatePlan()` that returns a sequence of actions which takes the current state of the world, the desired "goal" state of the world and a list of actions.

```cpp
std::vector<GOAP::BaseAction*> FormulatePlan(const WorldState& start, const WorldState& goal, std::vector<BaseAction*>& actions);
```

`FormulatePlan()` starts with making sure that the provided goal isn't already met:
```cpp
if (start.MeetsGoal(goal))
{
    std::cout << "Goal [" << goal.name << "] already meets current WorldState!" << std::endl;
    return std::vector<BaseAction*>();
}
```
It then iterates over the list of actions, resets them and puts all the valid actions in a list. These are the actions the planner will use to try and formulate a valid plan. 
```cpp
std::vector<BaseAction*> usable_actions;
for (auto& action : actions)
{
    action->Reset();
    if (action->IsValid(pBlackboard))
    {
        usable_actions.push_back(action);
    }
}
```
The A* search algorithm uses an `OpenList` and `ClosedList`, they contain nodes that still need to be checked and nodes that are already checked respectively.
Both lists get cleared and the first node gets added to the `OpenList`:
```cpp
// Create the first node and push it to the openlist, the list of yet to be checked actions
Node starting_node{ start, 0, CalculateHeuristic(start, goal), 0, nullptr};
m_OpenList.push_back(std::move(starting_node));
```
The planner method `CalculateHeuristic()` calculates the "distance" from the current node to the destination node. In the case of GOAP, it's the "difference" of variables between the current and goal state.
```cpp
int GOAP::WorldState::DistanceTo(const WorldState& goal_state) const
{
    int result{};

    // Loop through each variable in the goal WorldState we want to achieve
    for (const auto& kv : goal_state.vars)
    {
        auto it = vars.find(kv.first);
        if (it == end(vars) || it->second != kv.second)
        {
            // This WorldState does not have or doesn't match with the goal WorldState variable
            ++result;
        }
    }

    return result;
}
```
While the `OpenList` contains nodes, the planner puts the first node on the closed list and keeps a reference to it. The `OpenList` gets sorted by f-cost, ascending, so the first element of the `OpenList` has the lowest f-cost. The f-cost, or "full"-cost, is the combined cost of the cost that got us to this node and the calculated cost to the destination node.
```cpp
// Get the node with the lowest f-cost, put it on the closed list
// and get a reference to it
Node& current(PopAndClose());
```
The planner then checks if the current node's state satisfies the goal and returns the action plan if it does.
```cpp
// Does the current state meet the goal's state?
if (current.ws.MeetsGoal(goal))
{
	std::vector<BaseAction*> plan;
	do
	{
		plan.emplace_back(current.action);
		
		// Search parent node on open list
		auto it = std::find_if(begin(m_OpenList), end(m_OpenList), [&](const Node& n) { return n.id == current.parent_id; });
		
		// Parent node is not on the open list, search on closed list
		if (it == end(m_OpenList))
		{
			it = std::find_if(begin(m_ClosedList), end(m_ClosedList), [&](const Node& n) { return n.id == current.parent_id; });
		}
		current = *it;
	} while (current.parent_id != 0); // Walk back through the action sequence until we get to the start node

	return plan;
}
```
If the current node's state doesn't satisfy the goal state, the planner loops over all available actions. It then checks if the action's preconditions are met, if so, it applies it's effects to a copy of the current world state. It checks this copy of the world state with the nodes on the closed list, if the closed list already contains this exact world state, it skips the current action.
```cpp
 WorldState outcome = potential_action->ActOn(current.ws);

// Skip if already closed -- in other words, we've already got an action that gets us here
if (IsMemberOfClosed(outcome)) continue;
```
We then check the `OpenList` for a node with the current world state. If there is none, we create a new node with this world state and the current node as its parent and push it on the `OpenList`.
```cpp
// Look for a Node with this WorldState on the open list.
auto potential_outcome_node = IsMemberOfOpen(outcome);
if (potential_outcome_node == end(m_OpenList))
{ 
	// Not a member of open list
    // Make a new node, with current node as its parent, recording G & H
    Node newNode(outcome, current.g + potential_action->GetCost(), CalculateHeuristic(outcome, goal), current.id, potential_action);
    // Add it to the open list (maintaining sort-order therein)
    AddToOpenList(std::move(newNode));
}
```
But if there's already a node with the current world state on the open list, we check if the current path is cheaper. If so, we update that node with our current, cheaper values. Lastely, we sort the open list on the f-cost, ascending.
```cpp
else
{
	// already a member of the open list
	// check if the current G is better than the recorded G
	if (current.g + potential_action->GetCost() < potential_outcome_node->g)
	{    
		potential_outcome_node->parent_id = current.id; // make current its parent
		potential_outcome_node->g = current.g + potential_action->GetCost(); // recalc G & H
		potential_outcome_node->h = CalculateHeuristic(outcome, goal);
		potential_outcome_node->action = potential_action;

		// resort open list to account for the new F
		// sorting likely invalidates the potential_outcome_node iterator, but we don't need it anymore
		std::sort(begin(m_OpenList), end(m_OpenList));
	}
}
```

And that's it! the planner repeats these steps untill the open list contains no more nodes. It'll then either have an optimal path or no path at all. In case of the latter, the main gameplay logic catches this and acts accordingly.
<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- Conclusion -->
## Conclusion
Now that I know how GOAP works and how it can be applied to applications and games, I don't see a reason why I'd ever use a FSM again for big and complex projects that don't require predicatble agents. There is still a lot to be improved upon in this project. [Fuzzy Logic](https://en.wikipedia.org/wiki/Fuzzy_logic) could be used to determine which goal is to be set as the goal to achieve. Fuzzy Logic is an extend of [Boolean Logic](https://en.wikipedia.org/wiki/Boolean_algebra) of sorts. Where Boolean Logic only has two values (false and true), Fuzzy Logic can have multiple values. Something can for example be "one third true" or "three fifth false". If you were to apply Fuzzy Logic to this project, you could for example state what the agent needs to do at certain health values. If the agent is "moderately hit" (health at 80%) and "very hungry" (energy at 20%), they could decide to first satisfy their hunger and then go check for a medkit to heal their wounds. This is just a vague (ha, get it?) example of how Fuzzy Logic could be incorperated into this project.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->
## License

Distributed under the GNU General Public License v3.0. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- SOURCES -->
## Sources

* [I had heard of GOAP, but it was only a few weeks ago that I started diving into it. Jeff Orkin's website, the godfather of GOAP, has been my main source of knowledge.](https://alumni.media.mit.edu/~jorkin/goap.html)
* [This document writen by Jeff Orkin himself is what got me to understand the general idea of GOAP and how it works.](https://alumni.media.mit.edu/~jorkin/gdc2006_orkin_jeff_fear.pdf)
* [Big thanks to Chris Powell's implementation of GOAP. His examples really helped me get an idea of GOAPS general logic.](https://github.com/cpowell/cppGOAP)
* [This blog with Unity demo helped me get a great understanding of procedural preconditions.](https://gamedevelopment.tutsplus.com/tutorials/goal-oriented-action-planning-for-a-smarter-ai--cms-20793)
* [This GDC video from 2017 gave a nice insight into how the big studios implement and use GOAP in their games.](https://youtu.be/gm7K68663rA)
* [This video by TheHappieCat did a real good job of explaining how every component of GOAP works and might be implemented.](https://youtu.be/n6vn7d5R_2c)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

Belmans Jef - belmansjef@gmail.com

Project Link: [https://github.com/belmansjef/GOAP_ZombieGame](https://github.com/belmansjef/GOAP_ZombieGame)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[license-shield]: https://img.shields.io/github/license/belmansjef/GOAP_ZombieGame.svg?style=for-the-badge
[license-url]: https://github.com/belmansjef/GOAP_ZombieGame/blob/main/LICENSE.txt
[cpp]: https://img.shields.io/badge/CPlusPlus-000000?style=for-the-badge&logo=cplusplus&logoColor=white
[cpp-url]: https://en.cppreference.com/w/cpp/language
