#include <gtest/gtest.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <json/json.h>
#include "../dot_h/AuthController.h"
#include "../models/User.h"
#include <bcrypt/BCrypt.hpp>

using namespace drogon;

class AuthControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test user
        Json::Value userJson;
        userJson["username"] = "testuser";
        userJson["password"] = "testpass";
        testUser = std::make_unique<User>(userJson);
    }

    void TearDown() override {
        testUser.reset();
    }

    std::unique_ptr<User> testUser;
};

TEST_F(AuthControllerTest, registerUser_success) {
    Json::Value json;
    json["username"] = "newuser";
    json["password"] = "newpass";
    auto req = HttpRequest::newHttpJsonRequest(json);

    AuthController controller;
    HttpResponsePtr result;
    
    // Create a temporary user for registration
    Json::Value newUserJson;
    newUserJson["username"] = "newuser";
    newUserJson["password"] = "newpass";
    User newUser(newUserJson);

    controller.registerUser(req, [&](const HttpResponsePtr &resp) {
        result = resp;
    }, std::move(newUser));

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->statusCode(), HttpStatusCode::k201Created);
}

TEST_F(AuthControllerTest, registerUser_username_taken) {
    Json::Value json;
    json["username"] = "testuser";  // Already exists
    json["password"] = "testpass";
    auto req = HttpRequest::newHttpJsonRequest(json);

    AuthController controller;
    HttpResponsePtr result;
    
    // Using the test user that already exists
    controller.registerUser(req, [&](const HttpResponsePtr &resp) {
        result = resp;
    }, User(*testUser));

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->statusCode(), HttpStatusCode::k400BadRequest);
}

TEST_F(AuthControllerTest, loginUser_ok) {
    Json::Value json;
    json["username"] = "testuser";
    json["password"] = "testpass";
    auto req = HttpRequest::newHttpJsonRequest(json);

    AuthController controller;
    HttpResponsePtr result;
    
    controller.loginUser(req, [&](const HttpResponsePtr &resp) {
        result = resp;
    }, User(*testUser));

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->statusCode(), HttpStatusCode::k200OK);
}

TEST_F(AuthControllerTest, loginUser_wrong_password) {
    Json::Value json;
    json["username"] = "testuser";
    json["password"] = "wrongpass";
    auto req = HttpRequest::newHttpJsonRequest(json);

    AuthController controller;
    HttpResponsePtr result;
    
    controller.loginUser(req, [&](const HttpResponsePtr &resp) {
        result = resp;
    }, User(*testUser));

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->statusCode(), HttpStatusCode::k401Unauthorized);
}

TEST_F(AuthControllerTest, loginUser_user_not_found) {
    Json::Value json;
    json["username"] = "nonexistent";
    json["password"] = "testpass";
    auto req = HttpRequest::newHttpJsonRequest(json);

    AuthController controller;
    HttpResponsePtr result;
    
    // Create a temporary unknown user
    Json::Value unknownUserJson;
    unknownUserJson["username"] = "nonexistent";
    unknownUserJson["password"] = "testpass";
    User unknownUser(unknownUserJson);
    
    controller.loginUser(req, [&](const HttpResponsePtr &resp) {
        result = resp;
    }, std::move(unknownUser));

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->statusCode(), HttpStatusCode::k400BadRequest);
}

// Note: Removed tests for private methods areFieldsValid and isPasswordValid
// as they shouldn't be tested directly