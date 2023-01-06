#pragma once

enum class Direction
{
	right,
	up,
	left,
	down
};

// --- Rect ---
// ------------
struct Rect
{
	Elite::Vector2 bottomLeft;
	float width;
	float height;
};

// --- Cell ---
// ------------
struct Cell
{
	Cell() = default;
	Cell(float left, float bottom, float width, float height);
	std::vector<Elite::Vector2> GetRectPoints() const;
	bool operator==(const Cell& other) const { return center == other.center; }

	bool hasVisited;
	Rect boundingBox;
	Elite::Vector2 center;
};

// --- Partitioned Space ---
// -------------------------
class CellSpace
{
public:
	CellSpace() = default;
	CellSpace(float width, float height, int rows, int cols);
	CellSpace& operator=(const CellSpace& other);

	Cell GetClosestCellOnPath(const Elite::Vector2& agentPos) const;
	std::vector<Cell> GetNeighbouringCells(int index) const;
	std::vector<Cell> GetCells() const;
	std::vector<Cell> GetPath() const;
	int PositionToIndex(const Elite::Vector2 pos) const;
	void MarkCellAsVisited(int index);
	void MarkCellAsVisited(const Elite::Vector2& agentPos);

private:
	std::vector<Cell> m_Cells;
	std::vector<Cell> m_ExplorationPath;

	float m_SpaceWidth;
	float m_SpaceHeight;

	int m_NrOfRows;
	int m_NrOfCols;

	const float m_VisitedDistance{ 100.f };

	float m_CellWidth;
	float m_CellHeight;
};