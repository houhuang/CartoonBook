//
//  DownloadManager.cpp
//  CartoonBook
//
//  Created by houhuang on 17/11/28.
//
//

#include "DownloadManager.hpp"
#include "CategoryManager.hpp"
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

void DownloadManager::downloadCartooninfo(string categoryName)
{//name=&type=%E5%B0%91%E5%B9%B4%E6%BC%AB%E7%94%BB&skip=&finish=1&key=f215dcc3a02924a4bda08f8d7b271c54
    
    stringstream url;
    url<<"";
    url<<"https://japi.juhe.cn/comic/book?name=&type=";
    url<<"%e8%80%bd%e7%be%8e%e6%bc%ab%e7%94%bb";
    url<<"&skip=&finish=1&key=f215dcc3a02924a4bda08f8d7b271c54";

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
                
                for (int i = 0; i < bookList.Size(); ++i)
                {
                    Cartoon cartoon;
                    rapidjson::Value& cartoonInfo = bookList[i];
                    cartoon.name = cartoonInfo["name"].GetString();
                    cartoon.categoryId = "4";
                    cartoon.area = cartoonInfo["area"].GetString();
                    cartoon.des = cartoonInfo["des"].GetString();
                    if (cartoonInfo.HasMember("coverImg"))
                    {
                        cartoon.coverUrl = cartoonInfo["coverImg"].GetString();
                    }
                    
                    cartoon.isfinish = "1";
                    
                    xCMInstance->getCartoonInfo().push_back(cartoon);
                }
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
                
                string path = FileUtils::getInstance()->getWritablePath() + lRequest->getTag() + "/chapter.csv";
                this->saveMyArtDataTofile(_vec, path);
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


