//
//  UrlEncodeHelper.hpp
//  CartoonBook
//
//  Created by houhuang on 17/11/28.
//
//

#ifndef UrlEncodeHelper_hpp
#define UrlEncodeHelper_hpp

#include <stdio.h>
#include <iostream>
using namespace std;

#define xUNInstance UrlEncodeHelper::getInstance()

class UrlEncodeHelper
{
public:
    UrlEncodeHelper* getInstance();

};

#endif /* UrlEncodeHelper_hpp */
