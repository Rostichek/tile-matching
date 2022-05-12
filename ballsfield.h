#ifndef BALLSFIELD_H
#define BALLSFIELD_H

#include <QAbstractListModel>
#include <vector>
#include <string>
#include <unordered_set>

class BallsField : public QAbstractListModel
{
  Q_OBJECT

public:
  using Color = std::string;

  explicit BallsField(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  Q_INVOKABLE void selectBall(int index);
  Q_INVOKABLE int getScore() const;

  Q_PROPERTY(int columns MEMBER m_columns CONSTANT);
  Q_PROPERTY(int rows MEMBER m_rows NOTIFY rowsChanged);
  Q_PROPERTY(int score MEMBER m_score NOTIFY scoreChanged);
  Q_PROPERTY(int steps MEMBER m_steps NOTIFY stepsChanged);
Q_SIGNALS:
  void rowsChanged();
  void scoreChanged();
  void stepsChanged();

private:
  const Color& get(size_t index) const;
  Color& get(size_t index);
  Color getRandomColor() const;
  void emitDecoration(size_t index);
  void computeScore();

  void CreateBalls() const;
  bool move(const int);

  void findBallsToRemove(size_t index, size_t iter = 0);
  void removeBallsGroup();
  void swapUp(size_t);

private:
  std::vector<Color> palette;
  size_t m_columns;
  size_t m_rows;
  size_t m_score = 0;
  size_t m_steps = 0;
  size_t points_counter = 0;
  int selected_idx = -1;

  mutable std::vector<std::vector<Color>> balls;
  std::unordered_set<size_t> indexes_to_remove;
};
#endif // BALLSFIELD_H
