#include "sortfilterproxymodel.h"

SortFilterProxyModel::SortFilterProxyModel(QObject *AParent) : QSortFilterProxyModel(AParent)
{
  FOptions = 0;
  setSortRole(Qt::DisplayRole);
  setSortLocaleAware(true);
  setSortCaseSensitivity(Qt::CaseInsensitive);
}

SortFilterProxyModel::~SortFilterProxyModel()
{

}

void SortFilterProxyModel::setSourceModel(QAbstractItemModel *ASourceModel)
{
  QSortFilterProxyModel::setSourceModel(ASourceModel);
}

bool SortFilterProxyModel::checkOption(IRostersView::Option AOption) const
{
  return (FOptions & AOption) == AOption;
}

void SortFilterProxyModel::setOption(IRostersView::Option AOption, bool AValue)
{
  AValue ? FOptions |= AOption : FOptions &= ~AOption;
  if (AOption == IRostersView::ShowOfflineContacts || AOption == IRostersView::ShowOnlineFirst)
    invalidate();
}

bool SortFilterProxyModel::lessThan(const QModelIndex &ALeft, const QModelIndex &ARight) const
{
  int leftType = ALeft.data(RDR_Type).toInt();
  int rightType = ARight.data(RDR_Type).toInt();
  if (leftType == rightType)
  {
    int leftShow = ALeft.data(RDR_Show).toInt();
    int rightShow = ARight.data(RDR_Show).toInt();
    bool showOnlineFirst = checkOption(IRostersView::ShowOnlineFirst);
    if (showOnlineFirst && leftType!=RIT_StreamRoot && leftShow!=rightShow)
    {
      const static int showOrders[] = {6,1,2,3,4,5,0,7};
      return showOrders[leftShow] < showOrders[rightShow];
    }
    else
      return QSortFilterProxyModel::lessThan(ALeft,ARight);
  }
  else
    return leftType < rightType;
}

bool SortFilterProxyModel::filterAcceptsRow(int AModelRow, const QModelIndex &AModelParent) const
{
  if (checkOption(IRostersView::ShowOfflineContacts))
    return true;

  QModelIndex index = sourceModel()->index(AModelRow,0,AModelParent);

  if (index.isValid())
  {
    int indexType = index.data(RDR_Type).toInt();
    switch(indexType)
    {
    case RIT_Contact:
    case RIT_Agent:
      {
        QList<QVariant> labelFlags = index.data(RDR_LabelFlags).toList();
        foreach(QVariant flag, labelFlags)
          if ((flag.toInt() & IRostersView::LabelVisible) > 0)
            return true;
        int indexShow = index.data(RDR_Show).toInt();
        return indexShow!=IPresence::Offline && indexShow!=IPresence::Error;
      }
    case RIT_Group:
    case RIT_AgentsGroup:
    case RIT_BlankGroup:
    case RIT_NotInRosterGroup:
      {
        for(int childRow = 0; index.child(childRow,0).isValid(); childRow++)
          if (filterAcceptsRow(childRow,index))
            return true;
        return false;
      }
    }
  }
  return true;
}

