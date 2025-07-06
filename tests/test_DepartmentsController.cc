#include <gtest/gtest.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <json/json.h>
#include "../dot_h/DepartmentsController.h"

using namespace drogon;

class DepartmentsControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test request objects
        Json::Value json;
        json["name"] = "Test Department";
        json["description"] = "Test Description";
        testReq = HttpRequest::newHttpJsonRequest(json);
    }

    HttpRequestPtr testReq;
};

TEST_F(DepartmentsControllerTest, GetDepartmentPersons_Success) {
    DepartmentsController controller;
    HttpResponsePtr result;
    
    auto req = HttpRequest::newHttpRequest();
    controller.getDepartmentPersons(req, [&result](const HttpResponsePtr &resp) {
        result = resp;
    }, 1); // departmentId = 1
    
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->statusCode(), HttpStatusCode::k200OK);
}

// Note: The following test cases are commented out because your controller
// doesn't have these methods. Uncomment and implement if you add them.

/*
TEST_F(DepartmentsControllerTest, CreateDepartment_Success) {
    DepartmentsController controller;
    HttpResponsePtr result;
    
    controller.createDepartment(testReq, [&result](const HttpResponsePtr &resp) {
        result = resp;
    });
    
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->statusCode(), HttpStatusCode::k201Created);
}

TEST_F(DepartmentsControllerTest, GetDepartmentById_Success) {
    DepartmentsController controller;
    HttpResponsePtr result;
    
    auto req = HttpRequest::newHttpRequest();
    controller.getDepartmentById(req, [&result](const HttpResponsePtr &resp) {
        result = resp;
    }, 1);
    
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->statusCode(), HttpStatusCode::k200OK);
}

TEST_F(DepartmentsControllerTest, UpdateDepartment_Success) {
    DepartmentsController controller;
    HttpResponsePtr result;
    
    controller.updateDepartment(testReq, [&result](const HttpResponsePtr &resp) {
        result = resp;
    }, 1);
    
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->statusCode(), HttpStatusCode::k200OK);
}

TEST_F(DepartmentsControllerTest, DeleteDepartment_Success) {
    DepartmentsController controller;
    HttpResponsePtr result;
    
    auto req = HttpRequest::newHttpRequest();
    controller.deleteDepartment(req, [&result](const HttpResponsePtr &resp) {
        result = resp;
    }, 1);
    
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->statusCode(), HttpStatusCode::k204NoContent);
}
*/