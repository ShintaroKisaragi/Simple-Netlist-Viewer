#pragma once
#include "Node.h"
#include "Instance.h"

//using namespace Netlist;

namespace Netlist {
    class Cell;
    class Net;
    
    class Term{
        public: 
           enum Type      { Internal=1, External=2 }; //instance ou cell si external
           enum Direction { In=1, Out=2, Inout=3, Tristate=4, Transcv=5, Unknown=6 };
        private:
            void*         owner_;
            std::string   name_;
            Direction     direction_;
            Type          type_;
            Net*          net_;
            NodeTerm          node_;
        public: 
            inline bool               isInternal   () const;
            inline bool               isExternal   () const;
            inline const std::string& getName      () const;
            inline NodeTerm*              getNode      ();
            inline Net*               getNet       () const;
            inline Cell*              getCell      () const;
                   Cell*              getOwnerCell () const;
            inline Instance*          getInstance  () const;
            inline Direction          getDirection () const;
            inline Point              getPosition  () const;
            inline Type               getType      () const;

        public:
            void setNet(Net*);
            void setNet(std::string name); // name du Net
            inline void  setDirection ( Direction d);
            void  setPosition  ( const Point& );
            void  setPosition  ( int x, int y );
            void toXml(std::ostream& o);
            static Term* fromXml(Cell*, xmlTextReaderPtr);

        public: 
            Term ( Cell*    , const std::string& name, Direction ); // constructeur de cell
            Term ( Instance*, const Term* modelTerm );// constructeur pour term d'une instance
            ~Term ();
        public:
            static std::string  toString    ( Type );
            static std::string  toString    ( Direction );
            static Direction    toDirection ( std::string );

    };

             inline bool        Term:: isInternal   ()const{
                return type_==Term::Type::Internal;
             }

             inline bool               Term::isExternal   ()const{
                return type_==Term::Type::External;
             }

             inline const std::string& Term::getName  ()const {
                return name_;
             }
             inline NodeTerm*              Term::getNode      (){
               return &node_; //ici on veut envoyer l'adresse de notre node
             }

             inline Net*               Term::getNet       () const{
                return net_;
             }

             inline Instance*          Term::getInstance  () const{

               return (type_==Internal)? static_cast<Instance*> (owner_):NULL;// si le term est à l'interieur, il s'agit d'une instance donc cast l'owner en instance*
             }

             inline Cell*              Term::getCell      () const{
                return (type_==External)? static_cast<Cell*> (owner_):NULL; // meme chose pour la fonction précedente mais avec un term externe
             }
             inline Term::Direction          Term::getDirection () const{
               return direction_;
             }
             inline Point              Term::getPosition  () const{
                return node_.getPosition();
             }
             inline Term::Type               Term::getType      () const{
                return type_;
             }
             inline void  Term::setDirection ( Direction d){
                  direction_=d;
             }




}