#!/bin/bash
# Run this after successful build & tests

# Capture coverage data
lcov --capture --directory build/ --output-file coverage.info

# Remove system/external files from report
lcov --remove coverage.info \
    '/usr/*' \
    '*/external/*' \
    '*/third_party/*' \
    '*/tests/*' \
    -o filtered_coverage.info

# Generate HTML report
genhtml filtered_coverage.info --output-directory coverage_report

# Extract summary for LLM
lcov --summary filtered_coverage.info > coverage_summary.txt
grep -B 2 -A 2 "lines.*%" coverage_summary.txt > llm_coverage_feedback.txt

# Find uncovered lines
uncovered=$(grep -L "DA:" $(find build/ -name "*.gcda" | sed 's/gcda/gcno/') | xargs -I {} grep -n -B 1 -A 1 "0$" {})
echo "Uncovered lines:\n$uncovered" >> llm_coverage_feedback.txt

# Prepare prompt for LLM
cat << EOF > llm_prompt.txt
Analyze these test coverage results and suggest improvements:
1. Increase coverage for uncovered code
2. Remove duplicate tests
3. Add edge cases
4. Improve test organization

$(cat llm_coverage_feedback.txt)
EOF