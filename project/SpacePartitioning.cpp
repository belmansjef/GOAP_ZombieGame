#include "stdafx.h"
#include "SpacePartitioning.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
	: hasVisited(false)
	, center({left + width / 2.f, bottom + height / 2.f})
{
	boundingBox.bottomLeft = { left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	auto left = boundingBox.bottomLeft.x;
	auto bottom = boundingBox.bottomLeft.y;
	auto width = boundingBox.width;
	auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_CellWidth(width / static_cast<float>(cols))
	, m_CellHeight(height / static_cast<float>(rows))
{
	for (int x{ -m_NrOfRows / 2 }; x < m_NrOfRows / 2; ++x)
	{
		for (int y{ -m_NrOfCols / 2 }; y < m_NrOfCols / 2; ++y)
		{
			m_Cells.push_back(Cell(m_CellWidth * y, m_CellHeight * x, m_CellWidth, m_CellHeight));
			if ((abs(y) == 6 && x <= 6 && x >= -6) || abs(x) == 6 && y <= 6 && y >= -6 || (x >= -1 && x <= 0 && y >= -1 && y <= 0))
			{
				m_ExplorationPath.push_back(m_Cells.back());
			}
		}
	}
}

CellSpace& CellSpace::operator=(const CellSpace& other)
{
	m_SpaceWidth = other.m_SpaceHeight;
	m_SpaceHeight = other.m_SpaceHeight;
	m_NrOfRows = other.m_NrOfRows;
	m_NrOfCols = other.m_NrOfCols;
	m_CellWidth = other.m_CellWidth;
	m_CellHeight = other.m_CellHeight;
	m_Cells = other.m_Cells;
	m_ExplorationPath = other.m_ExplorationPath;

	return *this;
}

std::vector<Cell> CellSpace::GetNeighbouringCells(int index) const
{
	int row{ index / m_NrOfCols };
	int col{ index % m_NrOfCols };
	return
	{
		m_Cells[++row * m_NrOfCols + --col],
		m_Cells[row * m_NrOfCols + ++col],
		m_Cells[row * m_NrOfCols + ++col],
		m_Cells[--row * m_NrOfCols + col],
		m_Cells[--row * m_NrOfCols + col],
		m_Cells[row * m_NrOfCols + --col],
		m_Cells[row * m_NrOfCols + --col],
		m_Cells[++row * m_NrOfCols + col]
	};
}

std::vector<Cell> CellSpace::GetCells() const
{
	return m_Cells;
}

std::vector<Cell> CellSpace::GetPath() const
{
	return m_ExplorationPath;
}

int CellSpace::PositionToIndex(const Elite::Vector2 pos) const
{
	for (int i{ 0 }; i < static_cast<int>(m_Cells.size()); ++i)
	{
		if (
			pos.x >= m_Cells[i].boundingBox.bottomLeft.x &&
			pos.x <= m_Cells[i].boundingBox.bottomLeft.x + m_Cells[i].boundingBox.width &&
			pos.y >= m_Cells[i].boundingBox.bottomLeft.y &&
			pos.y <= m_Cells[i].boundingBox.bottomLeft.y + m_Cells[i].boundingBox.height
			)
		{
			return i;
		}
	}
	return 0;
}

Cell CellSpace::GetClosestCellOnPath(const Elite::Vector2& agentPos) const
{
	Cell closestCell{ m_Cells[(m_NrOfRows / 2 - 1) * m_NrOfCols + (m_NrOfCols / 2 - 1)]};
	float closestDist{ FLT_MAX };
	for (auto& cell : m_ExplorationPath)
	{
		const float dist{ cell.center.DistanceSquared(agentPos) };
		if (dist < closestDist && !cell.hasVisited)
		{
			closestCell = cell;
			closestDist = dist;
		}
	}
	return closestCell;
}

void CellSpace::MarkCellAsVisited(int index)
{
	if (index < 0 || index > static_cast<int>(m_Cells.size()) - 1)
	{
		std::cout << "ERROR: Index out of range at \"MarkCellAsVisited()\"" << std::endl;
		return;
	}

	m_Cells[index].hasVisited = true;
}

void CellSpace::MarkCellAsVisited(const Elite::Vector2& agentPos)
{
	for (auto& cell : m_Cells)
	{
		if (cell.center.DistanceSquared(agentPos) < m_VisitedDistance)
		{
			cell.hasVisited = true;
			break;
		}
	}

	for (auto& cell : m_ExplorationPath)
	{
		if (cell.center.DistanceSquared(agentPos) < m_VisitedDistance)
		{
			cell.hasVisited = true;
			return;
		}
	}
}
