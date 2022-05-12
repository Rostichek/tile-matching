#include "ballsfield.h"
#include <random>
#include <set>
#include <cmath>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <exception>
#include <thread>
#include <chrono>

BallsField::BallsField(QObject *parent) : QAbstractListModel(parent)
{
  QFile properties_file;

  properties_file.setFileName("/home/rokoval/TileMatching/properties.json");
  if(!properties_file.open(QIODevice::ReadOnly))
    throw std::runtime_error("propertoe.json doesn't exist");

  QByteArray data = properties_file.readAll();
  QJsonDocument properties_document;
  properties_document = QJsonDocument::fromJson(data);
  m_columns = properties_document.object()["columns"].toInt();
  balls.resize(m_columns);

  auto json_palette = properties_document.object()["palette"].toArray();
  for(const auto& color : json_palette)
    palette.emplace_back(color.toString().toStdString());

  return;
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

void BallsField::waitForAnim() const {
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void BallsField::emitDecoration(size_t index) {
  emit dataChanged(this->index(index), this->index(index), { Qt::DecorationRole });
}

int BallsField::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;

  return model_setted ? m_columns * m_rows : 0;
}

QVariant BallsField::data(const QModelIndex &index, int role) const
{
  switch(role) {
    case Qt::DisplayRole:
      return palette.at(get(index.row())).c_str();

    case Qt::DecorationRole:
      return index.row() == selected_idx;

    case Qt::StatusTipRole:
      return static_cast<int>(m_score);
    default:
      return {};
    }
}

BallsField::Color BallsField::getRandomColor() const {
  std::random_device device;
  std::mt19937 gen(device());
  std::uniform_int_distribution<int> uniform_dist(0, palette.size()-1);
  return uniform_dist(gen);
}

void BallsField::createBalls() {
  beginResetModel();
  for(auto& row : balls) {
      row.resize(m_rows);
      for(auto& ball : row)
        ball = getRandomColor();
    }

  bool to_remove = false;
  do {
      for(size_t i = 0; i < m_rows * m_columns; i++) {
          indexes_to_remove.clear();
          findBallsToRemove(i);
          if(indexes_to_remove.size() >= 3) {
              get(*indexes_to_remove.begin()) = getRandomColor();
              get(*indexes_to_remove.cbegin()) = getRandomColor();
            }
        }
    } while (to_remove);

  indexes_to_remove.clear();
  model_setted = true;
  endResetModel();

  m_score = 0;
  m_steps = 0;
  emit scoreChanged();
  emit stepsChanged();

  return;
}

void BallsField::selectBall(int index) {
  if(-1 == selected_idx) {
      selected_idx = index;
      emitDecoration(index);
    }
  else if(index == selected_idx){
      selected_idx = -1;
      emitDecoration(index);
    }
  else {
      if(move(index)) {
          emit dataChanged(this->index(0), this->index(m_rows * m_columns - 1), { Qt::DisplayRole });
          areThereMoreMoves();
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
  static size_t basic_ball;
  if(!iter) basic_ball = index;

  indexes_to_remove.insert(index);

  size_t second_ball = 0;

  if(index && (index % m_columns)) { // right
      second_ball = index - 1;
      if(get(second_ball) == get(basic_ball) && !indexes_to_remove.count(second_ball))
        findBallsToRemove(second_ball, iter + 1);
    }
  if(index + 1 != balls.size() && (index + 1) % m_columns) { // left
      second_ball = index + 1;
      if(get(second_ball) == get(basic_ball) && !indexes_to_remove.count(second_ball))
        findBallsToRemove(second_ball, iter + 1);
    }
  if(index > m_columns) { // down
      second_ball = index - m_columns;
      if(get(second_ball) == get(basic_ball) && !indexes_to_remove.count(second_ball))
        findBallsToRemove(second_ball, iter + 1);
    }
  if(index < m_columns * (m_rows - 1)) { // up
      second_ball = index + m_columns;
      if(get(second_ball) == get(basic_ball) && !indexes_to_remove.count(second_ball))
        findBallsToRemove(second_ball, iter + 1);
    }

  return;
}

void BallsField::swapUp(size_t index) {
  size_t second_ball = index + m_columns;
  beginMoveRows(QModelIndex(), index, index, QModelIndex(), second_ball + 1);
  endMoveRows();
  beginMoveRows(QModelIndex(), second_ball- 1, second_ball - 1, QModelIndex(), index);
  endMoveRows();
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

void BallsField::findAllBallsGroup() {
  bool to_remove = false;
  do {
      for(size_t i = 0; i < m_rows * m_columns; i++) {
          indexes_to_remove.clear();
          findBallsToRemove(i);
          if(indexes_to_remove.size() >= 3) {
              removeBallsGroup();
              i = 0;
            }
        }
    } while (to_remove);
  indexes_to_remove.clear();
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

bool BallsField::move(const int index) {
  int diff = selected_idx - index;

  if ((1 == diff) && ((index + 1) % m_columns)) {
      if(!trySwap(index, selected_idx))
        return false;
      beginMoveRows(QModelIndex(), index, index, QModelIndex(), selected_idx+1);
      endMoveRows();
    }
  else if (-1 == diff && (index % m_columns)) {
      if(!trySwap(index, selected_idx))
        return false;
      beginMoveRows(QModelIndex(), index, index, QModelIndex(), selected_idx);
      endMoveRows();
    }
  else if(-m_columns == diff) {
      if(!trySwap(index, selected_idx))
        return false;
      beginMoveRows(QModelIndex(), index, index, QModelIndex(), index - m_columns);
      endMoveRows();
      beginMoveRows(QModelIndex(), selected_idx + 1, selected_idx + 1, QModelIndex(), index + 1);
      endMoveRows();
    }
  else if (m_columns == diff) {
      if(!trySwap(index, selected_idx))
        return false;
      beginMoveRows(QModelIndex(), index, index, QModelIndex(), selected_idx + 1);
      endMoveRows();
      beginMoveRows(QModelIndex(), selected_idx - 1, selected_idx - 1, QModelIndex(), index);
      endMoveRows();
    }

  if(indexes_to_remove.empty())
    return false;
  else {
      selected_idx = -1;
      emitDecoration(index);
      m_steps++;
      emit stepsChanged();
      findAllBallsGroup();
      return true;
    }
}

bool BallsField::areThereMoreMoves() {
  // check  all horizontal swaps
  for(size_t i = 0; i < m_rows; i++) {
      //    If there is an odd number of elements,
      //    then at the last transition we need to change the step to 1
      for(size_t j = 0; j < m_columns - 1; j += (m_columns - j == 3) ? 1 : 2) {
          size_t first = i * m_columns + j,
              second = i * m_columns + j + 1;
          if(trySwap(first, second)) {
              std::swap(get(first), get(second));
              return true;
            }
        }
    }
  // check all vertical swaps
  for(size_t i = 0; i < m_columns; i++) {
      //    If there is an odd number of elements,
      //    then at the last transition we need to change the step to 1
      for(size_t j = 0; j < m_rows - 1; (m_rows - j == 3) ? 1 : 2) {
          size_t first = j * m_columns + i,
              second = (j + 1) * m_columns + i;
          if(trySwap(first, second)) {
              std::swap(get(first), get(second));
              return true;
            }
        }
    }

  return false;
}
