//
//  DownloadManager.cpp
//  CartoonBook
//
//  Created by houhuang on 17/11/28.
//
//

#include "DownloadManager.hpp"
#include "CategoryManager.hpp"

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














