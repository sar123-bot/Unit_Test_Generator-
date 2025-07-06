## 🧪 Test Improvement Recommendations

### 1. 🚫 **Missing Coverage**
- **AuthController.cc:45-47** — Add test for invalid user object rejection
- **JobsController::updateJob()** — Edge case: empty title or missing ID
- **Department::setBudget()** — Test with `budget = -1`

---

### 2. 🧬 **Edge Case Tests**
- Send malformed JSON body to all endpoints
- Simulate SQL injection in query parameters
- Empty strings in `PersonInfo` name fields

---

### 3. 🔁 **Duplicate Tests**
- `test_AuthController.cc` login tested twice: lines 101–115 and 210–223

---

### 4. ✅ **Assertion Enhancements**
- Use `EXPECT_EQ(response.status_code, 400)` instead of `EXPECT_TRUE(res.bad())`
- Validate JSON error messages in response body

---

### 5. 🧹 **Organization Tips**
- Separate test files for controllers vs models
- Use fixtures to reduce boilerplate setup code
