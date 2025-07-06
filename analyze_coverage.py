#!/usr/bin/env python3
import subprocess
from llm_integration import query_llm  # Your existing LLM interface

def analyze_coverage():
    print("🔍 Analyzing coverage results...")
    
    # Extract coverage summary
    result = subprocess.run([
        "lcov", "--summary", "filtered_coverage.info"
    ], capture_output=True, text=True, check=True)
    
    # Prepare LLM prompt
    prompt = f"""Analyze these test coverage results:
{result.stdout}

Suggest specific improvements:
1. List untested code with file:line numbers
2. Recommend new test cases for edge conditions
3. Identify potential duplicate assertions
4. Suggest better test organization"""
    
    # Get LLM recommendations
    improvements = query_llm(prompt)
    
    # Save to file
    with open("test_improvements.md", "w") as f:
        f.write(improvements)
    
    print("💡 Improvement suggestions saved to test_improvements.md")

if __name__ == "__main__":
    analyze_coverage()