#ifndef BALLSFIELD_H
#define BALLSFIELD_H

#include <vector>
#include <string>
#include <unordered_set>
#include <QAbstractListModel>

class BallsField : public QAbstractListModel
{
  Q_OBJECT
  Q_ENUMS(Roles)
public:
    enum Roles {
        HiddenRole = Qt::UserRole + 1,
    };

public:
  using Color = size_t;

  explicit BallsField(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  QHash<int,QByteArray> roleNames() const override;

  Q_INVOKABLE void selectBall(int index);
  Q_INVOKABLE int getScore() const;
  Q_INVOKABLE void createBalls();

  // use findBallsToRemove on the all field
  Q_INVOKABLE bool findAllBallsGroup();

  // check is at least one move can be done
  Q_INVOKABLE bool areThereMoreMoves();

  // compute how many points have to be added
  // to the score after move
  Q_INVOKABLE void computeScore();

  Q_PROPERTY(int columns MEMBER m_columns CONSTANT); // set by properties
  Q_PROPERTY(int rows MEMBER m_rows NOTIFY rowsChanged); // computing by screen height and columns number
  Q_PROPERTY(int score MEMBER m_score NOTIFY scoreChanged);
  Q_PROPERTY(int steps MEMBER m_steps NOTIFY stepsChanged);

Q_SIGNALS:
  void rowsChanged();
  void scoreChanged();
  void stepsChanged();
  void endGame();

private:
  void readPropertiesByJson();

  // get code of ball's color by one dimensional index
  const size_t& get(size_t index) const;
  size_t& get(size_t index);

  // get random color code from the palette
  Color getRandomColor() const;

  // emit to the view that the ball by the index
  // was selected or deselected
  void emitDecoration(size_t index);

  // try to move selected ball to the index
  bool move(const int);

  // recursively search for all identical balls that touch the index
  void findBallsToRemove(size_t index, size_t basic) const;

  // move away to the top all balls from indexes_to_remove
  void removeBallsGroup();

  // move ball by the index away to the top
  void swapUp(size_t);

  // checking if the swap between indexes will give
  // a group of balls with more than 2 identical balls
  bool trySwap(size_t,size_t);

  // emitting about move rows
  void moveRows(int first_lhs, int first_rhs, int second_lhs = -1, int scond_rhs = -1);

private:
  std::vector<std::string> palette; // set by properties

  size_t m_columns = 0;
  size_t m_rows = 0;
  size_t m_score = 0;
  size_t m_steps = 0;
  size_t points_counter = 0;
  size_t total_removes = 0;
  int selected_idx = -1;

  std::vector<std::vector<Color>> balls;
  mutable std::unordered_set<size_t> indexes_to_remove;
  mutable std::unordered_set<size_t> to_hide;

  const QHash<int,QByteArray> rolesDictionary = {
      { Qt::DisplayRole, "display" },
      { Qt::DecorationRole, "decoration" },
      { HiddenRole, "hidden" },
  };
};
#endif // BALLSFIELD_H
