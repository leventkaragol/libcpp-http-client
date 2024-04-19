# libcpp-http-client
Modern, non-blocking and exception free, header-only HTTP Client library for C++ (17+)

[![linux](https://github.com/lk-libs/libcpp-http-client/actions/workflows/linux.yml/badge.svg)](https://github.com/lk-libs/libcpp-http-client/actions/workflows/linux.yml)
[![windows](https://github.com/lk-libs/libcpp-http-client/actions/workflows/windows.yml/badge.svg)](https://github.com/lk-libs/libcpp-http-client/actions/workflows/windows.yml)


> [!NOTE]
>  Please read this document before using the library. I know you don't have time, but reading this document will save you time. What I call a document is just this file. Trial and error will cost you more time.

# Table of Contents
* [How to add it to my project](#how-to-add-it-to-my-project)
* [How to use? (Simplest way)](#how-to-use-simplest-way)
* [What does exception free mean?](#what-does-exception-free-mean)
* [What does non-blocking mean?](#what-does-non-blocking-mean)
* [What about binary data?](#what-about-binary-data)
* [Sending Custom HTTP Headers](#sending-custom-http-headers)
* [POST request with form data](#post-request-with-form-data)
* [POST request with JSON data](#post-request-with-json-data)
* [What about others? (PUT, DELETE, PATCH)](#what-about-others-put-delete-patch)
* [License](#license)
* [Contact](#contact)


## How to add it to my project?

This is a header only library. So actually, all you need is to add the libcpp-http-client.hpp file to your project and start using it with #include.

But this library is a kind of Curl wrapper that uses Curl under the hood. So, you need to add Curl to your project before to use it.

You can find usage examples in the examples folder. You can also find a sample CMakeLists.txt file content below.


```cmake
cmake_minimum_required(VERSION 3.14)

project(myProject)

add_executable(myProject main.cpp)

find_package(CURL CONFIG REQUIRED)

target_link_libraries(myProject PRIVATE libcpp-http-client CURL::libcurl)

```


## How to use? (Simplest way)

Below you can see the simplest use case sending QueryString parameters to an API via HTTP GET.

> [!TIP]
> Please do not use it this way, if more than one call will be made . You do not use the non-blocking feature in this way.


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

## What does exception free mean?

Exception Free means that no exception will be thrown for any call you make to this library. If the URL cannot be found, there is a timeout, there is an authorization problem or another error occurs on the server, the bool typed "succeed" field of the response is returned as false. In addition, the HTTP Status Code value returned from the server is returned in the int typed "statusCode" field and possibly additional error information that may be returned from the server is returned in the string typed "errorMessage" field.

You can see an example use case below...

```cpp
#include <fstream>
#include "libcpp-http-client.hpp"

using namespace lklibs;

int main() {
    
    auto response = HttpClient::getRequest("https://www.myinvalidurl.com").get();

    // Instead of throwing an exception, the succeed field of the response object is set to false
    std::cout << "Succeed: " << response.succeed << std::endl;

    // And the http status code is set to the statusCode field
    std::cout << "Http Status Code: " << response.statusCode << std::endl;

    // Also if any error message is available, it is set to the errorMessage field
    std::cout << "Error Message: " << response.errorMessage << std::endl;

    return 0;
}
```

## What does non-blocking mean?

## What about binary data?

## Sending Custom HTTP Headers

## POST request with form data

## POST request with JSON data

## What about others? (PUT, DELETE, PATCH)

## License

## Contact
