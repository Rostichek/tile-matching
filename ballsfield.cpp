#include "ballsfield.h"
#include <random>
#include <set>
#include <utility>
#include <cmath>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <exception>

BallsField::BallsField(QObject *parent) : QAbstractListModel(parent)
{
  readPropertiesByJson();
  createBalls();
}

void BallsField::readPropertiesByJson() {
  QFile properties_file;

  properties_file.setFileName("../TileMatching/properties.json");
  if(!properties_file.open(QIODevice::ReadOnly))
    throw std::runtime_error("properties.json doesn't exist");

  QByteArray data = properties_file.readAll();
  QJsonDocument properties_document;
  properties_document = QJsonDocument::fromJson(data);
  m_columns = properties_document.object()["columns"].toInt();
  m_rows = properties_document.object()["rows"].toInt() * 2; // lsa row upper than screen
  balls.resize(m_columns);

  auto json_palette = properties_document.object()["palette"].toArray();
  for(const auto& color : json_palette)
    palette.emplace_back(color.toString().toStdString());
}

QHash<int,QByteArray> BallsField::roleNames() const {
  return rolesDictionary;
}

BallsField::Color& BallsField::get(size_t index) {
  return balls[index / m_rows][index % m_rows];
}

const BallsField::Color& BallsField::get(size_t index) const {
  return balls[index / m_rows][index % m_rows];
}

int BallsField::getScore() const {
  return m_score;
}

void BallsField::computeScore() {
  m_score += exp(indexes_to_remove.size()*0.35);
  emit scoreChanged();
  indexes_to_remove.clear();
}

void BallsField::emitDecoration(size_t index) {
  emit dataChanged(this->index(index), this->index(index), { Qt::DecorationRole });
}

int BallsField::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;

  return m_columns * m_rows;
}

QVariant BallsField::data(const QModelIndex &index, int role) const
{
  switch(role) {
    case Qt::DisplayRole:
      return palette.at(get(index.row())).c_str();

    case Qt::DecorationRole:
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

BallsField::Color BallsField::getRandomColor() const {
  std::random_device device;
  std::mt19937 gen(device());
  std::uniform_int_distribution<int> uniform_dist(0, palette.size() - 1);
  return uniform_dist(gen);
}

void BallsField::createBalls() {
  beginResetModel();
  for(auto& row : balls) {
      row.resize(m_rows);
      for(auto& ball : row)
        ball = getRandomColor();
    }


  for(size_t i = 0; i < m_rows * m_columns; i++) {
      indexes_to_remove.clear();
      findBallsToRemove(i);
      if(indexes_to_remove.size() >= 3) {
          get(*indexes_to_remove.begin()) = getRandomColor();
          get(*indexes_to_remove.cbegin()) = getRandomColor();
          i--;
        }
    }

  indexes_to_remove.clear();
  endResetModel();

  m_score = 0;
  m_steps = 0;
  emit scoreChanged();
  emit stepsChanged();

  return;
}

void BallsField::selectBall(int index) {
  if(-1 == selected_idx) { // no one ball is selected
      selected_idx = index;
      emitDecoration(index);
    }
  else if(index == selected_idx){ // this ball is selected
      selected_idx = -1;
      emitDecoration(index);
    }
  else { // to select new ball
      if(move(index)) {
          emit dataChanged(this->index(0), this->index(m_rows * m_columns - 1), { Qt::DisplayRole });
        }
      else {
          size_t tmp = selected_idx;
          selected_idx = index;
          emitDecoration(index);
          emitDecoration(tmp);
        }
    }
}

void BallsField::findBallsToRemove(size_t index, size_t iter) const {
  static size_t basic;
  if(!iter) basic = index;
  indexes_to_remove.insert(index);

  auto moveOn = [&](size_t second) {
    if(get(second) == get(basic) && !indexes_to_remove.count(second))
      findBallsToRemove(second, iter + 1);
  };

  if(index && (index % m_columns)) // right
    moveOn(index - 1);
  if(index + 1 != balls.size() && (index + 1) % m_columns) // left
    moveOn(index + 1);
  if(index >= m_columns)  // down
    moveOn(index - m_columns);
  if(index < m_columns * (m_rows / 2 - 1))  // up
    moveOn(index + m_columns);

  return;
}

void BallsField::swapUp(size_t index) {
  size_t second_ball = index + m_columns;
  moveRows(index, second_ball + 1, second_ball - 1, index);
  std::swap(get(index), get(second_ball));
}

void BallsField::removeBallsGroup() {
  if(indexes_to_remove.empty()) return;

  std::set<size_t> sorted_indexes(indexes_to_remove.begin(), indexes_to_remove.end());

  for(auto it = sorted_indexes.rbegin(); it != sorted_indexes.rend(); it++) {
      size_t index = *it;
      get(index) = getRandomColor();
      size_t steps = m_rows - (index / m_columns) - 1;
      for(size_t i = 0; i < steps; i++) {
          swapUp(index + m_columns * i);
        }
    }

  computeScore();
}

bool BallsField::findAllBallsGroup() {
  bool group_found = false;

  for(size_t i = 0; i < (m_rows / 2) * m_columns /* without unvisiable row */; i++) {
      indexes_to_remove.clear();
      findBallsToRemove(i);
      if(indexes_to_remove.size() >= 3) {
          to_hide.insert(indexes_to_remove.begin(), indexes_to_remove.end());
          emit dataChanged(this->index(0), this->index(m_rows * m_columns - 1), { HiddenRole });
          removeBallsGroup();
          group_found = true;
        }
    }

  indexes_to_remove.clear();

  return group_found;
}

bool BallsField::trySwap(size_t first,size_t second) {
  std::swap(get(first), get(second));

  for(size_t ball_index : {first, second}) {
      indexes_to_remove.clear();
      findBallsToRemove(ball_index);
      if(indexes_to_remove.size() >= 3) {
          return true;
        }
    }

  std::swap(get(first), get(second));
  return false;
}

void BallsField::moveRows(int first_lhs, int first_rhs, int second_lhs, int scond_rhs) {
  beginMoveRows(QModelIndex(), first_lhs, first_lhs, QModelIndex(), first_rhs);
  endMoveRows();
  if(-1 == second_lhs)
    return;
  beginMoveRows(QModelIndex(), second_lhs, second_lhs, QModelIndex(), scond_rhs);
  endMoveRows();
};

bool BallsField::move(const int index) {
  int diff = selected_idx - index;

  indexes_to_remove.clear();

  auto trySwapAndMove = [&](int first_lhs, int first_rhs, int second_lhs = -1, int second_rhs = - 1) {
      if(!trySwap(index, selected_idx))
        return false;
      moveRows(first_lhs, first_rhs, second_lhs, second_rhs);
      return true;
    };

  if ((1 == diff) && ((index + 1) % m_columns)) {
      if(!trySwapAndMove(index, selected_idx + 1))
        return false;
    }
  else if (-1 == diff && (index % m_columns)) {
      if(!trySwapAndMove(index, selected_idx))
        return false;
    }
  else if (m_columns == diff) {
      if(!trySwapAndMove(index, selected_idx + 1, selected_idx - 1, index))
        return false;
    }
  else if(-m_columns == diff) {
      if(!trySwapAndMove(index, selected_idx, selected_idx + 1, index + 1))
        return false;
    }

  if(indexes_to_remove.empty())
    return false;
  else {
      selected_idx = -1;
      emitDecoration(index);
      m_steps++;
      emit stepsChanged();
      return true;
    }
}

bool BallsField::areThereMoreMoves() {
  auto areThisMoveSucesfull = [&](const std::pair<size_t, size_t>& adjacent_balls) {
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
          if(areThisMoveSucesfull({i * m_columns + j, i * m_columns + j + 1}))
            return true;
        }
    }
  // check all vertical swaps
  for(size_t i = 0; i < m_columns; i++) {
      for(size_t j = 0; j < m_rows / 2 - 1; j++) {
          // { [i][j], [i][j + 1] }
          if(areThisMoveSucesfull({j * m_columns + i, (j + 1) * m_columns + i}))
            return true;
        }
    }

  emit endGame();
  qDebug() << "There are no one possible swaps";
  return false;
}
