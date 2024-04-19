# libcpp-http-client

Modern, non-blocking and exception free, header-only HTTP Client library for C++ (17+)

[![linux](https://github.com/lk-libs/libcpp-http-client/actions/workflows/linux.yml/badge.svg)](https://github.com/lk-libs/libcpp-http-client/actions/workflows/linux.yml)
[![windows](https://github.com/lk-libs/libcpp-http-client/actions/workflows/windows.yml/badge.svg)](https://github.com/lk-libs/libcpp-http-client/actions/workflows/windows.yml)


> [!TIP]
> Please read this document before using the library. I know, you don't have time but reading 
> this document will save you time. I mean just this file, it's not long at all. Trial and error 
> will cost you more time.


# Table of Contents

* [How to add it to my project](#how-to-add-it-to-my-project)
* [How to use? (Simplest way)](#how-to-use-simplest-way)
* [What does non-blocking mean?](#what-does-non-blocking-mean)
* [What does exception free mean?](#what-does-exception-free-mean)
* [What about binary data?](#what-about-binary-data)
* [Sending Custom HTTP Headers](#sending-custom-http-headers)
* [POST request with form data](#post-request-with-form-data)
* [POST request with JSON data](#post-request-with-json-data)
* [What about others? (PUT, DELETE, PATCH)](#what-about-others-put-delete-patch)
* [Semantic Versioning](#semantic-versioning)
* [Full function list](#full-function-list)
* [License](#license)
* [Contact](#contact)


## How to add it to my project?

This is a header only library. So actually, all you need is to add the libcpp-http-client.hpp file 
in src folder to your project and start using it with #include.

But this library is a kind of Curl wrapper that uses Curl under the hood. So, you need to add Curl to 
your project before to use it.

You can find usage examples in the examples folder, also find a sample CMakeLists.txt file content below.

```cmake
cmake_minimum_required(VERSION 3.14)

project(myProject)

add_executable(myProject main.cpp)

find_package(CURL CONFIG REQUIRED)

target_link_libraries(myProject PRIVATE libcpp-http-client CURL::libcurl)

```


## How to use? (Simplest way)

Below you can see the simplest use case sending QueryString parameters to an API via HTTP GET.

> [!IMPORTANT]
> Please do not use it this way, if more than one call will be made . You do not use the non-blocking
> feature in this way. Just keep reading...

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {

    // The simplest but slowest method if multiple calls will be made
    auto response = HttpClient::getRequest("https://api.myproject.com?param1=7&param2=test").get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;

    return 0;
}
```


## What does non-blocking mean?

Let's talk about this through an example. Let's say we call 5 different API methods when our 
screen opens and each one takes an average of 500 ms. If we call these methods one after another 
as follows, we will get all the answers in 2.5 seconds in total.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
    auto response1 = HttpClient::getRequest("https://api.myproject.com/foo").get();
    auto response2 = HttpClient::getRequest("https://api.myproject.com/bar").get();
    auto response3 = HttpClient::getRequest("https://api.myproject.com/baz").get();
    auto response4 = HttpClient::getRequest("https://api.myproject.com/qux").get();
    auto response5 = HttpClient::getRequest("https://api.myproject.com/quux").get();

    // Take 2.5 seconds in total

    return 0;
}
```

".get()" call at the end of each line causes us to wait until the server responds. However, 
if we make the same call as follows, all requests will be processed in parallel and the total 
time will take approximately 0.5 seconds.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
    auto future1 = HttpClient::getRequest("https://api.myproject.com/foo");
    auto future2 = HttpClient::getRequest("https://api.myproject.com/bar");
    auto future3 = HttpClient::getRequest("https://api.myproject.com/baz");
    auto future4 = HttpClient::getRequest("https://api.myproject.com/qux");
    auto future5 = HttpClient::getRequest("https://api.myproject.com/quux");
    
    auto response1 = future1.get();
    auto response2 = future2.get();
    auto response3 = future3.get();
    auto response4 = future4.get();
    auto response5 = future5.get();

    // Take 0.5 seconds in total

    return 0;
}
```

All functions in the library return a future and allow the next line to run without blocking the flow.


## What does exception free mean?

Exception Free means that no exception will be thrown for any call you make to this library.
If the URL cannot be found, there is a timeout, there is an authorization problem or another
error occurs on the server, the bool typed "succeed" field of the response is returned as false.
In addition, the HTTP Status Code value returned from the server is returned in the int typed
"statusCode" field and possibly additional error information that may be returned from the server
is returned in the string typed "errorMessage" field.

You can see an example use case below...

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
    auto response = HttpClient::getRequest("https://www.myinvalidurl.com").get();

    // Instead of throwing an exception, the succeed field of the response object is set to false
    std::cout << "Succeed: " << response.succeed << std::endl;

    // And the http status code is set to the statusCode field (404 in this case)
    std::cout << "Http Status Code: " << response.statusCode << std::endl;

    // Also if any error message is available, it is set to the errorMessage field
    std::cout << "Error Message: " << response.errorMessage << std::endl;

    return 0;
}
```


## What about binary data?

In the examples so far, we have used the **"textData"** property of the returning response object.
However, we need binary data for requests made to binary files such as images. In such cases, 
we can ensure that the returned data is returned in **"binaryData"** of type 
***"std::vector&lt;unsigned char&gt;"*** instead of **"textData"** by passing the value **"true"** 
to the **"returnAsBinary"** parameter.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
    // If you need to retrieve binary data such as an image, just pass the "returnAsBinary" parameter as true
    auto response = HttpClient::getRequest("https://api.myproject.com/image/7", true).get();
    
    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;

    // In this case, you can get the data via binaryData instead of textData
    std::cout << "Data Size: " << response.binaryData.size() << std::endl;

    return 0;
}
```


## Sending custom HTTP headers

If you need to send custom HTTP HEADERs during the request, you can send them in 
std::map<std::string, std::string> format.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
     // You can send custom headers in a string/string map
    auto headers = std::map<std::string, std::string>();

    headers["Custom-Header1"] = "value1";
    headers["Custom-Header2"] = "value2";

    auto response = HttpClient::getRequest("https://api.myproject.com?param1=7&param2=test", headers).get();

    std::cout << "Succeed: " << response.succeed << std::endl;

    return 0;
}
```


## POST request with form data

Next is submitting form data via HTTP POST. All you have to do is use **"postRequest"** instead 
of **"getRequest"**. You can pass the form data to the **"payload"** variable as seen in the sample code below.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
    // You can send a POST request with form data in the payload
    std::string payload = "param1=7&param2=test";

    auto response = HttpClient::postRequest("https://api.myproject.com", payload).get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;

    return 0;
}
```


## POST request with JSON data

Sending JSON data via HTTP POST is not much different. Just remember to send **"Content-Type"** 
as **"application/json"** via HTTP HEADER.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
    std::string payload = R"({"param1": 7, "param2": "test"})";

    // You need to send the "Content-Type" as "application/json" in the HTTP Header, if you need to send json data in the payload
    auto headers = std::map<std::string, std::string>();

    headers["Content-Type"] = "application/json";

    auto response = HttpClient::postRequest("https://api.myproject.com", payload, headers).get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;

    return 0;
}
```


## What about others? (PUT, DELETE, PATCH)

You can also find the usage of other methods in the sample code below.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
    std::string payload = "param1=7&param2=test";

    auto future1 = HttpClient::putRequest("https://api.myproject.com", payload);
    auto future2 = HttpClient::deleteRequest("https://api.myproject.com", payload);
    auto future3 = HttpClient::patchRequest("https://api.myproject.com?param1=7&param2=test");

    auto response1 = future1.get();
    auto response2 = future2.get();
    auto response3 = future3.get();

    return 0;
}
```


## Semantic Versioning

Versioning of the library is done using conventional semantic versioning. Accordingly, 
in the versioning made as **MAJOR.MINOR.PATCH**;

**PATCH:** Includes possible Bug&Fixes and improvements. You definitely want to get this.

**MINOR:** Additional functionality added via backwards compatibility. You probably want to 
get this, it doesn't hurt.

**MAJOR:** Additional functionality that breaks backwards compatibility. You'll need to know 
what's changed before you get it, and you'll probably have to make changes to your own code. 
If I publish something like this, I will definitely add the changes required for migration 
section to the documentation.


## Full function list

You can find the complete list of functions in the library below. In fact, they are just 
overloaded versions of 5 functions in total.

> [!TIP]
> All methods and parameters descriptions are also available within the code as comment for IDEs.

```cpp
- getRequest
    - std::future<HttpResult> getRequest(const std::string &url)
    - std::future<HttpResult> getRequest(const std::string &url, bool returnAsBinary)
    - std::future<HttpResult> getRequest(const std::string &url, const std::map<std::string, std::string> &headers)
    - std::future<HttpResult> getRequest(const std::string &url, bool returnAsBinary, const std::map<std::string, std::string> &headers)


- postRequest
    - std::future<HttpResult> postRequest(const std::string &url)
    - std::future<HttpResult> postRequest(const std::string &url, const std::string &payload)
    - std::future<HttpResult> postRequest(const std::string &url, bool returnAsBinary)
    - std::future<HttpResult> postRequest(const std::string &url, const std::map<std::string, std::string> &headers)
    - std::future<HttpResult> postRequest(const std::string &url, const std::string &payload, bool returnAsBinary)
    - std::future<HttpResult> postRequest(const std::string &url, const std::string &payload, const std::map<std::string, std::string> &headers)
    - std::future<HttpResult> postRequest(const std::string &url, bool returnAsBinary, const std::map<std::string, std::string> &headers)
    - std::future<HttpResult> postRequest(const std::string &url, const std::string &payload, bool returnAsBinary, const std::map<std::string, std::string> &headers)


- putRequest
    - std::future<HttpResult> putRequest(const std::string &url)
    - std::future<HttpResult> putRequest(const std::string &url, const std::string &payload)
    - std::future<HttpResult> putRequest(const std::string &url, bool returnAsBinary)
    - std::future<HttpResult> putRequest(const std::string &url, const std::map<std::string, std::string> &headers)
    - std::future<HttpResult> putRequest(const std::string &url, const std::string &payload, bool returnAsBinary)
    - std::future<HttpResult> putRequest(const std::string &url, const std::string &payload, const std::map<std::string, std::string> &headers)
    - std::future<HttpResult> putRequest(const std::string &url, bool returnAsBinary, const std::map<std::string, std::string> &headers)
    - std::future<HttpResult> putRequest(const std::string &url, const std::string &payload, bool returnAsBinary, const std::map<std::string, std::string> &headers)


- deleteRequest
    - std::future<HttpResult> deleteRequest(const std::string &url)
    - std::future<HttpResult> deleteRequest(const std::string &url, const std::string &payload)
    - std::future<HttpResult> deleteRequest(const std::string &url, bool returnAsBinary)
    - std::future<HttpResult> deleteRequest(const std::string &url, const std::map<std::string, std::string> &headers)
    - std::future<HttpResult> deleteRequest(const std::string &url, const std::string &payload, bool returnAsBinary)
    - std::future<HttpResult> deleteRequest(const std::string &url, const std::string &payload, const std::map<std::string, std::string> &headers)
    - std::future<HttpResult> deleteRequest(const std::string &url, bool returnAsBinary, const std::map<std::string, std::string> &headers)
    - std::future<HttpResult> deleteRequest(const std::string &url, const std::string &payload, bool returnAsBinary, const std::map<std::string, std::string> &headers)


- patchRequest
    - std::future<HttpResult> patchRequest(const std::string &url)
    - std::future<HttpResult> patchRequest(const std::string &url, bool returnAsBinary)
    - std::future<HttpResult> patchRequest(const std::string &url, const std::map<std::string, std::string> &headers)
    - std::future<HttpResult> patchRequest(const std::string &url, bool returnAsBinary, const std::map<std::string, std::string> &headers)
```


## License

MIT License

Copyright (c) 2024 lk-libs, Levent KARAGÖL

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


## Contact

If you have problems regarding the library, please open an 
[issue on GitHub](https://github.com/lk-libs/libcpp-http-client/issues/new). 
Please describe your request, issue, or question in as much detail as possible 
and also include the version of your compiler and operating system, as well as 
the version of the library you are using. Before opening a new issue, please 
confirm that the topic is not already exists in closed issues.