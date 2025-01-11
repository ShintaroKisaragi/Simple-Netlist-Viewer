#pragma once
#include <QDialog>
#include <QLineEdit>

namespace Netlist
{
    class SaveCellDialog : public QDialog
    {
        Q_OBJECT;

    public:
        SaveCellDialog(QWidget *parent = NULL);
        bool run(QString &nameCell,QString &namePath);
        const QString getCellName() const;
        const QString getPathName() const;
        void setCellName(const QString &);
        void setPathName(const QString &);

    protected:
        QLineEdit *lineEdit_; // ligne de texte que l'utilisateur Ã©crit
        QLineEdit *linePath_;
    };
}