//
//  DownloadManager.hpp
//  CartoonBook
//
//  Created by houhuang on 17/11/28.
//
//

#ifndef DownloadManager_hpp
#define DownloadManager_hpp

#include <stdio.h>
#include "network/HttpRequest.h"
#include "network/HttpClient.h"
#include "cocos-ext.h"
#include "CategoryManager.hpp"
USING_NS_CC;

#define xDMInstance DownloadManager::getInstance()

using namespace std;
using namespace network;

class DownloadManager
{
public:
    static DownloadManager* getInstance();
    
    DownloadManager();
    
public:
    void downloadCartooninfo(string categoryName);
    
    void downloadChapter(string cartoonName, string folder);
    
    void downloadCover(string folder, string url);
    
//private:
    bool createDirectory(const char *path);
    
    //download picture csv
    void downloadPictureCsv();
    void startDownload(Chapter chapter, string cartoonName, string carttonFolder);
    vector<Chapter> _needDownloadChapter;
    
    bool _downloadSwitch;
    string saveFolder;
    string _cartoonName;
    
    void saveToCsv();
    vector<list<string>> vecPic;
    
    
    string StrToHex(string strData);
    string UrlGB2312(const char * str);
    
    void saveMyArtDataTofile(const vector<list<string>>& lCsvData, const string& fileName);
};

#endif /* DownloadManager_hpp */
