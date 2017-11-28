//
//  CategoryManager.hpp
//  CartoonBook
//
//  Created by houhuang on 17/11/28.
//
//

#ifndef CategoryManager_hpp
#define CategoryManager_hpp

#include <stdio.h>
USING_NS_CC;

#define xCMInstance CategoryManager::getInstance()

using namespace std;

class Cartoon
{
public:
    string categoryId;
    string name;
    string area;
    string des;
    string coverUrl;
    string isfinish;
    string folder;
};

class Category
{
public:
    string name;
    string id;
};

class CategoryManager
{
public:
    static CategoryManager* getInstance();
    
    void readCategoryFromCsv();
    vector<Category> getCategoryInfo();
    
    void readCartoonFromCsv();
    void saveCartoonInfoToCsv();
    vector<Cartoon>& getCartoonInfo();
private:
    void saveMyArtDataTofile(const vector<list<string>>& lCsvData, const string& fileName);
    
private:
    vector<Category> _CategoryInfo;
    vector<Cartoon> _CartoonInfo;

};

#endif /* CategoryManager_hpp */
