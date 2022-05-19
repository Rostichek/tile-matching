#include "ballsfield.h"
#include <random>
#include <set>
#include <sstream>
#include <cmath>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

BallsField::BallsField(QObject *parent) : QAbstractListModel(parent)
{
    readPropertiesByJson();
    createBalls();
}

/*!
 * \brief Reads the color palette and number of columns
 * from the properties.json file
 */
void BallsField::readPropertiesByJson() {
    QFile properties_file;

    properties_file.setFileName("../TileMatching/properties.json");
    if(!properties_file.open(QIODevice::ReadOnly)) {
	throw std::runtime_error("properties.json doesn't exist");
    }
    QByteArray data = properties_file.readAll();
    QJsonDocument properties_document;
    properties_document = QJsonDocument::fromJson(data);
    m_columns = properties_document.object()["columns"].toInt();
    m_rows = properties_document.object()["rows"].toInt() * 2; // lsa row upper than screen
    balls.resize(m_columns);

    QJsonArray json_palette = properties_document.object()["palette"].toArray();
    for(const auto& color : json_palette) {
	palette.emplace_back(color.toString().toStdString());
    }
}

QHash<int,QByteArray> BallsField::roleNames() const {
    return rolesDictionary;
}

/*!
 * \brief Returns reference to the color of the ball by index
 */
BallsField::Color& BallsField::get(size_t index) {
    try {
	return balls[index / m_rows][index % m_rows];
    }  catch (...) {
	std::stringstream ss;
	ss << "Can't returns an elements with index " << index << ". Model size is " << rowCount();
	throw std::out_of_range(ss.str());
    }
}

/*!
 * \brief Returns constant reference to the color of the ball by index
 */
const BallsField::Color& BallsField::get(size_t index) const {
    try {
	return balls.at(index / m_rows).at(index % m_rows);
    }  catch (...) {
	std::stringstream ss;
	ss << "Can't returns an elements with index " << index << ". Model size is " << rowCount();
	throw std::out_of_range(ss.str());
    }}

/*!
 * \brief Returns points
 */
int BallsField::getScore() const {
    return m_score;
}

/*!
 * \brief Adds points to the result after removing the balls
 */
void BallsField::computeScore() {
    m_score += exp(total_removes * 0.35f);
    total_removes = 0;
    emit scoreChanged();
    indexes_to_remove.clear();
}

void BallsField::emitSelected(size_t index) {
    emit dataChanged(this->index(index), this->index(index), { SelectedRole });
}

int BallsField::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
	return 0;
    }

    return m_columns * m_rows;
}

QVariant BallsField::data(const QModelIndex &index, int role) const
{
    switch(role) {
	case Qt::DisplayRole:
	    return palette.at(get(index.row())).c_str();

	case SelectedRole:
	    return index.row() == selected_idx;

	case HiddenRole: {
	    bool return_value = static_cast<bool>(to_hide.count(index.row()));
	    if(return_value) to_hide.erase(index.row());
	    return return_value;
	}

	default:
	    return {};
    }
}

/*!
 * \brief Returns a random color from the palette using the std::mt19937 generator
 */
BallsField::Color BallsField::getRandomColor() const {
    std::random_device device;
    std::mt19937 gen(device());
    std::uniform_int_distribution<int> uniform_dist(0, palette.size() - 1);
    return uniform_dist(gen);
}

/*!
 * \brief Sets the size of the model and fills it with random color balls
 * from the palette. Resets all variables.
 */
void BallsField::createBalls() {
    beginResetModel();
    for(auto& row : balls) {
	row.resize(m_rows);
	for(auto& ball : row) {
	    ball = getRandomColor();
	}
    }

    for(size_t i = 0; i < m_rows * m_columns; i++) {
	indexes_to_remove.clear();
	findBallsToRemove(i, i);
	if(indexes_to_remove.size() >= 3) {
	    get(*indexes_to_remove.begin()) = getRandomColor();
	    get(*indexes_to_remove.cbegin()) = getRandomColor();
	    i--;
	}
    }

    indexes_to_remove.clear();
    endResetModel();

    if(!areThereMoreMoves()) {
	createBalls();
    }

    m_score = 0;
    m_steps = 0;
    emit scoreChanged();
    emit stepsChanged();
}

/*!
 * \brief Mark element by index as selected.
 * Tries to swap previously selected element and index
 */
void BallsField::selectBall(int index) {
    if(-1 == selected_idx) { // no one ball is selected
	selected_idx = index;
	emitSelected(index);
    }
    else if(index == selected_idx) { // this ball is selected
	selected_idx = -1;
	emitSelected(index);
    }
    else { // to select new ball
	if(move(index)) {
	    emit dataChanged(this->index(0), this->index(m_rows * m_columns - 1), { Qt::DisplayRole });
	}
	else {
	    size_t tmp = selected_idx;
	    selected_idx = index;
	    emitSelected(index);
	    emitSelected(tmp);
	}
    }
}

/*!
 * \brief Recursively finds all balls of the same color that touch an element by index.
 * Saves the result in the parameter.
 */
void BallsField::findBallsToRemove(size_t index, size_t basic) const {
    indexes_to_remove.insert(index);

    std::function<void(size_t)> moveOn =
	    [&](size_t second)
    {
	if(get(second) == get(basic) && !indexes_to_remove.count(second)) {
	    findBallsToRemove(second, basic);
	}
    };

    if(index && (index % m_columns)) { // right
	moveOn(index - 1);
    }
    if(index + 1 != balls.size() && (index + 1) % m_columns) { // left
	moveOn(index + 1);
    }
    if(index >= m_columns) {  // down
	moveOn(index - m_columns);
    }
    if(index < m_columns * (m_rows / 2 - 1)) { // up
	moveOn(index + m_columns);
    }

    return;
}

/*!
 * \brief Swaps the index element to the top.
 */
void BallsField::swapUp(size_t index) {
    size_t second_ball = index + m_columns;
    moveRows(index, second_ball + 1, second_ball - 1, index);
    std::swap(get(index), get(second_ball));
}

/*!
 * \brief Swaps to the top all elements found by BallsField::findBallsToRemove.
 */
void BallsField::removeBallsGroup() {
    if(indexes_to_remove.empty()) { return; }

    std::set<size_t> sorted_indexes(indexes_to_remove.begin(), indexes_to_remove.end());

    for(auto it = sorted_indexes.rbegin(); it != sorted_indexes.rend(); it++) {
	size_t index = *it;
	get(index) = getRandomColor();
	emit dataChanged(this->index(index), this->index(index), { Qt::DisplayRole });
	size_t steps = m_rows - (index / m_columns) - 1;
	for(size_t i = 0; i < steps; i++) {
	    swapUp(index + m_columns * i);
	}
    }

    total_removes += indexes_to_remove.size();
}

/*!
 * \brief Finds all balls groups with match 3 or more same adjacent elements.
 */
bool BallsField::findAllBallsGroup() {
    for(size_t i = 0; i < (m_rows / 2) * m_columns; i++) {
	indexes_to_remove.clear();
	findBallsToRemove(i, i);
	if(indexes_to_remove.size() >= 3) {
	    to_hide.insert(indexes_to_remove.begin(), indexes_to_remove.end());
	    emit dataChanged(this->index(0), this->index(m_rows * m_columns - 1), { HiddenRole });
	    removeBallsGroup();
	    indexes_to_remove.clear();
	    return true;
	}
    }
    indexes_to_remove.clear();
    return false;
}


/*!
 * \brief Returns whether the swap of the first and second elements can be successful,
 *  and swaps if possible.
 */
bool BallsField::trySwap(size_t first,size_t second) {
    std::swap(get(first), get(second));

    for(size_t ball_index : {first, second}) {
	indexes_to_remove.clear();
	findBallsToRemove(ball_index, ball_index);
	if(indexes_to_remove.size() >= 3) {
	    return true;
	}
    }

    std::swap(get(first), get(second));
    return false;
}

/*!
 * \brief Moves entities in the model by indexes from arguments
 */
void BallsField::moveRows(int first_lhs, int first_rhs, int second_lhs, int scond_rhs) {
    beginMoveRows(QModelIndex(), first_lhs, first_lhs, QModelIndex(), first_rhs);
    endMoveRows();
    if(-1 == second_lhs) {
	return;
    }
    beginMoveRows(QModelIndex(), second_lhs, second_lhs, QModelIndex(), scond_rhs);
    endMoveRows();
};

/*!
 * \brief Swaps index and selected element if
 *  they are adjacent and moving them create a match 3 or more balls
 */
bool BallsField::move(const int index) {
    int diff = selected_idx - index;

    indexes_to_remove.clear();

    auto trySwapAndMove =
	    [&](int first_lhs, int first_rhs, int second_lhs = -1, int second_rhs = - 1)
    {
	if(!trySwap(index, selected_idx))
	    return false;
	moveRows(first_lhs, first_rhs, second_lhs, second_rhs);
	return true;
    };

    if ((1 == diff) && ((index + 1) % m_columns)) {
	if(!trySwapAndMove(index, selected_idx + 1)) {
	    return false;
	}
    }
    else if (-1 == diff && (index % m_columns)) {
	if(!trySwapAndMove(index, selected_idx)) {
	    return false;
	}
    }
    else if (m_columns == diff) {
	if(!trySwapAndMove(index, selected_idx + 1, selected_idx - 1, index)) {
	    return false;
	}
    }
    else if(-m_columns == diff) {
	if(!trySwapAndMove(index, selected_idx, selected_idx + 1, index + 1)) {
	    return false;
	}
    }

    if(indexes_to_remove.empty()) {
	return false;
    }
    else {
	selected_idx = -1;
	emitSelected(index);
	m_steps++;
	emit stepsChanged();
	return true;
    }
}

/*!
 * \brief Returns whether the player can make another successful move
 */
bool BallsField::areThereMoreMoves() {
    std::function<bool(const std::pair<size_t, size_t>&)> areThisMoveSucesfull =
	    [&](const std::pair<size_t, size_t>& adjacent_balls)
    {
	if(trySwap(adjacent_balls.first, adjacent_balls.second)) {
	    std::swap(get(adjacent_balls.first), get(adjacent_balls.second));
	    qDebug() << "Possible swap: " << adjacent_balls.first << " <-> " << adjacent_balls.second;
	    return true;
	}
	return false;
    };

    // check  all horizontal swaps
    for(size_t i = 0; i < m_rows / 2; i++) {
	for(size_t j = 0; j < m_columns - 1; j++) {
	    // { [j][i], [j + 1][i] }
	    if(areThisMoveSucesfull({i * m_columns + j, i * m_columns + j + 1})) {
		return true;
	    }
	}
    }
    // check all vertical swaps
    for(size_t i = 0; i < m_columns; i++) {
	for(size_t j = 0; j < m_rows / 2 - 1; j++) {
	    // { [i][j], [i][j + 1] }
	    if(areThisMoveSucesfull({j * m_columns + i, (j + 1) * m_columns + i})) {
		return true;
	    }
	}
    }

    emit endGame();
    qDebug() << "There are no one possible swaps";
    return false;
}
