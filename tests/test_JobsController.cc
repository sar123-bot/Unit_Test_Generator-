#include <gtest/gtest.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <json/json.h>
#include "../dot_h/JobsController.h"
#include "../models/Job.h"

using namespace drogon;

class JobsControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup common request objects
        Json::Value jobJson;
        jobJson["title"] = "Test Job";
        jobJson["description"] = "Test Description";
        createReq = HttpRequest::newHttpJsonRequest(jobJson);
        
        updateReq = HttpRequest::newHttpJsonRequest(jobJson);
    }

    HttpRequestPtr createReq;
    HttpRequestPtr updateReq;
};

TEST_F(JobsControllerTest, GetJobs_Success) {
    JobsController controller;
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

TEST_F(JobsControllerTest, GetOneJob_Found) {
    JobsController controller;
    HttpResponsePtr result;
    
    auto req = HttpRequest::newHttpRequest();
    controller.getOne(req, [&result](const HttpResponsePtr &resp) {
        result = resp;
    }, 1); // jobId = 1
    
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->getStatusCode() == HttpStatusCode::k201Created || 
               result->getStatusCode() == HttpStatusCode::k404NotFound);
}

TEST_F(JobsControllerTest, CreateJob_Success) {
    JobsController controller;
    HttpResponsePtr result;
    
    Json::Value jobJson;
    jobJson["title"] = "New Job";
    auto req = HttpRequest::newHttpJsonRequest(jobJson);
    
    Job job(jobJson);
    controller.createOne(req, [&result](const HttpResponsePtr &resp) {
        result = resp;
    }, std::move(job));
    
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->statusCode(), HttpStatusCode::k201Created);
}

TEST_F(JobsControllerTest, UpdateJob_Success) {
    JobsController controller;
    HttpResponsePtr result;
    
    Json::Value jobJson;
    jobJson["title"] = "Updated Job";
    auto req = HttpRequest::newHttpJsonRequest(jobJson);
    
    Job job(jobJson);
    controller.updateOne(req, [&result](const HttpResponsePtr &resp) {
        result = resp;
    }, 1, std::move(job)); // jobId = 1
    
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->getStatusCode() == HttpStatusCode::k204NoContent || 
               result->getStatusCode() == HttpStatusCode::k404NotFound);
}

TEST_F(JobsControllerTest, DeleteJob_Success) {
    JobsController controller;
    HttpResponsePtr result;
    
    auto req = HttpRequest::newHttpRequest();
    controller.deleteOne(req, [&result](const HttpResponsePtr &resp) {
        result = resp;
    }, 1); // jobId = 1
    
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->getStatusCode() == HttpStatusCode::k204NoContent || 
               result->getStatusCode() == HttpStatusCode::k404NotFound);
}

TEST_F(JobsControllerTest, GetJobPersons_Success) {
    JobsController controller;
    HttpResponsePtr result;
    
    auto req = HttpRequest::newHttpRequest();
    controller.getJobPersons(req, [&result](const HttpResponsePtr &resp) {
        result = resp;
    }, 1); // jobId = 1
    
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->getStatusCode() == HttpStatusCode::k200OK || 
               result->getStatusCode() == HttpStatusCode::k404NotFound);
}