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
USING_NS_CC;

#define xDMInstance DownloadManager::getInstance()

using namespace std;
using namespace network;

class DownloadManager
{
public:
    static DownloadManager* getInstance();
    
public:
    void downloadCartooninfo(string categoryName);
    void downloadChapter();
    
    void downloadCover(string folder, string url);
    
//private:
    bool createDirectory(const char *path);
    
};

#endif /* DownloadManager_hpp */