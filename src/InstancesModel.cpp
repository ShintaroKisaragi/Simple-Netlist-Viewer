#include "InstancesModel.h"
#include "Instance.h"
#include "Cell.h"
namespace Netlist{
    //constructeur & destructeur
    InstancesModel::InstancesModel(QObject *parent)
    :QAbstractTableModel(parent), cell_( nullptr )
    {}

    InstancesModel::~InstancesModel()
    {}

    void InstancesModel::setCell(Cell* cell ){
        emit layoutAboutToBeChanged();//  Signale que la structure des données va changer Cela évite que la vue (par ex., QTableView) essaie d'accéder à des données qui sont en cours de modification.
        cell_ = cell;
        emit layoutChanged();// Signale que la structure des données a changé
    }

    int InstancesModel::rowCount( const QModelIndex & parent ) const
    { return ( cell_ ) ? cell_->getInstances().size():0;}// ici on compte le nombre de cellule utilisé pour le modele actuel

    int InstancesModel::columnCount(const QModelIndex &parent ) const
    { return 2; } // Instance et MasterCell

    
    QVariant InstancesModel::data(const QModelIndex &index, int role ) const //fournit les données d'une cellule dans la table.
    //QModelIndex : Représente une position dans le tableau (ligne et colonne)
    //role : Indique ce que l'on veut afficher dans la cellule (texte, fond, etc.).
    {
        if ( not cell_ or not index.isValid ()) return QVariant ();
        /*Si cell_ est nul (pas de cellule associée).
        Ou si index n'est pas valide (ligne ou colonne hors limite).
        Retourne un QVariant vide dans ces cas-là.
        */
        if ( role == Qt::DisplayRole ) //Qt::DisplayRole : C'est le rôle par défaut pour afficher du texte dans la table.
        {
            int row = index.row(); // Ligne sélectionnée
            switch(index.column())// colonne sélectionnée
            {
                case 0:return cell_->getInstances()[row]->getName().c_str(); //1er colonne
                case 1:return cell_->getInstances()[row]->getMasterCell()->getName().c_str();
            }
        }
        return QVariant();
    }

    QVariant InstancesModel::headerData ( int section, Qt::Orientation orient, int role ) const
    {
        /*
        section : Numéro de la colonne.
        orient : Orientation (horizontal pour les colonnes, vertical pour les lignes).
        role : Ce qui doit être affiché (comme Qt::DisplayRole pour du texte).
        */
        if ( orient != Qt::Horizontal) return QVariant ();//Ici, seuls les en-têtes horizontaux (colonnes) sont pris en compte.
        if ( role == Qt::DisplayRole ) {
        
            switch ( section ) 
            {
                case 0: return "Instance";// En-tête de la première colonne
                case 1: return "MasterCell";
            }
        }
        return QVariant();
        }

    Cell* InstancesModel::getModel( int row )const
    {
        if ( not cell_ ) return nullptr ;
        if ( row >= ( int )cell_->getInstances().size())return NULL ;//Si row dépasse la taille : Retourne NULL (ligne invalide).
        return cell_->getInstances()[row]->getMasterCell();// ici permet d' acceder au mastercell de l'instance que l'on a choisit
    }
}