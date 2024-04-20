#include "libcpp-http-client.hpp"
#include <nlohmann/json.hpp>
#include <gtest/gtest.h>

using namespace lklibs;
using json = nlohmann::json;

TEST(HttpGetTest, HttpGetRequestMustBeCompletedSuccessfullyInItsSimplestForm) {

    HttpClient httpClient;

    auto response = httpClient.getRequest("https://httpbun.com/get?param1=7&param2=test").get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data = json::parse(response.textData);

    ASSERT_EQ(data["method"], "GET") << "HTTP Method is invalid";
    ASSERT_EQ(data["args"]["param1"], "7") << "Querystring is invalid";
    ASSERT_EQ(data["args"]["param2"], "test") << "Querystring is invalid";
}

TEST(HttpGetTest, MultipleHttpGetRequestMustBeCompletedSuccessfullyInNonBlockingForm) {

    HttpClient httpClient;

    auto future1 = httpClient.getRequest("https://httpbun.com/get?param1=1&param2=test1");
    auto future2 = httpClient.getRequest("https://httpbun.com/get?param1=2&param2=test2");
    auto future3 = httpClient.getRequest("https://httpbun.com/get?param1=3&param2=test3");

    auto response1 = future1.get();
    auto response2 = future2.get();
    auto response3 = future3.get();

    ASSERT_TRUE(response1.succeed) << "HTTP Request failed";
    ASSERT_EQ(response1.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response1.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response1.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response1.errorMessage.empty()) << "HTTP Error Message is not empty";

    ASSERT_TRUE(response2.succeed) << "HTTP Request failed";
    ASSERT_EQ(response2.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response2.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response2.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response2.errorMessage.empty()) << "HTTP Error Message is not empty";

    ASSERT_TRUE(response3.succeed) << "HTTP Request failed";
    ASSERT_EQ(response3.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response3.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response3.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response3.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data1 = json::parse(response1.textData);
    auto data2 = json::parse(response2.textData);
    auto data3 = json::parse(response3.textData);

    ASSERT_EQ(data1["method"], "GET") << "HTTP Method is invalid";
    ASSERT_EQ(data1["args"]["param1"], "1") << "Querystring is invalid";
    ASSERT_EQ(data1["args"]["param2"], "test1") << "Querystring is invalid";

    ASSERT_EQ(data2["method"], "GET") << "HTTP Method is invalid";
    ASSERT_EQ(data2["args"]["param1"], "2") << "Querystring is invalid";
    ASSERT_EQ(data2["args"]["param2"], "test2") << "Querystring is invalid";

    ASSERT_EQ(data3["method"], "GET") << "HTTP Method is invalid";
    ASSERT_EQ(data3["args"]["param1"], "3") << "Querystring is invalid";
    ASSERT_EQ(data3["args"]["param2"], "test3") << "Querystring is invalid";
}

TEST(HttpGetTest, ResponseOfAnHttpGetRequestCanBeReceivedInBinaryFormat) {

    HttpClient httpClient;

    auto response = httpClient.getRequest("https://httpbun.com/bytes/100", true).get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_TRUE(response.textData.empty()) << "Text data is not empty";
    ASSERT_EQ(response.binaryData.size(), 100) << "Binary data length is invalid";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";
}

TEST(HttpGetTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpGetRequestMadeToAnInvalidAddress) {

    HttpClient httpClient;

    auto response = httpClient.getRequest("https://httpbun.com/not_found").get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpGetTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpGetRequestForAnotherError) {

    HttpClient httpClient;

    auto response = httpClient.getRequest("https://httpbun.com/bearer").get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpGetTest, HttpHeadersCanBeSentWithTheHttpGetRequest) {

    auto headers = std::map<std::string, std::string>();

    headers["Custom-Header1"] = "value1";
    headers["Custom-Header2"] = "value2";

    HttpClient httpClient;

    auto response = httpClient.getRequest("https://httpbun.com/get?param1=7&param2=test", headers).get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data = json::parse(response.textData);

    ASSERT_EQ(data["method"], "GET") << "HTTP Method is invalid";
    ASSERT_EQ(data["args"]["param1"], "7") << "Querystring is invalid";
    ASSERT_EQ(data["args"]["param2"], "test") << "Querystring is invalid";

    ASSERT_EQ(data["headers"]["Custom-Header1"], "value1") << "Custom-Header1 is invalid";
    ASSERT_EQ(data["headers"]["Custom-Header2"], "value2") << "Custom-Header2 is invalid";
}

TEST(HttpPostTest, HttpPostRequestMustBeCompletedSuccessfullyInItsSimplestForm) {

    std::string payload = "param1=7&param2=test";

    HttpClient httpClient;

    auto response = httpClient.postRequest("https://httpbun.com/post", payload).get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data = json::parse(response.textData);

    ASSERT_EQ(data["method"], "POST") << "HTTP Method is invalid";
    ASSERT_EQ(data["form"]["param1"], "7") << "Payload is invalid";
    ASSERT_EQ(data["form"]["param2"], "test") << "Payload is invalid";
}

TEST(HttpPostTest, MultipleHttpPostRequestMustBeCompletedSuccessfullyInNonBlockingForm) {

    std::string payload1 = "param1=1&param2=test1";
    std::string payload2 = "param1=2&param2=test2";
    std::string payload3 = "param1=3&param2=test3";

    HttpClient httpClient;

    auto future1 = httpClient.postRequest("https://httpbun.com/post", payload1);
    auto future2 = httpClient.postRequest("https://httpbun.com/post", payload2);
    auto future3 = httpClient.postRequest("https://httpbun.com/post", payload3);

    auto response1 = future1.get();
    auto response2 = future2.get();
    auto response3 = future3.get();

    ASSERT_TRUE(response1.succeed) << "HTTP Request failed";
    ASSERT_EQ(response1.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response1.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response1.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response1.errorMessage.empty()) << "HTTP Error Message is not empty";

    ASSERT_TRUE(response2.succeed) << "HTTP Request failed";
    ASSERT_EQ(response2.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response2.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response2.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response2.errorMessage.empty()) << "HTTP Error Message is not empty";

    ASSERT_TRUE(response3.succeed) << "HTTP Request failed";
    ASSERT_EQ(response3.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response3.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response3.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response3.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data1 = json::parse(response1.textData);
    auto data2 = json::parse(response2.textData);
    auto data3 = json::parse(response3.textData);

    ASSERT_EQ(data1["method"], "POST") << "HTTP Method is invalid";
    ASSERT_EQ(data1["form"]["param1"], "1") << "Payload is invalid";
    ASSERT_EQ(data1["form"]["param2"], "test1") << "Payload is invalid";

    ASSERT_EQ(data2["method"], "POST") << "HTTP Method is invalid";
    ASSERT_EQ(data2["form"]["param1"], "2") << "Payload is invalid";
    ASSERT_EQ(data2["form"]["param2"], "test2") << "Payload is invalid";

    ASSERT_EQ(data3["method"], "POST") << "HTTP Method is invalid";
    ASSERT_EQ(data3["form"]["param1"], "3") << "Payload is invalid";
    ASSERT_EQ(data3["form"]["param2"], "test3") << "Payload is invalid";
}

TEST(HttpPostTest, ResponseOfAnHttpPostRequestCanBeReceivedInBinaryFormat) {

    HttpClient httpClient;

    auto response = httpClient.postRequest("https://httpbun.com/bytes/100", true).get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_TRUE(response.textData.empty()) << "Text data is not empty";
    ASSERT_EQ(response.binaryData.size(), 100) << "Binary data length is invalid";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";
}

TEST(HttpPostTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPostRequestMadeToAnInvalidAddress) {

    HttpClient httpClient;

    auto response = httpClient.postRequest("https://httpbun.com/not_found").get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPostTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPostRequestForAnotherError) {

    HttpClient httpClient;

    auto response = httpClient.postRequest("https://httpbun.com/bearer").get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPostTest, HttpHeadersCanBeSentWithTheHttpPostRequest) {

    std::string payload = R"({"param1": 7, "param2": "test"})";

    auto headers = std::map<std::string, std::string>();

    headers["Content-Type"] = "application/json";
    headers["Custom-Header1"] = "value1";
    headers["Custom-Header2"] = "value2";

    HttpClient httpClient;

    auto response = httpClient.postRequest("https://httpbun.com/post", payload, headers).get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data = json::parse(response.textData);

    ASSERT_EQ(data["method"], "POST") << "HTTP Method is invalid";
    ASSERT_EQ(data["json"]["param1"], 7) << "Payload is invalid";
    ASSERT_EQ(data["json"]["param2"], "test") << "Payload is invalid";

    ASSERT_EQ(data["headers"]["Content-Type"], "application/json") << "Content-Type is invalid";
    ASSERT_EQ(data["headers"]["Custom-Header1"], "value1") << "Custom-Header1 is invalid";
    ASSERT_EQ(data["headers"]["Custom-Header2"], "value2") << "Custom-Header2 is invalid";
}

TEST(HttpPutTest, HttpPutRequestMustBeCompletedSuccessfullyInItsSimplestForm) {

    std::string payload = "param1=7&param2=test";

    HttpClient httpClient;

    auto response = httpClient.putRequest("https://httpbun.com/put", payload).get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data = json::parse(response.textData);

    ASSERT_EQ(data["method"], "PUT") << "HTTP Method is invalid";
    ASSERT_EQ(data["form"]["param1"], "7") << "Payload is invalid";
    ASSERT_EQ(data["form"]["param2"], "test") << "Payload is invalid";
}

TEST(HttpPutTest, MultipleHttpPutRequestMustBeCompletedSuccessfullyInNonBlockingForm) {

    std::string payload1 = "param1=1&param2=test1";
    std::string payload2 = "param1=2&param2=test2";
    std::string payload3 = "param1=3&param2=test3";

    HttpClient httpClient;

    auto future1 = httpClient.putRequest("https://httpbun.com/put", payload1);
    auto future2 = httpClient.putRequest("https://httpbun.com/put", payload2);
    auto future3 = httpClient.putRequest("https://httpbun.com/put", payload3);

    auto response1 = future1.get();
    auto response2 = future2.get();
    auto response3 = future3.get();

    ASSERT_TRUE(response1.succeed) << "HTTP Request failed";
    ASSERT_EQ(response1.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response1.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response1.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response1.errorMessage.empty()) << "HTTP Error Message is not empty";

    ASSERT_TRUE(response2.succeed) << "HTTP Request failed";
    ASSERT_EQ(response2.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response2.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response2.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response2.errorMessage.empty()) << "HTTP Error Message is not empty";

    ASSERT_TRUE(response3.succeed) << "HTTP Request failed";
    ASSERT_EQ(response3.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response3.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response3.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response3.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data1 = json::parse(response1.textData);
    auto data2 = json::parse(response2.textData);
    auto data3 = json::parse(response3.textData);

    ASSERT_EQ(data1["method"], "PUT") << "HTTP Method is invalid";
    ASSERT_EQ(data1["form"]["param1"], "1") << "Payload is invalid";
    ASSERT_EQ(data1["form"]["param2"], "test1") << "Payload is invalid";

    ASSERT_EQ(data2["method"], "PUT") << "HTTP Method is invalid";
    ASSERT_EQ(data2["form"]["param1"], "2") << "Payload is invalid";
    ASSERT_EQ(data2["form"]["param2"], "test2") << "Payload is invalid";

    ASSERT_EQ(data3["method"], "PUT") << "HTTP Method is invalid";
    ASSERT_EQ(data3["form"]["param1"], "3") << "Payload is invalid";
    ASSERT_EQ(data3["form"]["param2"], "test3") << "Payload is invalid";
}

TEST(HttpPutTest, ResponseOfAnHttpPutRequestCanBeReceivedInBinaryFormat) {

    HttpClient httpClient;

    auto response = httpClient.putRequest("https://httpbun.com/bytes/100", true).get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_TRUE(response.textData.empty()) << "Text data is not empty";
    ASSERT_EQ(response.binaryData.size(), 100) << "Binary data length is invalid";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";
}

TEST(HttpPutTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPutRequestMadeToAnInvalidAddress) {

    HttpClient httpClient;

    auto response = httpClient.putRequest("https://httpbun.com/not_found").get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPutTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPutRequestForAnotherError) {

    HttpClient httpClient;

    auto response = httpClient.putRequest("https://httpbun.com/bearer").get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPutTest, HttpHeadersCanBeSentWithTheHttpPutRequest) {

    std::string payload = R"({"param1": 7, "param2": "test"})";

    auto headers = std::map<std::string, std::string>();

    headers["Content-Type"] = "application/json";
    headers["Custom-Header1"] = "value1";
    headers["Custom-Header2"] = "value2";

    HttpClient httpClient;

    auto response = httpClient.putRequest("https://httpbun.com/put", payload, headers).get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data = json::parse(response.textData);

    ASSERT_EQ(data["method"], "PUT") << "HTTP Method is invalid";
    ASSERT_EQ(data["json"]["param1"], 7) << "Payload is invalid";
    ASSERT_EQ(data["json"]["param2"], "test") << "Payload is invalid";

    ASSERT_EQ(data["headers"]["Content-Type"], "application/json") << "Content-Type is invalid";
    ASSERT_EQ(data["headers"]["Custom-Header1"], "value1") << "Custom-Header1 is invalid";
    ASSERT_EQ(data["headers"]["Custom-Header2"], "value2") << "Custom-Header2 is invalid";
}

TEST(HttpDeleteTest, HttpDeleteRequestMustBeCompletedSuccessfullyInItsSimplestForm) {

    std::string payload = "param1=7&param2=test";

    HttpClient httpClient;

    auto response = httpClient.deleteRequest("https://httpbun.com/delete", payload).get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data = json::parse(response.textData);

    ASSERT_EQ(data["method"], "DELETE") << "HTTP Method is invalid";
    ASSERT_EQ(data["form"]["param1"], "7") << "Payload is invalid";
    ASSERT_EQ(data["form"]["param2"], "test") << "Payload is invalid";
}

TEST(HttpDeleteTest, MultipleHttpDeleteRequestMustBeCompletedSuccessfullyInNonBlockingForm) {

    std::string payload1 = "param1=1&param2=test1";
    std::string payload2 = "param1=2&param2=test2";
    std::string payload3 = "param1=3&param2=test3";

    HttpClient httpClient;

    auto future1 = httpClient.deleteRequest("https://httpbun.com/delete", payload1);
    auto future2 = httpClient.deleteRequest("https://httpbun.com/delete", payload2);
    auto future3 = httpClient.deleteRequest("https://httpbun.com/delete", payload3);

    auto response1 = future1.get();
    auto response2 = future2.get();
    auto response3 = future3.get();

    ASSERT_TRUE(response1.succeed) << "HTTP Request failed";
    ASSERT_EQ(response1.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response1.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response1.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response1.errorMessage.empty()) << "HTTP Error Message is not empty";

    ASSERT_TRUE(response2.succeed) << "HTTP Request failed";
    ASSERT_EQ(response2.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response2.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response2.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response2.errorMessage.empty()) << "HTTP Error Message is not empty";

    ASSERT_TRUE(response3.succeed) << "HTTP Request failed";
    ASSERT_EQ(response3.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response3.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response3.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response3.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data1 = json::parse(response1.textData);
    auto data2 = json::parse(response2.textData);
    auto data3 = json::parse(response3.textData);

    ASSERT_EQ(data1["method"], "DELETE") << "HTTP Method is invalid";
    ASSERT_EQ(data1["form"]["param1"], "1") << "Payload is invalid";
    ASSERT_EQ(data1["form"]["param2"], "test1") << "Payload is invalid";

    ASSERT_EQ(data2["method"], "DELETE") << "HTTP Method is invalid";
    ASSERT_EQ(data2["form"]["param1"], "2") << "Payload is invalid";
    ASSERT_EQ(data2["form"]["param2"], "test2") << "Payload is invalid";

    ASSERT_EQ(data3["method"], "DELETE") << "HTTP Method is invalid";
    ASSERT_EQ(data3["form"]["param1"], "3") << "Payload is invalid";
    ASSERT_EQ(data3["form"]["param2"], "test3") << "Payload is invalid";
}

TEST(HttpDeleteTest, ResponseOfAnHttpDeleteRequestCanBeReceivedInBinaryFormat) {

    HttpClient httpClient;

    auto response = httpClient.deleteRequest("https://httpbun.com/bytes/100", true).get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_TRUE(response.textData.empty()) << "Text data is not empty";
    ASSERT_EQ(response.binaryData.size(), 100) << "Binary data length is invalid";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";
}

TEST(HttpDeleteTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpDeleteRequestMadeToAnInvalidAddress) {

    HttpClient httpClient;

    auto response = httpClient.deleteRequest("https://httpbun.com/not_found").get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpDeleteTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpDeleteRequestForAnotherError) {

    HttpClient httpClient;

    auto response = httpClient.deleteRequest("https://httpbun.com/bearer").get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpDeleteTest, HttpHeadersCanBeSentWithTheHttpDeleteRequest) {

    std::string payload = R"({"param1": 7, "param2": "test"})";

    auto headers = std::map<std::string, std::string>();

    headers["Content-Type"] = "application/json";
    headers["Custom-Header1"] = "value1";
    headers["Custom-Header2"] = "value2";

    HttpClient httpClient;

    auto response = httpClient.deleteRequest("https://httpbun.com/delete", payload, headers).get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data = json::parse(response.textData);

    ASSERT_EQ(data["method"], "DELETE") << "HTTP Method is invalid";
    ASSERT_EQ(data["json"]["param1"], 7) << "Payload is invalid";
    ASSERT_EQ(data["json"]["param2"], "test") << "Payload is invalid";

    ASSERT_EQ(data["headers"]["Content-Type"], "application/json") << "Content-Type is invalid";
    ASSERT_EQ(data["headers"]["Custom-Header1"], "value1") << "Custom-Header1 is invalid";
    ASSERT_EQ(data["headers"]["Custom-Header2"], "value2") << "Custom-Header2 is invalid";
}

TEST(HttpPatchTest, HttpPatchRequestMustBeCompletedSuccessfullyInItsSimplestForm) {

    HttpClient httpClient;

    auto response = httpClient.patchRequest("https://httpbun.com/patch?param1=7&param2=test").get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data = json::parse(response.textData);

    ASSERT_EQ(data["method"], "PATCH") << "HTTP Method is invalid";
    ASSERT_EQ(data["args"]["param1"], "7") << "Querystring is invalid";
    ASSERT_EQ(data["args"]["param2"], "test") << "Querystring is invalid";
}

TEST(HttpPatchTest, MultipleHttpPatchRequestMustBeCompletedSuccessfullyInNonBlockingForm) {

    HttpClient httpClient;

    auto future1 = httpClient.patchRequest("https://httpbun.com/patch?param1=1&param2=test1");
    auto future2 = httpClient.patchRequest("https://httpbun.com/patch?param1=2&param2=test2");
    auto future3 = httpClient.patchRequest("https://httpbun.com/patch?param1=3&param2=test3");

    auto response1 = future1.get();
    auto response2 = future2.get();
    auto response3 = future3.get();

    ASSERT_TRUE(response1.succeed) << "HTTP Request failed";
    ASSERT_EQ(response1.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response1.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response1.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response1.errorMessage.empty()) << "HTTP Error Message is not empty";

    ASSERT_TRUE(response2.succeed) << "HTTP Request failed";
    ASSERT_EQ(response2.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response2.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response2.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response2.errorMessage.empty()) << "HTTP Error Message is not empty";

    ASSERT_TRUE(response3.succeed) << "HTTP Request failed";
    ASSERT_EQ(response3.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response3.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response3.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response3.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data1 = json::parse(response1.textData);
    auto data2 = json::parse(response2.textData);
    auto data3 = json::parse(response3.textData);

    ASSERT_EQ(data1["method"], "PATCH") << "HTTP Method is invalid";
    ASSERT_EQ(data1["args"]["param1"], "1") << "Querystring is invalid";
    ASSERT_EQ(data1["args"]["param2"], "test1") << "Querystring is invalid";

    ASSERT_EQ(data2["method"], "PATCH") << "HTTP Method is invalid";
    ASSERT_EQ(data2["args"]["param1"], "2") << "Querystring is invalid";
    ASSERT_EQ(data2["args"]["param2"], "test2") << "Querystring is invalid";

    ASSERT_EQ(data3["method"], "PATCH") << "HTTP Method is invalid";
    ASSERT_EQ(data3["args"]["param1"], "3") << "Querystring is invalid";
    ASSERT_EQ(data3["args"]["param2"], "test3") << "Querystring is invalid";
}

TEST(HttpPatchTest, ResponseOfAnHttpPatchRequestCanBeReceivedInBinaryFormat) {

    HttpClient httpClient;

    auto response = httpClient.patchRequest("https://httpbun.com/bytes/100", true).get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_TRUE(response.textData.empty()) << "Text data is not empty";
    ASSERT_EQ(response.binaryData.size(), 100) << "Binary data length is invalid";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";
}

TEST(HttpPatchTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPatchRequestMadeToAnInvalidAddress) {

    HttpClient httpClient;

    auto response = httpClient.patchRequest("https://httpbun.com/not_found").get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPatchTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPatchRequestForAnotherError) {

    HttpClient httpClient;

    auto response = httpClient.patchRequest("https://httpbun.com/bearer").get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPatchTest, HttpHeadersCanBeSentWithTheHttpPatchRequest) {

    auto headers = std::map<std::string, std::string>();

    headers["Content-Type"] = "application/json";
    headers["Custom-Header1"] = "value1";
    headers["Custom-Header2"] = "value2";

    HttpClient httpClient;

    auto response = httpClient.patchRequest("https://httpbun.com/patch?param1=7&param2=test", headers).get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data = json::parse(response.textData);

    ASSERT_EQ(data["method"], "PATCH") << "HTTP Method is invalid";
    ASSERT_EQ(data["args"]["param1"], "7") << "Querystring is invalid";
    ASSERT_EQ(data["args"]["param2"], "test") << "Querystring is invalid";

    ASSERT_EQ(data["headers"]["Content-Type"], "application/json") << "Content-Type is invalid";
    ASSERT_EQ(data["headers"]["Custom-Header1"], "value1") << "Custom-Header1 is invalid";
    ASSERT_EQ(data["headers"]["Custom-Header2"], "value2") << "Custom-Header2 is invalid";
}

int main(int argc, char** argv) {

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
