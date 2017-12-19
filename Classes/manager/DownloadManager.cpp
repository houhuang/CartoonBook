 //
//  DownloadManager.cpp
//  CartoonBook
//
//  Created by houhuang on 17/11/28.
//
//

#include "DownloadManager.hpp"
#include "CSVParse.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32) && (CC_TARGET_PLATFORM != CC_PLATFORM_WP8) && (CC_TARGET_PLATFORM != CC_PLATFORM_WINRT)
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <vector>
#endif

static DownloadManager* _instance = nullptr;

DownloadManager* DownloadManager::getInstance()
{
    if (!_instance)
    {
        _instance = new DownloadManager();
    }
    

    return _instance;
}

DownloadManager::DownloadManager()
{
    _downloadSwitch = false;
}

void DownloadManager::downloadCartooninfo(string categoryName)
{//name=&type=%E5%B0%91%E5%B9%B4%E6%BC%AB%E7%94%BB&skip=&finish=1&key=f215dcc3a02924a4bda08f8d7b271c54
    
    string type = this->UrlGB2312("青年漫画");
    
    stringstream url;
    url<<"";
    url<<"https://japi.juhe.cn/comic/book?name=&type=";
    url<<type;
    url<<"&skip=40&finish=1&key=9ca9f71fc3e8dd6eec0c9a0c29bd895e";

    log("%s", url.str().c_str());
    
    HttpRequest* lRequest = new HttpRequest;
    lRequest->setUrl(url.str().c_str());
    lRequest->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    lRequest->setResponseCallback([this, lRequest](network::HttpClient* client, network::HttpResponse* response){
        
        if (!response || !response->isSucceed())
        {
            log("reponse failed!    获取网络资源失败！");
        }else
        {

            std::vector<char> *buffer = response->getResponseData();
            std::string buffff(buffer->begin(), buffer->end());
            
            rapidjson::Document _doc;
            _doc.Parse<0>(buffff.c_str());
            if (_doc.HasMember("result"))
            {
                rapidjson::Value& result = _doc["result"];
                rapidjson::Value& bookList = result["bookList"];
                
                ////////////////////////////
                int orignalFloder = 141;
                for (int i = 0; i < bookList.Size(); ++i)
                {
                    Cartoon cartoon;
                    rapidjson::Value& cartoonInfo = bookList[i];
                    cartoon.isNew = "true";
                    cartoon.isLock = "false";
                    cartoon.name = cartoonInfo["name"].GetString();
                    cartoon.categoryId = "2";
                    cartoon.area = cartoonInfo["area"].GetString();
                    cartoon.des = cartoonInfo["des"].GetString();
                    if (cartoonInfo.HasMember("coverImg"))
                    {
                        cartoon.coverUrl = cartoonInfo["coverImg"].GetString();
                    }
                    
                    cartoon.isfinish = "1";
                    
                    char buf[5];
                    sprintf(buf, "%04d", (orignalFloder+i));
                    cartoon.folder = buf;
                    
                    xCMInstance->getCartoonInfo().push_back(cartoon);
                }
                
                xCMInstance->saveCartoonInfoToCsv();
            }
        }
        
        lRequest->release();
    });
    
    HttpClient::getInstance()->send(lRequest);
}

void DownloadManager::downloadChapter(string cartoonName, string folder)
{
    //comicName=%E4%B8%80%E9%97%AA%E4%B8%80%E9%97%AA%E4%BA%AE%E6%99%B6%E6%99%B6&skip=&key=f215dcc3a02924a4bda08f8d7b271c54
    stringstream url;
    url<<"";
    url<<"http://japi.juhe.cn/comic/chapter?comicName=";
    url<<cartoonName;
    url<<"&skip=&key=f215dcc3a02924a4bda08f8d7b271c54";
    
    log("%s", url.str().c_str());
    
    HttpRequest* lRequest = new HttpRequest;
    lRequest->setUrl(url.str().c_str());
    lRequest->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    lRequest->setResponseCallback([this, lRequest](network::HttpClient* client, network::HttpResponse* response){
        
        if (!response || !response->isSucceed())
        {
            log("reponse failed!    获取网络资源失败！");
        }else
        {
            std::vector<char> *buffer = response->getResponseData();
            std::string buffff(buffer->begin(), buffer->end());
            
            vector<list<string>> _vec;
            
            list<string> ll;
            ll.push_back("name");
            ll.push_back("id");
            ll.push_back("folder");
            _vec.push_back(ll);
            
            rapidjson::Document _doc;
            _doc.Parse<0>(buffff.c_str());
            if (_doc.HasMember("result"))
            {
                rapidjson::Value& result = _doc["result"];
                rapidjson::Value& bookList = result["chapterList"];
                
                for (int i = 0; i < bookList.Size(); ++i)
                {
                    list<string> _list;
                    
                    rapidjson::Value& cartoonInfo = bookList[i];
                    _list.push_back(cartoonInfo["name"].GetString());
                    _list.push_back(to_string(cartoonInfo["id"].GetInt()));
                    
                    char buf[10];
                    sprintf(buf, "%04d", (i+1));
                    _list.push_back(buf);
                    
                    _vec.push_back(_list);
                }
                
                
                string path = FileUtils::getInstance()->getWritablePath() + "chapter/";
                if (!FileUtils::getInstance()->isFileExist(path))
                {
                    this->createDirectory(path.c_str());
                }
                
                string path2 = FileUtils::getInstance()->getWritablePath() + "chapter/" + lRequest->getTag() + "_chapter.csv";
                this->saveMyArtDataTofile(_vec, path2);
                
                
            }
        }
        
        lRequest->release();
    });
    
    lRequest->setTag(folder.c_str());
    HttpClient::getInstance()->send(lRequest);
}

void DownloadManager::downloadCover(string folder, string url)
{
   
    if (url == "")
    {
        return;
    }
    
    
    HttpRequest* lRequest = new HttpRequest();
    lRequest->setUrl(url.c_str());
    lRequest->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    lRequest->setResponseCallback([this, lRequest](network::HttpClient* client, network::HttpResponse* response){
        
        if (!response || !response->isSucceed())
        {
            log("response failed!   获取网络资源失败！");
        }
        
        std::vector<char>* buffer = response->getResponseData();
        std::string buffff(buffer->begin(), buffer->end());
        
        string path = FileUtils::getInstance()->getWritablePath() + lRequest->getTag() + "/";
        if (!FileUtils::getInstance()->isFileExist(path))
        {
            this->createDirectory(path.c_str());
            
            string name = path + "cover.png";
            
            FILE* fp = fopen(name.c_str(), "wb+");
            fwrite(buffff.c_str(), 1, buffer->size(), fp);
            fclose(fp);
        }
        
        lRequest->release();
        
    });
    
    lRequest->setTag(folder.c_str());
    HttpClient::getInstance()->send(lRequest);
}


void DownloadManager::startDownload(Chapter chapter, string cartoonName, string carttonFolder)
{
    const char* gg;
    gg = cartoonName.c_str();
    string name = xDMInstance->UrlGB2312(gg);
    
    _cartoonName = name;
    saveFolder = carttonFolder;
    _needDownloadChapter.push_back(chapter);
    this->downloadPictureCsv();
}

void DownloadManager::downloadPictureCsv()
{
    if (_downloadSwitch) return;
    
    _downloadSwitch = true;
    
    Chapter chapter = _needDownloadChapter.at(0);
    
    //comicName=%E7%A0%B4%E5%9D%8F%E5%85%BD&id=55501&key=f215dcc3a02924a4bda08f8d7b271c54
    
    
    
    stringstream ss;
    ss<<"";
    ss<<"http://japi.juhe.cn/comic/chapterContent?comicName=";
    ss<<_cartoonName;
    ss<<"&id=";
    ss<<chapter.id;
    ss<<"&key=9ca9f71fc3e8dd6eec0c9a0c29bd895e";
    
    HttpRequest* lRequest = new HttpRequest;
    lRequest->setUrl(ss.str().c_str());
    lRequest->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    lRequest->setResponseCallback([this, lRequest](network::HttpClient* client, network::HttpResponse* response){
        
        if (!response || !response->isSucceed())
        {
            log("reponse failed!    获取网络资源失败！");
        }else
        {
            std::vector<char> *buffer = response->getResponseData();
            std::string buffff(buffer->begin(), buffer->end());
            
            rapidjson::Document _doc;
            _doc.Parse<0>(buffff.c_str());
            if (_doc.HasMember("result"))
            {
                rapidjson::Value& result = _doc["result"];
                rapidjson::Value& bookList = result["imageList"];
                
                for (int i = 0; i < bookList.Size(); ++i)
                {
                    list<string> _list;
                    
                    rapidjson::Value& cartoonInfo = bookList[i];
                    _list.push_back(lRequest->getTag());
                    _list.push_back(to_string(cartoonInfo["id"].GetInt()));
                    _list.push_back(cartoonInfo["imageUrl"].GetString());
                    
                    vecPic.push_back(_list);
                }
            }

            _needDownloadChapter.erase(_needDownloadChapter.begin());
            
            _downloadSwitch = false;
            
            if (_needDownloadChapter.size() > 0)
            {
                this->downloadPictureCsv();
            }else
            {
                string folderPath = FileUtils::getInstance()->getWritablePath() + "pictureCsv/";
                string path = FileUtils::getInstance()->getWritablePath() + "pictureCsv/" + saveFolder + "_picture.csv";
                if (!FileUtils::getInstance()->isFileExist(folderPath))
                {
                    this->createDirectory(folderPath.c_str());
                }
                
                this->saveMyArtDataTofile(vecPic, path);
                
                vecPic.clear();
                
                xCMInstance->_CartoonInfo.erase(xCMInstance->_CartoonInfo.begin());
                if (xCMInstance->_CartoonInfo.size() > 0)
                {
                    Cartoon car = xCMInstance->_CartoonInfo.at(0);
                    for (int x = 0; x<car._chapterInfo.size(); ++x)
                    {
                        xDMInstance->startDownload(car._chapterInfo.at(x), car.name, car.folder);
                    }
                }else
                {
                    log("download finish!!!");
                }
                
                
            }
        }
        
        lRequest->release();
    });
    
    lRequest->setTag(chapter.floder.c_str());
    HttpClient::getInstance()->send(lRequest);

}

void DownloadManager::startDownloadPicture(Picture picture, string cartoonFolder)
{
    saveFolder = cartoonFolder;
    _needDownloadPicture.push_back(picture);
    this->downloadPicture();
}

void DownloadManager::downloadPicture()
{
    if (_downloadSwitch) return;
    
    _downloadSwitch = true;
    
    Picture picture = _needDownloadPicture.at(0);
    
    HttpRequest* lRequest = new HttpRequest;
    lRequest->setUrl(picture.url.c_str());
    lRequest->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    lRequest->setResponseCallback([this, lRequest](network::HttpClient* client, network::HttpResponse* response){
        
        if (!response || !response->isSucceed())
        {
            log("%s",saveFolder.c_str());
            log("reponse failed!    获取网络资源失败！");
        }else
        {
            std::vector<char>* buffer = response->getResponseData();
            std::string buffff(buffer->begin(), buffer->end());
            
            string path = FileUtils::getInstance()->getWritablePath() + saveFolder + "/" + lRequest->getTag();
            if (!FileUtils::getInstance()->isFileExist(path))
            {

                FILE* fp = fopen(path.c_str(), "wb+");
                fwrite(buffff.c_str(), 1, buffer->size(), fp);
                fclose(fp);
            }
            
            log("%lu",_needDownloadPicture.size());
            _needDownloadPicture.erase(_needDownloadPicture.begin());
            log("%lu",_needDownloadPicture.size());
            _downloadSwitch = false;
            
            if (_needDownloadPicture.size() > 0)
            {
                this->downloadPicture();
            }else
            {

                xCMInstance->_CartoonInfo.erase(xCMInstance->_CartoonInfo.begin());
                if (xCMInstance->_CartoonInfo.size() > 0)
                {
                    Cartoon car = xCMInstance->_CartoonInfo.at(0);
                    for (int x = 0; x < car._pictureInfo.size(); ++x)
                    {
                        xDMInstance->startDownloadPicture(car._pictureInfo.at(x), car.folder);
                    }
                }else
                {
                    log("download finish!!!");
                }
                
                
            }
        }
        
        lRequest->release();
    });
    
    lRequest->setTag(picture.picName.c_str());
    HttpClient::getInstance()->send(lRequest);
}


bool DownloadManager::createDirectory(const char *path)
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    mode_t processMask = umask(0);
    int ret = mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
    umask(processMask);
    if (ret != 0 && (errno != EEXIST))
    {
        return false;
    }
    
    return true;
#else
    BOOL ret = CreateDirectoryA(path, nullptr);
    if (!ret && ERROR_ALREADY_EXISTS != GetLastError())
    {
        return false;
    }
    return true;
#endif
    

}

string DownloadManager::UrlGB2312(const char * str)
{
    string dd;
    size_t len = strlen(str);
    for (size_t i=0;i<len;i++)
    {
        if(isalnum((unsigned char)str[i]))
        {
            char tempbuff[2];
            sprintf(tempbuff,"%c",str[i]);
            dd.append(tempbuff);
        }
        else if (isspace((unsigned char)str[i]))
        {
            dd.append("+");
        }
        else
        {
            char tempbuff[4];
            sprintf(tempbuff,"%%%X%X",((unsigned char*)str)[i] >>4,((unsigned char*)str)[i] %16);
            dd.append(tempbuff);
        }
        
    }
    return dd;
}


void DownloadManager::saveMyArtDataTofile(const vector<list<string>>& lCsvData, const string& fileName)
{
//    string csvFilePath = FileUtils::getInstance()->getWritablePath() + fileName;
    
    ofstream _streamToFile(fileName);
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


