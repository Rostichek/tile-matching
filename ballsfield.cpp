#include "ballsfield.h"
#include <random>
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
}

void BallsField::CreateBalls() const {
  for(auto& row : balls) {
      row.resize(m_rows);
      for(auto& ball : row) {
          std::random_device r;
          std::default_random_engine e1(r());
          std::uniform_int_distribution<int> uniform_dist(0, palette.size()-1);
          ball = palette.at(uniform_dist(e1));
        }
    }
}

void BallsField::selectBall(int index) {
  if(-1 == selected_idx) {
      selected_idx = index;
    }
  else if(index == selected_idx){
      selected_idx = -1;
    }
  else {
      move(index);
    }
  dataChanged(this->index(index), this->index(index), { Qt::DecorationRole });
}

void BallsField::move(const int index) {
    auto swapRows = [&]() {
        std::swap(balls[index], balls[selected_idx]);
        endMoveRows();
        selected_idx = -1;
    };

    int diff = selected_idx - index;

    if ((1 == diff) && ((index + 1) % m_columns)) {
        beginMoveRows(QModelIndex(), index, index, QModelIndex(), selected_idx+1);
        swapRows();
    }
    else if (-1 == diff && (index % m_columns)) {
        beginMoveRows(QModelIndex(), index, index, QModelIndex(), selected_idx);
        swapRows();
    }
    else if (-m_columns == diff) {
        beginMoveRows(QModelIndex(), index, index, QModelIndex(), index - m_columns);
        endMoveRows();
        beginMoveRows(QModelIndex(), selected_idx + 1, selected_idx + 1, QModelIndex(), index + 1);
        swapRows();
    }
    else if (m_columns == diff) {
        beginMoveRows(QModelIndex(), index, index, QModelIndex(), selected_idx + 1);
        endMoveRows();
        beginMoveRows(QModelIndex(), selected_idx - 1, selected_idx - 1, QModelIndex(), index);
        swapRows();
    }
}
