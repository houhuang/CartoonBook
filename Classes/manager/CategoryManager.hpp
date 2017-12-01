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

class Picture
{
public:
    string id;
    string url;
    string picName;
};

class Chapter
{
public:
    string name;
    string id;
    string floder;
};

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
    string isNew;
    string isLock;
    
    vector<Chapter> _chapterInfo;
    vector<Picture> _pictureInfo;
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
    
public:
    vector<Category> _CategoryInfo;
    vector<Cartoon> _CartoonInfo;

};

#endif /* CategoryManager_hpp */
