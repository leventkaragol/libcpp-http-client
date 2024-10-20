# libcpp-http-client

Modern, non-blocking and exception free HTTP Client library for C++ (17+)

[![linux](https://github.com/leventkaragol/libcpp-http-client/actions/workflows/linux.yml/badge.svg)](https://github.com/leventkaragol/libcpp-http-client/actions/workflows/linux.yml)
[![windows](https://github.com/leventkaragol/libcpp-http-client/actions/workflows/windows.yml/badge.svg)](https://github.com/leventkaragol/libcpp-http-client/actions/workflows/windows.yml)


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
* [How to ignore SSL certificate errors?](#how-to-ignore-ssl-certificate-errors)
* [Setting the TLS version](#setting-the-tls-version)
* [How to set timeout?](#how-to-set-timeout)
* [Setting the User Agent](#setting-the-user-agent)
* [How can I limit download and upload bandwidth?](#how-can-i-limit-download-and-upload-bandwidth)
* [Semantic Versioning](#semantic-versioning)
* [Full function list](#full-function-list)
* [License](#license)
* [Contact](#contact)


## How to add it to my project?

This is a header only library. So actually, all you need is to add the libcpp-http-client.hpp file 
in src folder to your project and start using it with #include.

But this library is a kind of Curl wrapper that uses Curl under the hood. So, you also need to add Curl to 
your project before to use it.

You can find usage examples in the examples folder, also find a sample CMakeLists.txt file content below.

```cmake
cmake_minimum_required(VERSION 3.14)

project(myProject)

find_package(CURL CONFIG REQUIRED)

add_executable(myProject main.cpp libcpp-http-client.hpp)

target_link_libraries(myProject PRIVATE CURL::libcurl)

```


## How to use? (Simplest way)

Below you can see the simplest use case sending QueryString parameters to an API via HTTP GET.

> [!IMPORTANT]
> Please do not use it this way, if more than one call will be made. You do not use the non-blocking
> feature in this way. Just keep reading...

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {

    HttpRequest httpRequest("https://api.myproject.com");

    // The simplest but slowest method if multiple calls will be made
    auto response = httpRequest
            .setQueryString("param1=7&param2=test")
            .send()
            .get();

    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;
    std::cout << "Data: " << response.textData << std::endl;

    return 0;
}
```


## What does non-blocking mean?

Let's talk about this through an example. Let's say, we call 5 different API methods when our 
screen opens and each one takes an average of 500 ms. If we call these methods one after another 
as follows, we will get all the answers in 2.5 seconds in total.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
    HttpRequest httpRequest1("https://api.myproject.com/foo");
    HttpRequest httpRequest2("https://api.myproject.com/bar");
    HttpRequest httpRequest3("https://api.myproject.com/baz");
    HttpRequest httpRequest4("https://api.myproject.com/qux");
    HttpRequest httpRequest5("https://api.myproject.com/quux");

    auto response1 = httpRequest1.send().get();
    auto response2 = httpRequest2.send().get();
    auto response3 = httpRequest3.send().get();
    auto response4 = httpRequest4.send().get();
    auto response5 = httpRequest5.send().get();

    // Takes 2.5 seconds in total

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
    
    HttpRequest httpRequest1("https://api.myproject.com/foo");
    HttpRequest httpRequest2("https://api.myproject.com/bar");
    HttpRequest httpRequest3("https://api.myproject.com/baz");
    HttpRequest httpRequest4("https://api.myproject.com/qux");
    HttpRequest httpRequest5("https://api.myproject.com/quux");

    auto future1 = httpRequest.send();
    auto future2 = httpRequest.send();
    auto future3 = httpRequest.send();
    auto future4 = httpRequest.send();
    auto future5 = httpRequest.send();
    
    auto response1 = future1.get();
    auto response2 = future2.get();
    auto response3 = future3.get();
    auto response4 = future4.get();
    auto response5 = future5.get();

    // Takes 0.5 seconds in total

    return 0;
}
```

**"send"** function in the library return a future and allow the next line to run without blocking the flow.


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
    
    HttpRequest httpRequest("https://www.myinvalidurl.com");

    auto response = httpRequest.send().get();

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
***"std::vector&lt;unsigned char&gt;"*** instead of **"textData"** by calling **"returnAsBinary()"** method before send as follow.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
    HttpRequest httpRequest("https://api.myproject.com/image/7");

    // If you need to retrieve binary data such as an image, just call the "returnAsBinary" method before send
    auto response = httpRequest
            .returnAsBinary()
            .send()
            .get();
    
    std::cout << "Succeed: " << response.succeed << std::endl;
    std::cout << "Http Status Code: " << response.statusCode << std::endl;

    // In this case, you can get the data via binaryData instead of textData
    std::cout << "Data Size: " << response.binaryData.size() << std::endl;

    return 0;
}
```


## Sending custom HTTP headers

If you need to send custom HTTP HEADERs during the request, you can add them to the request as key-value pairs with **"addHeader()"** method.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
    HttpRequest httpRequest("https://api.myproject.com");

    // You can send custom headers as key-value pairs
    auto response = httpRequest
            .addHeader("Custom-Header1", "value1")
            .addHeader("Custom-Header2", "value2")
            .send()
            .get();
    
    std::cout << "Succeed: " << response.succeed << std::endl;

    return 0;
}
```


## POST request with form data

Next is submitting form data via HTTP POST. All you have to do is use **"setMethod"** to change HTTP method type. You can pass the form data with **"setPaylod"** method as seen in the sample code below.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
    HttpRequest httpRequest("https://api.myproject.com");

    // You can send a POST request with form data in the payload
    auto response = httpRequest
            .setMethod(HttpMethod::POST)
            .setPayload("param1=7&param2=test")
            .send()
            .get();

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
    
    HttpRequest httpRequest("https://api.myproject.com");
    
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
    
    HttpRequest httpRequest1("https://api.myproject.com");

    auto future1 = httpRequest
            .setMethod(HttpMethod::PUT)
            .setPayload("param1=7&param2=test")
            .send();
    
    HttpRequest httpRequest2("https://api.myproject.com");

    auto future2 = httpRequest
            .setMethod(HttpMethod::DELETE_)
            .setPayload("param1=7&param2=test")
            .send();
    
    HttpRequest httpRequest3("https://api.myproject.com");
    
    auto future3 = httpRequest
            .setMethod(HttpMethod::PATCH)
            .setQueryString("param1=7&param2=test")
            .send();

    auto response1 = future1.get();
    auto response2 = future2.get();
    auto response3 = future3.get();

    return 0;
}
```


## How to ignore SSL certificate errors?

If you need to ignore SSL certificate errors for any valid reason, you can call "ignoreSslErrors" 
method before sending the request.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
    HttpRequest httpRequest("https://api.myinvalidssl.com");

    // If you need to ignore SSL errors, you can call "ignoreSslErrors" method before sending the request
    auto response = httpRequest
            .ignoreSslErrors()
            .send()
            .get();

    return 0;
}
```


## Setting the TLS version

You can set the TLS version used during the request with the setTLSVersion method

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    HttpRequest httpRequest("https://api.myproject.com");
    
    // You can set the TLS version to be used for the request with setTLSVersion method
    auto response = httpRequest
                    .setTLSVersion(TLSVersion::TLSv1_3)
                    .send()
                    .get();
    
    return 0;
}
```


## How to set timeout?

You can use the setTimeout method to set the timeout duration in seconds during requests.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    HttpRequest httpRequest("https://api.myproject.com");
    
    // You can set the timeout in seconds
    auto response = httpRequest
                    .setTimeout(3) // 3 sec
                    .send()
                    .get();
    
    return 0;
}
```


## Setting the User Agent

You can set the User Agent information to be sent during the request with the setUserAgent method.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    HttpRequest httpRequest("https://api.myproject.com");
    
    // You can set the user agent to be used for the request with setUserAgent method
    auto response = httpRequest
                    .setUserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36 Edg/124.0.0.0")
                    .send()
                    .get();
    
    return 0;
}
```


## How can I limit download and upload bandwidth?

If you do not want the bandwidth to exceed a certain limit during the download and upload process, you can determine the maximum limit that can be used in Bytes with the setDownloadBandwidthLimit and setUploadBandwidthLimit methods.

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    HttpRequest httpRequest("https://api.myproject.com");
    
    // You can set the download and upload bandwidth limit in bytes per second
    auto response = httpRequest
                    .setDownloadBandwidthLimit(10240) // 10 KB/sec
                    .setUploadBandwidthLimit(20480) // 20 KB/sec
                    .send()
                    .get();
    
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

You can find the complete list of functions in the library below. Since all methods except 
send return the class itself, so they can be added one after the other like a chain.

> [!TIP]
> All methods and parameters descriptions are also available within the code as comment for IDEs.

```cpp
HttpRequest& setMethod(const HttpMethod& method) noexcept;

HttpRequest& setQueryString(const std::string& queryString) noexcept;

HttpRequest& setPayload(const std::string& payload) noexcept;

HttpRequest& returnAsBinary() noexcept;

HttpRequest& addHeader(const std::string& key, const std::string& value) noexcept;

HttpRequest& setTimeout(const int timeout) noexcept;

HttpRequest& ignoreSslErrors() noexcept;

HttpRequest& setTLSVersion(const TLSVersion version) noexcept;

HttpRequest& setUserAgent(const std::string& userAgent) noexcept;

HttpRequest& setDownloadBandwidthLimit(const int limit) noexcept;

HttpRequest& setUploadBandwidthLimit(const int limit) noexcept;

std::future<HttpResult> send() noexcept;
```


## License

MIT License

Copyright (c) 2024 Levent KARAGÖL

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
[issue on GitHub](https://github.com/leventkaragol/libcpp-http-client/issues/new). 
Please describe your request, issue, or question in as much detail as possible 
and also include the version of your compiler and operating system, as well as 
the version of the library you are using. Before opening a new issue, please 
confirm that the topic is not already exists in closed issues.
