/*

Modern non-blocking HTTP Client library for C++ (17+)
version 1.2.0
https://github.com/lk-libs/libcpp-http-client

If you encounter any issues, please submit a ticket at https://github.com/lk-libs/libcpp-http-client/issues

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

*/

#ifndef LIBCPP_HTTP_CLIENT_HPP
#define LIBCPP_HTTP_CLIENT_HPP

#include <iostream>
#include <string>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <cstdlib>
#include <curl/curl.h>

namespace lklibs {

    /**
     * @brief Contains the result of HTTP requests
     */
    class HttpResult {
    public:
        /**
         * @brief Information on whether the request was successful or not
         */
        bool succeed = false;

        /**
         * @brief HTTP status code resulting from the request
         */
        int statusCode = 0;

        /**
         * @brief Text data received as a result of the request
         */
        std::string textData;

        /**
         * @brief Binary data received as a result of the request
         */
        std::vector<unsigned char> binaryData;

        /**
         * @brief Error message received as a result of the request
         */
        std::string errorMessage;

        HttpResult() = default;

        HttpResult(bool succeed, std::string textData, std::vector<unsigned char> binaryData, int statusCode, std::string errorMessage)
                : succeed(succeed), textData(std::move(textData)), binaryData(std::move(binaryData)), statusCode(statusCode), errorMessage(std::move(errorMessage)) {}
    };

    /**
     * @brief HTTP Method options for the request
     */
    enum class HttpMethod {
        GET,
        POST,
        PUT,
        DELETE_,
        PATCH
    };

    /**
     * @brief Class to initialize and cleanup the curl library
     */
    class CurlGlobalInitializer {
    public:
        /**
         * @brief Static initialize method that ensures that curl is initialized only once when it is first used in the program
         */
        static void initialize() {

            static bool initialized = false;

            static std::mutex init_mutex;

            std::lock_guard<std::mutex> lock(init_mutex);

            if (!initialized) {

                CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT);

                if (result == CURLE_OK) {

                    initialized = true;

                    // Cleanup is called at program exit.
                    std::atexit(cleanup);
                }
            }
        }

    private:

        static void cleanup() {

            curl_global_cleanup();
        }
    };

    /**
     * @brief HTTP request class that makes asynchronous HTTP calls
     */
    class HttpRequest {
    public:

        /**
         * @brief Constructor for the HttpRequest class
         *
         * @param url: URL for the request
         */
        explicit HttpRequest(const std::string &url) {

            this->url = url;

            CurlGlobalInitializer::initialize();
        }

        /**
         * @brief Set the HTTP method for the request
         *
         * @param method: HTTP method to be used for the request
         */
        HttpRequest &setMethod(const HttpMethod &method) noexcept {

            this->method = HttpMethodStrings[static_cast<int>(method)];

            return *this;
        }

        /**
         * @brief Set the query string for the request
         *
         * @param queryString: Query string to be sent with the request
         */
        HttpRequest &setQueryString(const std::string &queryString) noexcept {

            if (this->url.find('?') != std::string::npos) {

                this->url += "&" + queryString;

            } else {

                this->url += "?" + queryString;
            }

            return *this;
        }

        /**
         * @brief Set the payload for the request
         * You can send form data like param1=7&param2=test or JSON data like {"param1": 7, "param2": "test"}
         * You need to send the "Content-Type" as "application/json" in the HTTP Header, if you need to send json data in the payload
         *
         * @param payload: Payload to be sent with the request
         */
        HttpRequest &setPayload(const std::string &payload) noexcept {

            this->payload = payload;

            return *this;
        }

        /**
         * @brief Set the return format for the request as binary
         */
        HttpRequest &returnAsBinary() noexcept {

            this->returnFormat = ReturnFormat::BINARY;

            return *this;
        }

        /**
         * @brief Ignore SSL errors when making HTTP requests
         */
        HttpRequest &ignoreSslErrors() noexcept {

            this->sslErrorsWillBeIgnored = true;

            return *this;
        }

        /**
         * @brief Add a HTTP header to the request
         *
         * @param key: Header key
         * @param value: Header value
         */
        HttpRequest &addHeader(const std::string &key, const std::string &value) noexcept {

            this->headers[key] = value;

            return *this;
        }

        /**
         * @brief Set the download bandwidth limit for the request
         *
         * @param limit: Download bandwidth limit in bytes per second (0 for no limit)
         */
        HttpRequest &setDownloadBandwidthLimit(int limit) noexcept {

            this->downloadBandwidthLimit = limit;

            return *this;
        }

        /**
         * @brief Set the upload bandwidth limit for the request
         *
         * @param limit: Upload bandwidth limit in bytes per second (0 for no limit)
         */
        HttpRequest &setUploadBandwidthLimit(int limit) noexcept {

            this->uploadBandwidthLimit = limit;

            return *this;
        }

        /**
         * @brief Send the HTTP request and return the result as a future
         * The result can be obtained by calling the get() method of the future
         * get() method will block until the result is available so it is recommended to use it
         * when you need the result and no more other http requests will be made as parallel
         *
         * @return Result of the request as a future (see HttpResult object for details)
         */
        std::future<HttpResult> send() noexcept {

            return this->sendRequest();
        }

    private:

        enum class ReturnFormat {
            TEXT,
            BINARY
        };

        const char* HttpMethodStrings[5] = {
                "GET",
                "POST",
                "PUT",
                "DELETE",
                "PATCH"
        };

        std::string url;
        std::string method = "GET";
        std::string payload;
        bool sslErrorsWillBeIgnored = false;
        ReturnFormat returnFormat = ReturnFormat::TEXT;
        std::map<std::string, std::string> headers;
        int uploadBandwidthLimit = 0;
        int downloadBandwidthLimit = 0;

        struct CurlDeleter {

            void operator()(CURL* ptr) {

                if (ptr) {

                    curl_easy_cleanup(ptr);
                }
            }
        };

        struct CurlSlistDeleter {

            void operator()(curl_slist* ptr) {

                if (ptr) {

                    curl_slist_free_all(ptr);
                }
            }
        };

        std::future<HttpResult> sendRequest() noexcept {

            return std::async(std::launch::async, [this]() -> HttpResult {

                std::unique_ptr<CURL, CurlDeleter> curl(curl_easy_init());

                if (!curl) {

                    return {false, "", {}, 0, "CURL initialization failed"};
                }

                std::unique_ptr<curl_slist, CurlSlistDeleter> headerList(nullptr);

                for (const auto &header: this->headers) {

                    std::string headerStr = header.first + ": " + header.second;

                    headerList.reset(curl_slist_append(headerList.release(), headerStr.c_str()));
                }

                std::string stringBuffer;
                std::vector<unsigned char> binaryBuffer;
                int statusCode = 0;

                curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headerList.get());
                curl_easy_setopt(curl.get(), CURLOPT_URL, this->url.c_str());
                curl_easy_setopt(curl.get(), CURLOPT_CUSTOMREQUEST, this->method.c_str());
                curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, this->sslErrorsWillBeIgnored ? 0L : 1L);
                curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYHOST, this->sslErrorsWillBeIgnored ? 0L : 1L);
                curl_easy_setopt(curl.get(), CURLOPT_MAX_SEND_SPEED_LARGE, this->uploadBandwidthLimit);
                curl_easy_setopt(curl.get(), CURLOPT_MAX_RECV_SPEED_LARGE, this->downloadBandwidthLimit);


                if (!this->payload.empty()) {

                    curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, this->payload.c_str());
                }

                if (this->returnFormat == ReturnFormat::BINARY) {

                    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, binaryWriteCallback);
                    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &binaryBuffer);

                } else {

                    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, textWriteCallback);
                    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &stringBuffer);
                }

                CURLcode res = curl_easy_perform(curl.get());
                curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &statusCode);

                if (res == CURLE_OK && statusCode >= 200 && statusCode < 300) {

                    return {true, stringBuffer, binaryBuffer, statusCode, ""};

                } else {

                    std::string errorMessage = curl_easy_strerror(res);

                    if (res == CURLE_OK) {

                        errorMessage = "HTTP Error: " + std::to_string(statusCode);
                    }

                    return {false, stringBuffer, binaryBuffer, statusCode, errorMessage};
                }
            });
        }

        static size_t textWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {

            ((std::string*) userp)->append((char*) contents, size * nmemb);

            return size * nmemb;
        }

        static size_t binaryWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {

            auto &buffer = *static_cast<std::vector<unsigned char>*>(userp);

            auto* data = static_cast<unsigned char*>(contents);

            buffer.insert(buffer.end(), data, data + size * nmemb);

            return size * nmemb;
        }
    };
}

#endif //LIBCPP_HTTP_CLIENT_HPP
