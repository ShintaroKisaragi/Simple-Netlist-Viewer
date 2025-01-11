#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "OpenCellDialog.h"

using namespace std;

namespace Netlist {
    OpenCellDialog::OpenCellDialog( QWidget* parent )
    :QDialog ( parent )
    ,lineEdit_ ( NULL )
    {   
        setWindowTitle( "Open Cell");

        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        QLabel* label = new QLabel();
        label->setText( "Enter Cell name ( without extention )");
        lineEdit_ = new QLineEdit();
        lineEdit_->setMinimumWidth ( 400 );
        QPushButton* okButton = new QPushButton();
        okButton->setText(" OK ");
        okButton->setDefault( true );
        QPushButton* cancelButton = new QPushButton();
        cancelButton->setText(" Cancel ");
        QHBoxLayout * hLayout = new QHBoxLayout();
        hLayout->addStretch();
        hLayout->addWidget( okButton );
        hLayout->addStretch();
        hLayout->addWidget( cancelButton );
        hLayout->addStretch();
        QFrame* separator = new QFrame();
        separator->setFrameShape( QFrame::HLine );
        separator->setFrameShadow( QFrame::Sunken );
        QVBoxLayout* vLayout = new QVBoxLayout();
        vLayout->setSizeConstraint( QLayout::SetFixedSize );
        vLayout->addWidget( label );
        vLayout->addWidget( lineEdit_ );
        vLayout->addLayout( hLayout );
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

        setLayout( vLayout );
        connect( okButton , SIGNAL(clicked()) , this , SLOT( accept()) );
        connect( cancelButton , SIGNAL(clicked()) , this , SLOT( reject()) );
        okButton->setDefault(true);
    }

    bool OpenCellDialog::run( QString & name )
    {   
        OpenCellDialog* Open = new OpenCellDialog();
        Open->setCellName(name);
        int dialogResult = Open->exec();
        name = Open->getCellName();
        delete Open;
        return (dialogResult == Accepted);
    }

    const QString OpenCellDialog :: getCellName () const
    { 
        return lineEdit_->text(); 
    }

    void OpenCellDialog::setCellName(const QString &name)
    { 
        lineEdit_->setText( name ); 
    }    
}  // Netlist namespace.