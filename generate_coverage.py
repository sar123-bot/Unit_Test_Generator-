#!/usr/bin/env python3
import subprocess
import os

def generate_coverage():
    print("⏳ Generating coverage data...")
    
    # Run lcov to capture data
    subprocess.run([
        "lcov", "--capture",
        "--directory", "build/",
        "--output-file", "coverage.info"
    ], check=True)
    
    # Filter coverage data
    subprocess.run([
        "lcov", "--remove", "coverage.info",
        "'/usr/*'", "'*/external/*'", "'*/third_party/*'", "'*/tests/*'",
        "--output-file", "filtered_coverage.info"
    ], check=True)
    
    # Generate HTML report
    subprocess.run([
        "genhtml", "filtered_coverage.info",
        "--output-directory", "coverage_report"
    ], check=True)
    
    print("✅ Coverage report generated at coverage_report/index.html")

if __name__ == "__main__":
    generate_coverage()