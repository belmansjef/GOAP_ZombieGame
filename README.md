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
        <li><a href="#actions">Actions</a></li>
        <li><a href="#states">WorldState</a></li>
        <li><a href="#planner">Planner</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
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
The values denoted over the arrows are the cost to execute the action. Let's dive a bit deeper under the hood of the planner!
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
It then iterates over the list of actions, resets them and puts all the valid actions in a list. These are the actions the planner will use to try and formulate a valid plan. The A* search algorithm uses an `OpenList` and `ClosedList`, they contain nodes that still need to be checked and nodes that are already checked respectively.
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

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Implementation

Use this space to show useful examples of how a project can be used. Additional screenshots, code examples and demos work well in this space. You may also link to more resources.

_For more examples, please refer to the [Documentation](https://example.com)_

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [ ] Feature 1
- [ ] Feature 2
- [ ] Feature 3
    - [ ] Nested Feature

See the [open issues](https://github.com/belmansjef/GOAP_ZombieGame/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the GNU General Public License v3.0. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Your Name - [@twitter_handle](https://twitter.com/twitter_handle) - email@email_client.com

Project Link: [https://github.com/belmansjef/GOAP_ZombieGame](https://github.com/belmansjef/GOAP_ZombieGame)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* []()
* []()
* []()

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/belmansjef/GOAP_ZombieGame.svg?style=for-the-badge
[contributors-url]: https://github.com/belmansjef/GOAP_ZombieGame/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/belmansjef/GOAP_ZombieGame.svg?style=for-the-badge
[forks-url]: https://github.com/belmansjef/GOAP_ZombieGame/network/members
[stars-shield]: https://img.shields.io/github/stars/belmansjef/GOAP_ZombieGame.svg?style=for-the-badge
[stars-url]: https://github.com/belmansjef/GOAP_ZombieGame/stargazers
[issues-shield]: https://img.shields.io/github/issues/belmansjef/GOAP_ZombieGame.svg?style=for-the-badge
[issues-url]: https://github.com/belmansjef/GOAP_ZombieGame/issues
[license-shield]: https://img.shields.io/github/license/belmansjef/GOAP_ZombieGame.svg?style=for-the-badge
[license-url]: https://github.com/belmansjef/GOAP_ZombieGame/blob/main/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/linkedin_username
[product-screenshot]: images/screenshot.png
[cpp]: https://img.shields.io/badge/CPlusPlus-000000?style=for-the-badge&logo=cplusplus&logoColor=white
[cpp-url]: https://en.cppreference.com/w/cpp/language
[React.js]: https://img.shields.io/badge/React-20232A?style=for-the-badge&logo=react&logoColor=61DAFB
[React-url]: https://reactjs.org/
[Vue.js]: https://img.shields.io/badge/Vue.js-35495E?style=for-the-badge&logo=vuedotjs&logoColor=4FC08D
[Vue-url]: https://vuejs.org/
[Angular.io]: https://img.shields.io/badge/Angular-DD0031?style=for-the-badge&logo=angular&logoColor=white
[Angular-url]: https://angular.io/
[Svelte.dev]: https://img.shields.io/badge/Svelte-4A4A55?style=for-the-badge&logo=svelte&logoColor=FF3E00
[Svelte-url]: https://svelte.dev/
[Laravel.com]: https://img.shields.io/badge/Laravel-FF2D20?style=for-the-badge&logo=laravel&logoColor=white
[Laravel-url]: https://laravel.com
[Bootstrap.com]: https://img.shields.io/badge/Bootstrap-563D7C?style=for-the-badge&logo=bootstrap&logoColor=white
[Bootstrap-url]: https://getbootstrap.com
[JQuery.com]: https://img.shields.io/badge/jQuery-0769AD?style=for-the-badge&logo=jquery&logoColor=white
[JQuery-url]: https://jquery.com 

# GOAP_ZombieGame
Research done on GOAP (Goal Oriented Action Planning) applied to a zombie game developed using an in-house engine of DAE, a course at Howest, Belgium.

## SOURCES ##
Websites:

https://alumni.media.mit.edu/~jorkin/goap.html
https://jaredemitchell.com/goal-oriented-action-planning-research/
https://medium.com/@vedantchaudhari/goal-oriented-action-planning-34035ed40d0b
https://gamedevelopment.tutsplus.com/tutorials/goal-oriented-action-planning-for-a-smarter-ai--cms-20793

Github:
https://github.com/crashkonijn/GOAP

Videos:
https://www.youtube.com/watch?v=Q7aHXn_LypI
https://www.youtube.com/watch?v=gm7K68663rA
