#include "libcpp-http-client.hpp"
#include <nlohmann/json.hpp>
#include <gtest/gtest.h>

using namespace lklibs;
using json = nlohmann::json;

TEST(HttpGetTest, HttpGetRequestMustBeCompletedSuccessfullyInItsSimplestForm) {

    HttpClient httpClient("https://httpbun.com/get");

    auto response = httpClient
            .setQueryString("param1=7&param2=test")
            .send()
            .get();

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

    HttpClient httpClient1("https://httpbun.com/get");
    HttpClient httpClient2("https://httpbun.com/get");
    HttpClient httpClient3("https://httpbun.com/get");

    auto future1 = httpClient1.setQueryString("param1=1&param2=test1").send();
    auto future2 = httpClient2.setQueryString("param1=2&param2=test2").send();
    auto future3 = httpClient3.setQueryString("param1=3&param2=test3").send();

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

    HttpClient httpClient("https://httpbun.com/bytes/100");

    auto response = httpClient
            .returnAsBinary()
            .send()
            .get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_TRUE(response.textData.empty()) << "Text data is not empty";
    ASSERT_EQ(response.binaryData.size(), 100) << "Binary data length is invalid";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";
}

TEST(HttpGetTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpGetRequestMadeToAnInvalidAddress) {

    HttpClient httpClient("https://httpbun.com/not_found");

    auto response = httpClient.send().get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpGetTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpGetRequestForAnotherError) {

    HttpClient httpClient("https://httpbun.com/bearer");

    auto response = httpClient.send().get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpGetTest, HttpHeadersCanBeSentWithTheHttpGetRequest) {

    HttpClient httpClient("https://httpbun.com/get");

    auto response = httpClient
            .setQueryString("param1=7&param2=test")
            .addHeader("Custom-Header1", "value1")
            .addHeader("Custom-Header2", "value2")
            .send()
            .get();

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

    HttpClient httpClient("https://httpbun.com/post");

    auto response = httpClient
            .setMethod(HttpMethod::POST)
            .setPayload("param1=7&param2=test")
            .send()
            .get();

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

    HttpClient httpClient1("https://httpbun.com/post");
    HttpClient httpClient2("https://httpbun.com/post");
    HttpClient httpClient3("https://httpbun.com/post");

    auto future1 = httpClient1.setMethod(HttpMethod::POST).setPayload("param1=1&param2=test1").send();
    auto future2 = httpClient2.setMethod(HttpMethod::POST).setPayload("param1=2&param2=test2").send();
    auto future3 = httpClient3.setMethod(HttpMethod::POST).setPayload("param1=3&param2=test3").send();

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

    HttpClient httpClient("https://httpbun.com/bytes/100");

    auto response = httpClient
            .setMethod(HttpMethod::POST)
            .returnAsBinary()
            .send()
            .get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_TRUE(response.textData.empty()) << "Text data is not empty";
    ASSERT_EQ(response.binaryData.size(), 100) << "Binary data length is invalid";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";
}

TEST(HttpPostTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPostRequestMadeToAnInvalidAddress) {

    HttpClient httpClient("https://httpbun.com/not_found");

    auto response = httpClient
            .setMethod(HttpMethod::POST)
            .send()
            .get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPostTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPostRequestForAnotherError) {

    HttpClient httpClient("https://httpbun.com/bearer");

    auto response = httpClient
            .setMethod(HttpMethod::POST)
            .send()
            .get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPostTest, HttpHeadersCanBeSentWithTheHttpPostRequest) {

    HttpClient httpClient("https://httpbun.com/post");

    auto response = httpClient
            .setMethod(HttpMethod::POST)
            .setPayload(R"({"param1": 7, "param2": "test"})")
            .addHeader("Content-Type", "application/json")
            .addHeader("Custom-Header1", "value1")
            .addHeader("Custom-Header2", "value2")
            .send()
            .get();

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

    HttpClient httpClient("https://httpbun.com/put");

    auto response = httpClient
            .setMethod(HttpMethod::PUT)
            .setPayload("param1=7&param2=test")
            .send()
            .get();

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

    HttpClient httpClient1("https://httpbun.com/put");
    HttpClient httpClient2("https://httpbun.com/put");
    HttpClient httpClient3("https://httpbun.com/put");

    auto future1 = httpClient1.setMethod(HttpMethod::PUT).setPayload("param1=1&param2=test1").send();
    auto future2 = httpClient2.setMethod(HttpMethod::PUT).setPayload("param1=2&param2=test2").send();
    auto future3 = httpClient3.setMethod(HttpMethod::PUT).setPayload("param1=3&param2=test3").send();

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

    HttpClient httpClient("https://httpbun.com/bytes/100");

    auto response = httpClient
            .setMethod(HttpMethod::PUT)
            .returnAsBinary()
            .send()
            .get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_TRUE(response.textData.empty()) << "Text data is not empty";
    ASSERT_EQ(response.binaryData.size(), 100) << "Binary data length is invalid";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";
}

TEST(HttpPutTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPutRequestMadeToAnInvalidAddress) {

    HttpClient httpClient("https://httpbun.com/not_found");

    auto response = httpClient
            .setMethod(HttpMethod::PUT)
            .send()
            .get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPutTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPutRequestForAnotherError) {

    HttpClient httpClient("https://httpbun.com/bearer");

    auto response = httpClient
            .setMethod(HttpMethod::PUT)
            .send()
            .get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPutTest, HttpHeadersCanBeSentWithTheHttpPutRequest) {

    HttpClient httpClient("https://httpbun.com/put");

    auto response = httpClient
            .setMethod(HttpMethod::PUT)
            .setPayload(R"({"param1": 7, "param2": "test"})")
            .addHeader("Content-Type", "application/json")
            .addHeader("Custom-Header1", "value1")
            .addHeader("Custom-Header2", "value2")
            .send()
            .get();

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

    HttpClient httpClient("https://httpbun.com/delete");

    auto response = httpClient
            .setMethod(HttpMethod::DELETE_)
            .setPayload("param1=7&param2=test")
            .send()
            .get();

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

    HttpClient httpClient1("https://httpbun.com/delete");
    HttpClient httpClient2("https://httpbun.com/delete");
    HttpClient httpClient3("https://httpbun.com/delete");

    auto future1 = httpClient1.setMethod(HttpMethod::DELETE_).setPayload("param1=1&param2=test1").send();
    auto future2 = httpClient2.setMethod(HttpMethod::DELETE_).setPayload("param1=2&param2=test2").send();
    auto future3 = httpClient3.setMethod(HttpMethod::DELETE_).setPayload("param1=3&param2=test3").send();

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

    HttpClient httpClient("https://httpbun.com/bytes/100");

    auto response = httpClient
            .setMethod(HttpMethod::DELETE_)
            .returnAsBinary()
            .send()
            .get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_TRUE(response.textData.empty()) << "Text data is not empty";
    ASSERT_EQ(response.binaryData.size(), 100) << "Binary data length is invalid";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";
}

TEST(HttpDeleteTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpDeleteRequestMadeToAnInvalidAddress) {

    HttpClient httpClient("https://httpbun.com/not_found");

    auto response = httpClient
            .setMethod(HttpMethod::DELETE_)
            .send()
            .get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpDeleteTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpDeleteRequestForAnotherError) {

    HttpClient httpClient("https://httpbun.com/bearer");

    auto response = httpClient
            .setMethod(HttpMethod::DELETE_)
            .send()
            .get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpDeleteTest, HttpHeadersCanBeSentWithTheHttpDeleteRequest) {

    HttpClient httpClient("https://httpbun.com/delete");

    auto response = httpClient
            .setMethod(HttpMethod::DELETE_)
            .setPayload(R"({"param1": 7, "param2": "test"})")
            .addHeader("Content-Type", "application/json")
            .addHeader("Custom-Header1", "value1")
            .addHeader("Custom-Header2", "value2")
            .send()
            .get();

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

    HttpClient httpClient("https://httpbun.com/patch");

    auto response = httpClient
            .setMethod(HttpMethod::PATCH)
            .setQueryString("param1=7&param2=test")
            .send()
            .get();

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

    HttpClient httpClient1("https://httpbun.com/patch");
    HttpClient httpClient2("https://httpbun.com/patch");
    HttpClient httpClient3("https://httpbun.com/patch");

    auto future1 = httpClient1.setMethod(HttpMethod::PATCH).setQueryString("param1=1&param2=test1").send();
    auto future2 = httpClient2.setMethod(HttpMethod::PATCH).setQueryString("param1=2&param2=test2").send();
    auto future3 = httpClient3.setMethod(HttpMethod::PATCH).setQueryString("param1=3&param2=test3").send();

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

    HttpClient httpClient("https://httpbun.com/bytes/100");

    auto response = httpClient
            .setMethod(HttpMethod::PATCH)
            .returnAsBinary()
            .send()
            .get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_TRUE(response.textData.empty()) << "Text data is not empty";
    ASSERT_EQ(response.binaryData.size(), 100) << "Binary data length is invalid";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";
}

TEST(HttpPatchTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPatchRequestMadeToAnInvalidAddress) {

    HttpClient httpClient("https://httpbun.com/not_found");

    auto response = httpClient.setMethod(HttpMethod::PATCH).send().get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPatchTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPatchRequestForAnotherError) {

    HttpClient httpClient("https://httpbun.com/bearer");

    auto response = httpClient.setMethod(HttpMethod::PATCH).send().get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPatchTest, HttpHeadersCanBeSentWithTheHttpPatchRequest) {

    HttpClient httpClient("https://httpbun.com/patch");

    auto response = httpClient
            .setMethod(HttpMethod::PATCH)
            .setQueryString("param1=7&param2=test")
            .addHeader("Custom-Header1", "value1")
            .addHeader("Custom-Header2", "value2")
            .send()
            .get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";

    auto data = json::parse(response.textData);

    ASSERT_EQ(data["method"], "PATCH") << "HTTP Method is invalid";
    ASSERT_EQ(data["args"]["param1"], "7") << "Querystring is invalid";
    ASSERT_EQ(data["args"]["param2"], "test") << "Querystring is invalid";

    ASSERT_EQ(data["headers"]["Custom-Header1"], "value1") << "Custom-Header1 is invalid";
    ASSERT_EQ(data["headers"]["Custom-Header2"], "value2") << "Custom-Header2 is invalid";
}

TEST(InvalidSSLTest, HttpGetRequestMustReturnErrorForAnInvalidSsl) {

    HttpClient httpClient("https://self-signed-cert.httpbun.com");

    auto response = httpClient.send().get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 0) << "HTTP Status Code is not 0";
    ASSERT_EQ(response.errorMessage, "SSL peer certificate or SSH remote key was not OK") << "HTTP Error Message is invalid";
}

TEST(InvalidSSLTest, HttpGetRequestMustBeCompletedSuccessfullyForAnInvalidSslIfIgnoreSslErrorsFieldSetTrue) {

    HttpClient httpClient("https://self-signed-cert.httpbun.com");

    auto response = httpClient.ignoreSslErrors().send().get();

    ASSERT_TRUE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 200) << "HTTP Status Code is not 200";
    ASSERT_FALSE(response.textData.empty()) << "HTTP Response is empty";
    ASSERT_TRUE(response.binaryData.empty()) << "Binary data is not empty";
    ASSERT_TRUE(response.errorMessage.empty()) << "HTTP Error Message is not empty";
}

int main(int argc, char** argv) {

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
