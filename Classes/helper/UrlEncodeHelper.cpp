//
//  UrlEncodeHelper.cpp
//  CartoonBook
//
//  Created by houhuang on 17/11/28.
//
//

#include "UrlEncodeHelper.hpp"

static UrlEncodeHelper* _instance = nullptr;

UrlEncodeHelper* UrlEncodeHelper::getInstance()
{
    if (!_instance)
    {
        _instance = new UrlEncodeHelper();
    }
    
    return _instance;
}


