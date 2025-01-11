#pragma once
#include "XmlUtil.h"
#include <iostream>
#include "Symbol.h"
#include "Term.h"
#include "Instance.h"

namespace Netlist
{
    class Shape
    {
        protected:
            Symbol* symbol_;

        public:
            Shape(Symbol* symbol);
            virtual ~Shape();
            static Shape* fromXml(Symbol*, xmlTextReaderPtr);
            Symbol* getSymbol() const;
            virtual void toXml(std::ostream& o) const {o << "<Shape/>\n";}
            virtual Box getBox() const {return Box();}
    };

    class LineShape : public Shape
    {
        private:
            int x1_, y1_, x2_, y2_;

        public:
            LineShape(Symbol* symbol, int x1, int y1, int x2, int y2);
            ~LineShape();
            static Shape* fromXml(Symbol*, xmlTextReaderPtr);
            void toXml(std::ostream&) const override;
            int getX1() const {return x1_;}
            int getX2() const {return x2_;}
            int getY1() const {return y1_;}
            int getY2() const {return y2_;}
            Box getBox() const override {return Box(x1_, y1_, x2_, y2_);}
    };

    class BoxShape : public Shape
    {
        private:
            Box* box_;

        public:
            BoxShape(Symbol* symbol, int x1, int y1, int x2, int y2);
            ~BoxShape();
            static Shape* fromXml(Symbol*, xmlTextReaderPtr);
            void toXml(std::ostream&) const override;
            int getX1() const {return box_->getX1();}
            int getX2() const {return box_->getX2();}
            int getY1() const {return box_->getY1();}
            int getY2() const {return box_->getY2();}
            Box getBox() const override {return *box_;}
    };

    class EllipseShape : public Shape
    {
        private:
            Box* box_;
            
        public:
            EllipseShape(Symbol* symbol, int x1, int y1, int x2, int y2);
            ~EllipseShape();
            static Shape* fromXml(Symbol*, xmlTextReaderPtr);
            void toXml(std::ostream&) const override;
            int getX1() const {return box_->getX1();}
            int getX2() const {return box_->getX2();}
            int getY1() const {return box_->getY1();}
            int getY2() const {return box_->getY2();}
            Box getBox() const override {return Box(getX1(), getY1(), getX2(), getY2());}
    };

    class ArcShape : public Shape
    {
        private:
            Box* box_;
            int start_;
            int span_;

        public:
            ArcShape(Symbol* symbol, int x1, int y1, int x2, int y2, int start, int span);
            ~ArcShape();
            static Shape* fromXml(Symbol*, xmlTextReaderPtr);
            void toXml(std::ostream&) const override;
            int getX1() const {return box_->getX1();}
            int getX2() const {return box_->getX2();}
            int getY1() const {return box_->getY1();}
            int getY2() const {return box_->getY2();}
            int getStart() const {return start_;}
            int getSpan() const {return span_;}
            Box getBox() const override {return Box(getX1(), getY1(), getX2(), getY2());}
    };

    class TermShape : public Shape
    {
        public:
            enum NameAlign{TopLeft=1, TopRight=2, BottomLeft=3, BottomRight=4};
            TermShape(Symbol* symbol,int x1, int y1, std::string term, NameAlign align);
            ~TermShape();
            static Shape* fromXml(Symbol*, xmlTextReaderPtr);
            void toXml(std::ostream&) const override;
            static NameAlign strToAlign(std::string);
            static std::string alignToStr(NameAlign);
            NameAlign getAlign() const {return align_;}
            int getX1() const {return x1_;}
            int getY1() const {return y1_;}
            Term* getTerm() const {return term_;}
            Box getBox() const override {return Box(x1_, y1_, x1_, y1_);}
            

        private:
            int x1_, y1_;
            Term* term_;
            NameAlign align_;
    };
}