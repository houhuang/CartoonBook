//
//  CartoonScene.cpp
//  CartoonBook
//
//  Created by houhuang on 17/11/28.
//
//

#include "CartoonScene.hpp"
#include "CategoryManager.hpp"

bool CartoonScene::init()
{
    if (!Scene::init()) return false;
    
    LayerColor* layer = LayerColor::create(Color4B::WHITE);
    this->addChild(layer);
    
    xCMInstance->readCartoonFromCsv();
    
    this->createTable();
    return true;
}

void CartoonScene::createTable()
{
    TableView* table = TableView::create(this, Size(this->getContentSize().width, this->getContentSize().height));
    table->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL);
    table->setVerticalFillOrder(cocos2d::extension::TableView::VerticalFillOrder::TOP_DOWN);
    
    table->setDelegate(this);
    table->ignoreAnchorPointForPosition(false);
    table->setAnchorPoint(Vec2(0.5, 0.5));
    table->setPosition(this->getContentSize()/2);
    table->reloadData();
    
    this->addChild(table);
}

void CartoonScene::tableCellTouched(TableView* table, TableViewCell* cell)
{
    
}

Size CartoonScene::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    return Size(this->getContentSize().width, 200);
}

TableViewCell* CartoonScene::tableCellAtIndex(TableView *table, ssize_t idx)
{
    TableViewCell* cell = table->dequeueCell();
    if (!cell)
    {
        cell = new TableViewCell();
        cell->autorelease();
    }
    
    cell->removeAllChildrenWithCleanup(true);
    
    Cartoon cartoon = xCMInstance->getCartoonInfo().at(idx);
    
    stringstream text;
    text<<"";
    text<<cartoon.name;
    text<<"\n";
    text<<cartoon.categoryId;
    text<<"\n";
    text<<cartoon.area;
    text<<"\n";
    text<<cartoon.des;
    text<<"\n";
    text<<cartoon.coverUrl;
    text<<"\n";
    
    Label* name = Label::createWithSystemFont(text.str(), "Arial", 20);
    name->setAlignment(cocos2d::TextHAlignment::LEFT);
    name->setDimensions(this->getContentSize().width, 200);
    name->setAnchorPoint(Vec2(0, 0.5));
    name->setPosition(Vec2(0, 100));
    name->setColor(Color3B(200, 100, 80));
    cell->addChild(name);
    
    return cell;
}

ssize_t CartoonScene::numberOfCellsInTableView(TableView *table)
{
    return xCMInstance->getCartoonInfo().size();
}