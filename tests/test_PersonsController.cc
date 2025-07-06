#include <gtest/gtest.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <json/json.h>
#include "../dot_h/PersonsController.h"
#include "../models/Person.h"

using namespace drogon;

class PersonsControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup common request objects
        Json::Value personJson;
        personJson["name"] = "Test Person";
        personJson["email"] = "test@example.com";
        createReq = HttpRequest::newHttpJsonRequest(personJson);
        
        updateReq = HttpRequest::newHttpJsonRequest(personJson);
    }

    HttpRequestPtr createReq;
    HttpRequestPtr updateReq;
};

TEST_F(PersonsControllerTest, GetPersons_Success) {
    PersonsController controller;
    HttpResponsePtr result;
    
    auto req = HttpRequest::newHttpRequest();
    req->setParameter("limit", "10");
    req->setParameter("offset", "0");
    
    controller.get(req, [&result](const HttpResponsePtr &resp) {
        result = resp;
    });
    
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->statusCode(), HttpStatusCode::k200OK);
    
    Json::Value json;
    Json::Reader reader;
    std::string body = std::string(result->body());
    ASSERT_TRUE(reader.parse(body, json));
    EXPECT_FALSE(json.empty());
}

TEST_F(PersonsControllerTest, GetOnePerson_Found) {
    PersonsController controller;
    HttpResponsePtr result;
    
    auto req = HttpRequest::newHttpRequest();
    controller.getOne(req, [&result](const HttpResponsePtr &resp) {
        result = resp;
    }, 1); // personId = 1
    
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->getStatusCode() == HttpStatusCode::k200OK || 
               result->getStatusCode() == HttpStatusCode::k404NotFound);
}

TEST_F(PersonsControllerTest, CreatePerson_Success) {
    PersonsController controller;
    HttpResponsePtr result;
    
    Json::Value personJson;
    personJson["name"] = "New Person";
    personJson["email"] = "new@example.com";
    auto req = HttpRequest::newHttpJsonRequest(personJson);
    
    Person person(personJson);
    controller.createOne(req, [&result](const HttpResponsePtr &resp) {
        result = resp;
    }, std::move(person));
    
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->statusCode(), HttpStatusCode::k201Created);
}

TEST_F(PersonsControllerTest, UpdatePerson_Success) {
    PersonsController controller;
    HttpResponsePtr result;
    
    Json::Value personJson;
    personJson["name"] = "Updated Person";
    auto req = HttpRequest::newHttpJsonRequest(personJson);
    
    Person person(personJson);
    controller.updateOne(req, [&result](const HttpResponsePtr &resp) {
        result = resp;
    }, 1, std::move(person)); // personId = 1
    
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->getStatusCode() == HttpStatusCode::k200OK || 
               result->getStatusCode() == HttpStatusCode::k404NotFound);
}

TEST_F(PersonsControllerTest, DeletePerson_Success) {
    PersonsController controller;
    HttpResponsePtr result;
    
    auto req = HttpRequest::newHttpRequest();
    controller.deleteOne(req, [&result](const HttpResponsePtr &resp) {
        result = resp;
    }, 1); // personId = 1
    
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->getStatusCode() == HttpStatusCode::k204NoContent || 
               result->getStatusCode() == HttpStatusCode::k404NotFound);
}