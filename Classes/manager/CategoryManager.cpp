//
//  CategoryManager.cpp
//  CartoonBook
//
//  Created by houhuang on 17/11/28.
//
//

#include "CategoryManager.hpp"
#include "CSVParse.h"
#include "DownloadManager.hpp"

static CategoryManager* _instance = nullptr;

CategoryManager* CategoryManager::getInstance()
{
    if (!_instance)
    {
        _instance = new CategoryManager();
    }
    
    return _instance;
}

void CategoryManager::readCategoryFromCsv()
{
    CSVParse* lCsv = CSVParse::create("category.csv");
    if (lCsv)
    {
        unsigned int row = lCsv->getRows();
        for (int i = 1; i < row; ++i)
        {
            Category category;
            category.name = lCsv->getDatas(i, "name");
            
            _CategoryInfo.push_back(category);
        }
    }
}

vector<Category> CategoryManager::getCategoryInfo()
{
    return _CategoryInfo;
}

vector<Cartoon>& CategoryManager::getCartoonInfo()
{
    return _CartoonInfo;
}

void CategoryManager::readCartoonFromCsv()
{
    CSVParse* lCsv = CSVParse::create("cartoon.csv");
    if (lCsv)
    {
        unsigned int row = lCsv->getRows();
        for (int i = 65; i < row; ++i)
        {
            Cartoon cartoon;
            cartoon.name = lCsv->getDatas(i, "name");
            cartoon.categoryId = lCsv->getDatas(i, "cateId");
            cartoon.area = lCsv->getDatas(i, "area");
            cartoon.des = lCsv->getDatas(i, "des");
            cartoon.coverUrl = lCsv->getDatas(i, "coverUrl");
            cartoon.isfinish = lCsv->getDatas(i, "isfinsh");
            cartoon.folder = lCsv->getDatas(i, "folder");
            
            _CartoonInfo.push_back(cartoon);
        }
    }
    
    
    //read chapter
    for (int i = 0; i < _CartoonInfo.size(); ++i)
    {
        string path = FileUtils::getInstance()->getWritablePath() + _CartoonInfo.at(i).folder + "/chapter.csv";
        if (FileUtils::getInstance()->isFileExist(path))
        {
            CSVParse* lCsv = CSVParse::create(path.c_str());
            unsigned int row = lCsv->getRows();
            for (int j = 1; j < row; ++j)
            {
                Chapter chapter;
                chapter.name = lCsv->getDatas(j, "name");
                chapter.id = lCsv->getDatas(j, "id");
                chapter.floder = lCsv->getDatas(j, "folder");
                
                _CartoonInfo.at(i)._chapterInfo.push_back(chapter);
            }
        }
    }
    
    Cartoon _car = _CartoonInfo.at(0);
    
    for (int x = 0; x<_car._chapterInfo.size(); ++x)
    {
        xDMInstance->startDownload(_car._chapterInfo.at(x), _car.name, _car.folder);
    }

    return;
    
    for (int i = 0; i < _CartoonInfo.size(); ++i)
    {
        Cartoon cartoon = _CartoonInfo.at(i);
        
        
        
        const char* gg;
        gg = cartoon.name.c_str();
        string ss = xDMInstance->UrlGB2312(gg);
        
        xDMInstance->downloadChapter(ss, cartoon.folder);

    }
}

void CategoryManager::saveCartoonInfoToCsv()
{
    vector<list<string>> _vec;
    list<string> _list;
    _list.push_back("name,cateId,area,des,coverUrl,isfinsh");
    _vec.push_back(_list);
    for (int i = 0; i < _CartoonInfo.size(); ++i)
    {
        Cartoon cartoon = _CartoonInfo.at(i);
        list<string> lList;
        lList.push_back(cartoon.name);
        lList.push_back(cartoon.categoryId);
        lList.push_back(cartoon.area);
        
        string des = "\"" + cartoon.des + "\"";
        lList.push_back(des);
        
        lList.push_back(cartoon.coverUrl);
        lList.push_back(cartoon.isfinish);
        
        _vec.push_back(lList);
    }
    
    this->saveMyArtDataTofile(_vec, "cartoon.csv");
}

void CategoryManager::saveMyArtDataTofile(const vector<list<string>>& lCsvData, const string& fileName)
{
    string csvFilePath = FileUtils::getInstance()->getWritablePath() + fileName;
    
    ofstream _streamToFile(csvFilePath);
    ///    判断打开文件是否成功
    if (!_streamToFile.is_open())
        return;
    /// 本地变量
    static char chQuote = '"';
    static char chComma = ',';
    /// Loop through each list of string in vector
    for (vector<list<string> >::const_iterator vIt = lCsvData.begin(); vIt != lCsvData.end(); vIt ++) {
        /// Loop through each string in list
        for (list<string>::const_iterator lIt = vIt->begin(); lIt != vIt->end(); lIt ++) {
            /// Separate this value from previous
            if (vIt->begin() != lIt)
                _streamToFile.put(chComma);
            /// 考虑string中可能有,或"的情况，这就要特殊包装。
            bool bComma = (lIt->find(chComma) != lIt->npos);
            bool bQuote = (lIt->find(chQuote) != lIt->npos);
            /// 真的含有,或"的情况
            if (bComma || bQuote) {
                _streamToFile.put(chQuote);
                
                
                if (bQuote) {
                    for (string::const_iterator chIt = lIt->begin(); chIt != lIt->end(); chIt ++ ) {
                        // Pairs of quotes interpreted as single quote
                        if (chQuote == *chIt)
                            _streamToFile.put(chQuote);
                        
                        _streamToFile.put(*chIt);
                    }
                }
                else
                    _streamToFile << *lIt;
                
                
                _streamToFile.put(chQuote);
            }
            else
                _streamToFile << *lIt;
        }
        /// 换行
        _streamToFile << endl;
    }
}
