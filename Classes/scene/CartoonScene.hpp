//
//  CartoonScene.hpp
//  CartoonBook
//
//  Created by houhuang on 17/11/28.
//
//

#ifndef CartoonScene_hpp
#define CartoonScene_hpp

#include <stdio.h>
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

class CartoonScene : public Scene, public TableViewDelegate, public TableViewDataSource
{
public:
    CREATE_FUNC(CartoonScene);
    
    bool init();
    
public:
    void createTable();
    
public:
    virtual void tableCellTouched(TableView* table, TableViewCell* cell);
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx);
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(TableView *table);
    
};

#endif /* CartoonScene_hpp */
