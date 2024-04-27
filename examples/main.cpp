#include "libcpp-http-client.hpp"

using namespace lklibs;

void simpleGet()
{
    HttpRequest httpRequest("https://httpbun.com/get");

    // The simplest but slowest method if multiple calls will be made
    auto response = httpRequest
                    .setQueryString("param1=7&param2=test")
                    .send()
                    .get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void nonBlockingGet()
{
    HttpRequest httpRequest1("https://httpbun.com/get");
    HttpRequest httpRequest2("https://httpbun.com/get");
    HttpRequest httpRequest3("https://httpbun.com/get");

    // All requests are made one after the other without waiting for a response
    auto future1 = httpRequest1.setQueryString("param1=1&param2=test1").send();
    auto future2 = httpRequest2.setQueryString("param1=2&param2=test2").send();
    auto future3 = httpRequest3.setQueryString("param1=3&param2=test3").send();

    // Then all the answers are received. Thus, 3 requests are sent in parallel
    auto response1 = future1.get();
    auto response2 = future2.get();
    auto response3 = future3.get();

    std::cout << "Response1 Succeed: " << response1.succeed << std::endl;
    std::cout << "Response1 Http Status Code: " << response1.statusCode << std::endl;
    std::cout << "Response1 Data: " << response1.textData << std::endl;

    std::cout << "Response2 Succeed: " << response2.succeed << std::endl;
    std::cout << "Response2 Http Status Code: " << response2.statusCode << std::endl;
    std::cout << "Response2 Data: " << response2.textData << std::endl;

    std::cout << "Response3 Succeed: " << response3.succeed << std::endl;
    std::cout << "Response3 Http Status Code: " << response3.statusCode << std::endl;
    std::cout << "Response3 Data: " << response3.textData << std::endl;
}

void receiveBinaryData()
{
    HttpRequest httpRequest("https://httpbun.com/bytes/100");

    // If you need to retrieve binary data such as an image, just call the "returnAsBinary" method before send
    auto response = httpRequest
                    .returnAsBinary()
                    .send()
                    .get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;

    // In this case, you can get the data via binaryData instead of textData
    std::cout << "Data Size: " << response.binaryData.size() << std::endl;
}

void receiveError()
{
    HttpRequest httpRequest("https://httpbun.com/not_found");

    // This is an exception free library. If an error occurs, no exception is thrown
    auto response = httpRequest.send().get();

    // Instead, the succeed field of the response object is set to false
    std::cout << "Succeed: " << response.succeed << std::endl;

    // And the http status code is set to the statusCode field
    std::cout << "Http Status Code: " << response.statusCode << std::endl;

    // Also if any error message is available, it is set to the errorMessage field
    std::cout << "Error Message: " << response.errorMessage << std::endl;
}

void sendingHttpHeaders()
{
    HttpRequest httpRequest("https://httpbun.com/get?param1=7&param2=test");

    // You can send custom headers as key-value pairs
    auto response = httpRequest
                    .addHeader("Custom-Header1", "value1")
                    .addHeader("Custom-Header2", "value2")
                    .send()
                    .get();

    std::cout << "Succeed: " << response.succeed << std::endl;
}

void simplePostWithFormData()
{
    HttpRequest httpRequest("https://httpbun.com/post");

    // You can send a POST request with form data in the payload
    auto response = httpRequest
                    .setMethod(HttpMethod::POST)
                    .setPayload("param1=7&param2=test")
                    .send()
                    .get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void simplePostWithJSONData()
{
    HttpRequest httpRequest("https://httpbun.com/post");

    // You need to send the "Content-Type" as "application/json" in the HTTP Header, if you need to send json data in the payload
    auto response = httpRequest
                    .setMethod(HttpMethod::POST)
                    .setPayload(R"({"param1": 7, "param2": "test"})")
                    .addHeader("Content-Type", "application/json")
                    .send()
                    .get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void simplePutWithFormData()
{
    HttpRequest httpRequest("https://httpbun.com/put");

    // You can send a PUT request with form data in the payload just like POST
    auto response = httpRequest
                    .setMethod(HttpMethod::PUT)
                    .setPayload("param1=7&param2=test")
                    .send()
                    .get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void simpleDeleteWithFormData()
{
    HttpRequest httpRequest("https://httpbun.com/delete");

    // You can send a DELETE request with form data in the payload just like POST
    auto response = httpRequest
                    .setMethod(HttpMethod::DELETE_)
                    .setPayload("param1=7&param2=test")
                    .send()
                    .get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void simplePatch()
{
    HttpRequest httpRequest("https://httpbun.com/patch");

    // You can send a PATCH request with QueryString just like GET
    auto response = httpRequest
                    .setMethod(HttpMethod::PATCH)
                    .setQueryString("param1=7&param2=test")
                    .send()
                    .get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void ignoreSslErrors()
{
    HttpRequest httpRequest("https://self-signed-cert.httpbun.com");

    // If you need to ignore SSL errors, you can call "ignoreSslErrors" method before sending the request
    auto response = httpRequest.ignoreSslErrors().send().get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void setTLSVersion()
{
    HttpRequest httpRequest("https://httpbun.com/get");

    // You can set the TLS version to be used for the request with setTLSVersion method
    auto response = httpRequest
                    .setTLSVersion(TLSVersion::TLSv1_3)
                    .send()
                    .get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void setUserAgent()
{
    HttpRequest httpRequest("https://httpbun.com/get");

    // You can set the user agent to be used for the request with setUserAgent method
    auto response = httpRequest
                    .setUserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36 Edg/124.0.0.0")
                    .send()
                    .get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void setTimeout()
{
    HttpRequest httpRequest("https://httpstat.us/504?sleep=10000");

    // You can set the timeout in seconds
    auto response = httpRequest
                    .setTimeout(3) // 3 sec
                    .send()
                    .get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Error Message: " << response.errorMessage << std::endl;
}

void setDownloadAndUploadBandwidthLimit()
{
    HttpRequest httpRequest("https://httpbun.com/get");

    // You can set the download and upload bandwidth limit in bytes per second
    auto response = httpRequest
                    .setDownloadBandwidthLimit(10240) // 10 KB/sec
                    .setUploadBandwidthLimit(20480) // 20 KB/sec
                    .send()
                    .get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}


int main()
{
    simpleGet();

    nonBlockingGet();

    receiveBinaryData();

    receiveError();

    sendingHttpHeaders();

    simplePostWithFormData();

    simplePostWithJSONData();

    simplePutWithFormData();

    simpleDeleteWithFormData();

    simplePatch();

    ignoreSslErrors();

    setTLSVersion();

    setUserAgent();

    setTimeout();

    setDownloadAndUploadBandwidthLimit();

    return 0;
}
