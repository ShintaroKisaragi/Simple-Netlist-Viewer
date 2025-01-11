#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "SaveCellDialog.h"

namespace Netlist
{
    SaveCellDialog::SaveCellDialog(QWidget *parent)
        : QDialog(parent), lineEdit_(nullptr), linePath_(nullptr)
    {
        setWindowTitle(tr("Save Cell"));
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        // Label et champ pour le nom de la cellule
        QLabel *nameLabel = new QLabel();
        nameLabel->setText(tr("Enter Cell name (without extension):"));
        lineEdit_ = new QLineEdit();
        lineEdit_->setMinimumWidth(400);

        // Label et champ pour le chemin de stockage
        QLabel *pathLabel = new QLabel();
        pathLabel->setText(tr("Enter Storage Path:"));
        linePath_ = new QLineEdit();
        linePath_->setMinimumWidth(400);

        // Boutons OK et Cancel
        QPushButton *okButton = new QPushButton();
        okButton->setText("OK");
        okButton->setDefault(true);
        QPushButton *cancelButton = new QPushButton();
        cancelButton->setText("Cancel");

        // Layout pour les boutons
        QHBoxLayout *hLayout = new QHBoxLayout();
        hLayout->addStretch();
        hLayout->addWidget(okButton);
        hLayout->addStretch();
        hLayout->addWidget(cancelButton);
        hLayout->addStretch();

        // Séparateur
        QFrame *separator = new QFrame();
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Sunken);

        // Layout principal
        QVBoxLayout *vLayout = new QVBoxLayout();
        vLayout->setSizeConstraint(QLayout::SetFixedSize);
        vLayout->addWidget(nameLabel);
        vLayout->addWidget(lineEdit_);
        vLayout->addWidget(pathLabel);
        vLayout->addWidget(linePath_);
        vLayout->addWidget(separator);
        vLayout->addLayout(hLayout);
        setLayout(vLayout);
        okButton->setStyleSheet(
            "QPushButton {"
            "  background-color: #4CAF50;"
            "  color: white;"
            "  border: 2px solid #4CAF50;"
            "  border-radius: 8px;"
            "  padding: 8px;"
            "}"
            "QPushButton:hover {"
            "  background-color: white;"
            "  color: #4CAF50;"
            "}");
        cancelButton->setStyleSheet(
            "QPushButton {"
            "  background-color:rgb(175, 76, 76);"
            "  color: white;"
            "  border: 2px solidrgb(175, 76, 76);"
            "  border-radius: 8px;"
            "  padding: 8px;"
            "}"
            "QPushButton:hover {"
            "  background-color: white;"
            "  color:rgb(175, 76, 76);"
            "}");

        // Connexions des boutons
        connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
        connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    }

    const QString SaveCellDialog::getCellName() const { return lineEdit_->text(); }
    const QString SaveCellDialog::getPathName() const { return linePath_->text(); }

    void SaveCellDialog::setCellName(const QString &name) { return lineEdit_->setText(name); }
    void SaveCellDialog::setPathName(const QString &name) { return linePath_->setText(name); }

    bool SaveCellDialog::run(QString &nameCell, QString &namePath)
    {
        setCellName(nameCell);
        setPathName(namePath);

        int dialogResult = exec();
        nameCell = getCellName();
        namePath = getPathName();

        return (dialogResult == Accepted);
    }
}