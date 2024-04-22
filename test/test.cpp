#include "libcpp-http-client.hpp"
#include <nlohmann/json.hpp>
#include <gtest/gtest.h>

using namespace lklibs;
using json = nlohmann::json;

TEST(HttpGetTest, HttpGetRequestMustBeCompletedSuccessfullyInItsSimplestForm) {

    HttpRequest httpRequest("https://httpbun.com/get");

    auto response = httpRequest
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

    HttpRequest httpRequest1("https://httpbun.com/get");
    HttpRequest httpRequest2("https://httpbun.com/get");
    HttpRequest httpRequest3("https://httpbun.com/get");

    auto future1 = httpRequest1.setQueryString("param1=1&param2=test1").send();
    auto future2 = httpRequest2.setQueryString("param1=2&param2=test2").send();
    auto future3 = httpRequest3.setQueryString("param1=3&param2=test3").send();

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

    HttpRequest httpRequest("https://httpbun.com/bytes/100");

    auto response = httpRequest
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

    HttpRequest httpRequest("https://httpbun.com/not_found");

    auto response = httpRequest.send().get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpGetTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpGetRequestForAnotherError) {

    HttpRequest httpRequest("https://httpbun.com/bearer");

    auto response = httpRequest.send().get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpGetTest, HttpHeadersCanBeSentWithTheHttpGetRequest) {

    HttpRequest httpRequest("https://httpbun.com/get");

    auto response = httpRequest
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

    HttpRequest httpRequest("https://httpbun.com/post");

    auto response = httpRequest
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

    HttpRequest httpRequest1("https://httpbun.com/post");
    HttpRequest httpRequest2("https://httpbun.com/post");
    HttpRequest httpRequest3("https://httpbun.com/post");

    auto future1 = httpRequest1.setMethod(HttpMethod::POST).setPayload("param1=1&param2=test1").send();
    auto future2 = httpRequest2.setMethod(HttpMethod::POST).setPayload("param1=2&param2=test2").send();
    auto future3 = httpRequest3.setMethod(HttpMethod::POST).setPayload("param1=3&param2=test3").send();

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

    HttpRequest httpRequest("https://httpbun.com/bytes/100");

    auto response = httpRequest
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

    HttpRequest httpRequest("https://httpbun.com/not_found");

    auto response = httpRequest
            .setMethod(HttpMethod::POST)
            .send()
            .get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPostTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPostRequestForAnotherError) {

    HttpRequest httpRequest("https://httpbun.com/bearer");

    auto response = httpRequest
            .setMethod(HttpMethod::POST)
            .send()
            .get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPostTest, HttpHeadersCanBeSentWithTheHttpPostRequest) {

    HttpRequest httpRequest("https://httpbun.com/post");

    auto response = httpRequest
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

    HttpRequest httpRequest("https://httpbun.com/put");

    auto response = httpRequest
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

    HttpRequest httpRequest1("https://httpbun.com/put");
    HttpRequest httpRequest2("https://httpbun.com/put");
    HttpRequest httpRequest3("https://httpbun.com/put");

    auto future1 = httpRequest1.setMethod(HttpMethod::PUT).setPayload("param1=1&param2=test1").send();
    auto future2 = httpRequest2.setMethod(HttpMethod::PUT).setPayload("param1=2&param2=test2").send();
    auto future3 = httpRequest3.setMethod(HttpMethod::PUT).setPayload("param1=3&param2=test3").send();

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

    HttpRequest httpRequest("https://httpbun.com/bytes/100");

    auto response = httpRequest
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

    HttpRequest httpRequest("https://httpbun.com/not_found");

    auto response = httpRequest
            .setMethod(HttpMethod::PUT)
            .send()
            .get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPutTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPutRequestForAnotherError) {

    HttpRequest httpRequest("https://httpbun.com/bearer");

    auto response = httpRequest
            .setMethod(HttpMethod::PUT)
            .send()
            .get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPutTest, HttpHeadersCanBeSentWithTheHttpPutRequest) {

    HttpRequest httpRequest("https://httpbun.com/put");

    auto response = httpRequest
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

    HttpRequest httpRequest("https://httpbun.com/delete");

    auto response = httpRequest
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

    HttpRequest httpRequest1("https://httpbun.com/delete");
    HttpRequest httpRequest2("https://httpbun.com/delete");
    HttpRequest httpRequest3("https://httpbun.com/delete");

    auto future1 = httpRequest1.setMethod(HttpMethod::DELETE_).setPayload("param1=1&param2=test1").send();
    auto future2 = httpRequest2.setMethod(HttpMethod::DELETE_).setPayload("param1=2&param2=test2").send();
    auto future3 = httpRequest3.setMethod(HttpMethod::DELETE_).setPayload("param1=3&param2=test3").send();

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

    HttpRequest httpRequest("https://httpbun.com/bytes/100");

    auto response = httpRequest
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

    HttpRequest httpRequest("https://httpbun.com/not_found");

    auto response = httpRequest
            .setMethod(HttpMethod::DELETE_)
            .send()
            .get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpDeleteTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpDeleteRequestForAnotherError) {

    HttpRequest httpRequest("https://httpbun.com/bearer");

    auto response = httpRequest
            .setMethod(HttpMethod::DELETE_)
            .send()
            .get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpDeleteTest, HttpHeadersCanBeSentWithTheHttpDeleteRequest) {

    HttpRequest httpRequest("https://httpbun.com/delete");

    auto response = httpRequest
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

    HttpRequest httpRequest("https://httpbun.com/patch");

    auto response = httpRequest
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

    HttpRequest httpRequest1("https://httpbun.com/patch");
    HttpRequest httpRequest2("https://httpbun.com/patch");
    HttpRequest httpRequest3("https://httpbun.com/patch");

    auto future1 = httpRequest1.setMethod(HttpMethod::PATCH).setQueryString("param1=1&param2=test1").send();
    auto future2 = httpRequest2.setMethod(HttpMethod::PATCH).setQueryString("param1=2&param2=test2").send();
    auto future3 = httpRequest3.setMethod(HttpMethod::PATCH).setQueryString("param1=3&param2=test3").send();

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

    HttpRequest httpRequest("https://httpbun.com/bytes/100");

    auto response = httpRequest
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

    HttpRequest httpRequest("https://httpbun.com/not_found");

    auto response = httpRequest.setMethod(HttpMethod::PATCH).send().get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 404) << "HTTP Status Code is not 404";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPatchTest, AnErrorMessageShouldBeReturnedInResponseToAnHttpPatchRequestForAnotherError) {

    HttpRequest httpRequest("https://httpbun.com/bearer");

    auto response = httpRequest.setMethod(HttpMethod::PATCH).send().get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 401) << "HTTP Status Code is not 401";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(HttpPatchTest, HttpHeadersCanBeSentWithTheHttpPatchRequest) {

    HttpRequest httpRequest("https://httpbun.com/patch");

    auto response = httpRequest
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

    HttpRequest httpRequest("https://self-signed-cert.httpbun.com");

    auto response = httpRequest.send().get();

    ASSERT_FALSE(response.succeed) << "HTTP Request failed";
    ASSERT_EQ(response.statusCode, 0) << "HTTP Status Code is not 0";
    ASSERT_FALSE(response.errorMessage.empty()) << "HTTP Error Message is empty";
}

TEST(InvalidSSLTest, HttpGetRequestMustBeCompletedSuccessfullyForAnInvalidSslIfIgnoreSslErrorsFieldSetTrue) {

    HttpRequest httpRequest("https://self-signed-cert.httpbun.com");

    auto response = httpRequest.ignoreSslErrors().send().get();

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
