#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

void simpleGet() {

    // The simplest but slowest method if multiple calls will be made
    auto response = HttpClient::getRequest("https://httpbun.com/get?param1=7&param2=test").get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void nonBlockingGet() {

    // All requests are made one after the other without waiting for a response
    auto future1 = HttpClient::getRequest("https://httpbun.com/get?param1=1&param2=test1");
    auto future2 = HttpClient::getRequest("https://httpbun.com/get?param1=2&param2=test2");
    auto future3 = HttpClient::getRequest("https://httpbun.com/get?param1=3&param2=test3");

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

void receiveBinaryData() {

    // If you need to retrieve binary data such as an image, just pass the "returnAsBinary" parameter as true
    auto response = HttpClient::getRequest("https://httpbun.com/bytes/100", true).get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;

    // In this case, you can get the data via binaryData instead of textData
    std::cout << "Data Size: " << response.binaryData.size() << std::endl;
}

void receiveError() {

    // This is an exception free library. If an error occurs, no exception is thrown
    auto response = HttpClient::getRequest("https://httpbun.com/not_found").get();

    // Instead, the succeed field of the response object is set to false
    std::cout << "Succeed: " << response.succeed << std::endl;

    // And the http status code is set to the statusCode field
    std::cout << "Http Status Code: " << response.statusCode << std::endl;

    // Also if any error message is available, it is set to the errorMessage field
    std::cout << "Error Message: " << response.errorMessage << std::endl;
}

void sendingHttpHeaders() {

    // You can send custom headers in a string/string map
    auto headers = std::map<std::string, std::string>();

    headers["Custom-Header1"] = "value1";
    headers["Custom-Header2"] = "value2";

    auto response = HttpClient::getRequest("https://httpbun.com/get?param1=7&param2=test", headers).get();

    std::cout << "Succeed: " << response.succeed << std::endl;
}

void simplePostWithFormData() {

    // You can send a POST request with form data in the payload
    std::string payload = "param1=7&param2=test";

    auto response = HttpClient::postRequest("https://httpbun.com/post", payload).get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void simplePostWithJSONData() {

    std::string payload = R"({"param1": 7, "param2": "test"})";

    // You need to send the "Content-Type" as "application/json" in the HTTP Header, if you need to send json data in the payload
    auto headers = std::map<std::string, std::string>();

    headers["Content-Type"] = "application/json";

    auto response = HttpClient::postRequest("https://httpbun.com/post", payload, headers).get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void simplePutWithFormData() {

    // You can send a PUT request with form data in the payload just like POST
    std::string payload = "param1=7&param2=test";

    auto response = HttpClient::putRequest("https://httpbun.com/put", payload).get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void simpleDeleteWithFormData() {

    // You can send a DELETE request with form data in the payload just like POST
    std::string payload = "param1=7&param2=test";

    auto response = HttpClient::deleteRequest("https://httpbun.com/delete", payload).get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}

void simplePatch() {

    // You can send a PATCH request with QueryString just like GET
    auto response = HttpClient::patchRequest("https://httpbun.com/patch?param1=7&param2=test").get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;
}


int main() {

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

    return 0;
}