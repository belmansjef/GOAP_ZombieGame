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

	return *this;
}

Cell CellSpace::GetNextCellExpandingSquare()
{
	int row{ m_NrOfRows / 2 - 1};
	int col{ m_NrOfCols / 2 - 1};
	for (int size = 1; size < m_NrOfCols; size += 6	)
	{
		// Right
		for (int i = 0; i < size; i++)
		{
			if (!m_Cells[row * m_NrOfCols + col].hasVisited)
			{
				return m_Cells[row * m_NrOfCols + col];
			}
			col++;
		}

		// up
		for (int i = 0; i < size; i++)
		{
			if (!m_Cells[row * m_NrOfCols + col].hasVisited)
			{
				return m_Cells[row * m_NrOfCols + col];
			}
			row++;
		}

		// left
		for (int i = 0; i < size + 3; i++)
		{
			if (!m_Cells[row * m_NrOfCols + col].hasVisited)
			{
				return m_Cells[row * m_NrOfCols + col];
			}
			col--;
		}

		// down
		for (int i = 0; i < size + 3; i++)
		{
			if (!m_Cells[row * m_NrOfCols + col].hasVisited)
			{
				return m_Cells[row * m_NrOfCols + col];
			}
			row--;
		}
		if (row <=0 || row > m_NrOfRows - 1 || col < 0 || col > m_NrOfCols - 1) return m_Cells[m_NrOfRows / 2 * m_NrOfCols + m_NrOfCols / 2];
	}

	return m_Cells[m_NrOfRows / 2 * m_NrOfCols + m_NrOfCols / 2];
}

Cell CellSpace::GetNextCellSectorSearch()
{
	int row{ m_NrOfRows / 2};
	int col{ m_NrOfCols / 2};
	int legLength{ row - 1 };

	// Check starting square
	/*if (!m_Cells[row * m_NrOfCols + col].hasVisited)
	{
		return m_Cells[row * m_NrOfCols + col];
	}
	col++;

	if (!m_Cells[row * m_NrOfCols + col].hasVisited)
	{
		return m_Cells[row * m_NrOfCols + col];
	}
	row++;

	if (!m_Cells[row * m_NrOfCols + col].hasVisited)
	{
		return m_Cells[row * m_NrOfCols + col];
	}
	col--;

	if (!m_Cells[row * m_NrOfCols + col].hasVisited)
	{
		return m_Cells[row * m_NrOfCols + col];
	}
	row--;*/

	// Left
	if (!m_Cells[row * m_NrOfCols + col - legLength].hasVisited)
	{
		return m_Cells[row * m_NrOfCols + col];
	}

	// Top Left
	if (!m_Cells[(row + legLength - 2) * m_NrOfCols + col - legLength + 2].hasVisited)
	{
		return m_Cells[row * m_NrOfCols + col];
	}

	// Top
	if (!m_Cells[(row + legLength) * m_NrOfCols + col].hasVisited)
	{
		return m_Cells[row * m_NrOfCols + col];
	}

	return GetNextCellExpandingSquare();
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
			return;
		}
	}
}
