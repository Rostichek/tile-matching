#ifndef BALLSFIELD_H
#define BALLSFIELD_H

#include <QAbstractListModel>
#include <vector>
#include <string>

class BallsField : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit BallsField(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  Q_INVOKABLE void selectBall(int index);

  Q_PROPERTY(int columns MEMBER m_columns NOTIFY columnsChanged);
  Q_PROPERTY(int rows MEMBER m_rows NOTIFY rowsChanged);

Q_SIGNALS:
  void columnsChanged();
  void rowsChanged();

private:
  using Color = std::string;

  const std::vector<Color> palette { "red", "green", "blue", "purple", "yellow", "orange"}; // to implement by JSON
  size_t m_columns = 7;
  size_t m_rows;
  int selected_idx = -1;
  mutable std::vector<std::vector<Color>> balls;

private:
  void CreateBalls() const;
  void move(const int);

};
#endif // BALLSFIELD_H
