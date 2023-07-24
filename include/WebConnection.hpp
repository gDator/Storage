#ifndef WEB_CONNECTION_HPP
#define WEB_CONNECTION_HPP

#define CURL_STATICLIB
#include "cpr/cpr.h"

namespace dist
{
class DistributorConnector
{
    void test();
};

class FarnellConnector : public DistributorConnector
{

};

class MouserConnector : public DistributorConnector
{

};

class DigikeyConnector : public DistributorConnector
{

};
}

#endif /*WEB_CONNECTION_HPP*/