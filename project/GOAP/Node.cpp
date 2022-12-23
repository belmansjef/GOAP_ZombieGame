#include "Node.h"
#include <iostream>

int GOAP::Node::last_id = 0;

GOAP::Node::Node()
    :g(0)
    , h(0)
{
    id = ++last_id;
}

GOAP::Node::Node(const WorldState& _ws, int _g, int _h, int _parent_id, const Action* _action)
    : ws(_ws)
    , g(_g)
    , h(_h)
    , parent_id(_parent_id)
    , action (_action)
{
    id = ++last_id;
}

bool GOAP::operator<(const GOAP::Node& lhs, const GOAP::Node& rhs)
{
    return lhs.f() < rhs.f();
}