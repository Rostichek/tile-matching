#include "ballsfield.h"
#include <random>
#include <set>
#include <QDebug>

BallsField::BallsField(QObject *parent) : QAbstractListModel(parent), balls(m_columns)
{}

int BallsField::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;

  return m_columns * m_rows;
}

QVariant BallsField::data(const QModelIndex &index, int role) const
{
  if(balls.front().empty()) CreateBalls();

  switch(role) {
    case Qt::DisplayRole: {
        size_t i = index.row() / m_rows,
            j = index.row() % m_rows;
        std::string color = balls.at(i).at(j);
        return color.c_str();
      }
    case Qt::DecorationRole: {
        return index.row() == selected_idx;
      }
    }

  return {};
}

BallsField::Color BallsField::getRandomColor() const {
  std::random_device r;
  std::default_random_engine e1(r());
  std::uniform_int_distribution<int> uniform_dist(0, palette.size()-1);
  return palette.at(uniform_dist(e1));
}

void BallsField::CreateBalls() const {
  for(auto& row : balls) {
      row.resize(m_rows);
      for(auto& ball : row) {
          ball = getRandomColor();
        }
    }
}

void BallsField::selectBall(int index) {
  if(-1 == selected_idx) {
      selected_idx = index;
      emit dataChanged(this->index(index), this->index(index), { Qt::DecorationRole });
    }
  else if(index == selected_idx){
      selected_idx = -1;
      emit dataChanged(this->index(index), this->index(index), { Qt::DecorationRole });
    }
  else {
      if(move(index))
        emit dataChanged(this->index(0), this->index(m_rows * m_columns - 1), { Qt::DisplayRole });
      else {
          size_t tmp = selected_idx;
          selected_idx = index;
          emit dataChanged(this->index(index), this->index(index), { Qt::DecorationRole });
          emit dataChanged(this->index(tmp), this->index(tmp), { Qt::DecorationRole });
        }
    }
}

void BallsField::findBallsToRemove(size_t index, size_t iter) {
  static size_t basic_ball;
  if(!iter) basic_ball = index;

  indexes_to_remove.insert(index);

  if(index && (index % m_columns)) { // right
      if(get(index - 1) == get(basic_ball) && !indexes_to_remove.count(index - 1))
        findBallsToRemove(index - 1, iter + 1);
    }
  if(index + 1 != balls.size() && (index + 1) % m_columns) {// left
      if(get(index + 1) == get(basic_ball) && !indexes_to_remove.count(index + 1))
        findBallsToRemove(index + 1, iter + 1);
    }
  if(index > m_columns) {// down
      if(get(index - m_columns) == get(basic_ball) && !indexes_to_remove.count(index - m_columns))
        findBallsToRemove(index - m_columns, index - m_columns);
    }
  if(index < m_columns * (m_rows - 1)) { // up
      if(get(index + m_columns) == get(basic_ball) && !indexes_to_remove.count(index + m_columns))
        findBallsToRemove(index + m_columns, index + m_columns);
    }

  return;
}

BallsField::Color& BallsField::get(size_t index){
  return balls[index / m_rows][index % m_rows];
}

void BallsField::swapUp(size_t index) {
  beginMoveRows(QModelIndex(), index, index, QModelIndex(), index + m_columns + 1);
  endMoveRows();
  beginMoveRows(QModelIndex(), index + m_columns - 1, index + m_columns - 1, QModelIndex(), index);
  endMoveRows();
  std::swap(get(index), get(index + m_columns));
}

void BallsField::removeBallsGroup() {
  std::set<size_t> sorted_indexes(indexes_to_remove.begin(), indexes_to_remove.end());

  for(auto it = sorted_indexes.rbegin(); it != sorted_indexes.rend(); it++) {
      size_t index = *it;
      get(index) = getRandomColor();
      size_t steps = m_rows - (index / m_columns) - 1;
      for(size_t i = 0; i < steps; i++) {
          swapUp(index + m_columns * i);
        }
    }

  indexes_to_remove.clear();
}

bool BallsField::move(const int index) {
  int diff = selected_idx - index;

  if ((1 == diff) && ((index + 1) % m_columns)) {
      std::swap(get(index), get(selected_idx));
      findBallsToRemove(index);
      if(indexes_to_remove.size() < 3) {
          std::swap(get(index), get(selected_idx));
          return false;
        }
      beginMoveRows(QModelIndex(), index, index, QModelIndex(), selected_idx+1);
      endMoveRows();

      selected_idx = -1;
      emit dataChanged(this->index(index), this->index(index), { Qt::DecorationRole });

      removeBallsGroup();
      return true;
    }
  else if (-1 == diff && (index % m_columns)) {
      std::swap(get(index), get(selected_idx));
      findBallsToRemove(index);
      if(indexes_to_remove.size() < 3) {
          std::swap(get(index), get(selected_idx));
          return false;
        }
      beginMoveRows(QModelIndex(), index, index, QModelIndex(), selected_idx);
      endMoveRows();

      selected_idx = -1;
      emit dataChanged(this->index(index), this->index(index), { Qt::DecorationRole });

      removeBallsGroup();
      return true;
    }
  else if(-m_columns == diff) {
      std::swap(get(index), get(selected_idx));
      findBallsToRemove(index);
      if(indexes_to_remove.size() < 3) {
          std::swap(get(index), get(selected_idx));
          return false;
        }
      beginMoveRows(QModelIndex(), index, index, QModelIndex(), index - m_columns);
      endMoveRows();
      beginMoveRows(QModelIndex(), selected_idx + 1, selected_idx + 1, QModelIndex(), index + 1);
      endMoveRows();

      selected_idx = -1;
      emit dataChanged(this->index(index), this->index(index), { Qt::DecorationRole });

      removeBallsGroup();
      return true;
    }
  else if (m_columns == diff) {
      std::swap(get(index), get(selected_idx));
      findBallsToRemove(index);
      if(indexes_to_remove.size() < 3) {
          std::swap(get(index), get(selected_idx));
          return false;
        }
      beginMoveRows(QModelIndex(), index, index, QModelIndex(), selected_idx + 1);
      endMoveRows();
      beginMoveRows(QModelIndex(), selected_idx - 1, selected_idx - 1, QModelIndex(), index);
      endMoveRows();

      selected_idx = -1;
      emit dataChanged(this->index(index), this->index(index), { Qt::DecorationRole });

      removeBallsGroup();
      return true;
    }
  return false;
}
